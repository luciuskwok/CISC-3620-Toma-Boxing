// main.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <SDL.h>
#include <emscripten.h>

#include "color.h"
#include "matrix.h"
#include "mesh.h"
#include "vector.h"


// Constants
#define BLACK_COLOR (0xFF000000)
#define WHITE_COLOR (0xFFFFFFFF)
#define FPS (60)
#define FRAME_TARGET_TIME (1000 / FPS)
#define PIXELS_WIDTH (320)
#define PIXELS_HEIGHT (200)
#define PIXELS_SCALE (2)

#define M_PI_F ((float)(M_PI))
#define M_PI_2_F ((float)(M_PI * 2.0))

// Globals for SDL
SDL_Window* sdl_window;
SDL_Rect window_rect;
SDL_Renderer* sdl_renderer;
SDL_Texture* sdl_texture;
uint32_t* pixels;
int pixels_w, pixels_h;
uint64_t frame_index;


// Globals for 3D Cube
// Triangle parameters
triangle_t projected_triangles[M_MESH_FACES];
vec2_t projected_points[M_MESH_VERTICES];
uint32_t triangle_line_color;
uint32_t triangle_point_color;

// Transform
mat3_t transform_2d;
mat4_t transform_3d;
float angle;

// Momemtum
float pitch, roll, yaw; // how many degrees per second of rotation

// Camera
vec3_t camera_position = { 0.0f, 0.0f, -5.0f };


#pragma mark - 3D Drawing

vec2_t orthographic_project_point(vec3_t pt3d, float scale2d) {
    // Apply scaling and drop z
    vec2_t pt2d = { .x = pt3d.x * scale2d, .y = pt3d.y * scale2d };

    // Apply 2d matrix transform
    pt2d = vec2_mat3_multiply(pt2d, transform_2d);
    return pt2d;
}

vec2_t perspective_project_point(vec3_t pt3d, float scale2d) {
    // Apply 3d transform
    pt3d = vec3_mat4_multiply(pt3d, transform_3d);

    // Apply camera position
    pt3d = vec3_sub(pt3d, camera_position);

    vec2_t pt2d = { .x = pt3d.x / pt3d.z, .y = pt3d.y / pt3d.z };

    // Apply 2d transform
    pt2d = vec2_mat3_multiply(pt2d, transform_2d);

    // Scale and center on screen
    pt2d.x = pt2d.x * scale2d + pixels_w / 2;
    pt2d.y = pt2d.y * scale2d + pixels_h / 2;

    return pt2d;
}

void project_model(void) {
    // Project the 3d model into 2d space
    // Triangles
    const float scale2d = pixels_h;
    for (int f = 0; f < M_MESH_FACES; f++) {
        projected_triangles[f].a = perspective_project_point(cube_vertices[cube_faces[f].a], scale2d);
        projected_triangles[f].b = perspective_project_point(cube_vertices[cube_faces[f].b], scale2d);
        projected_triangles[f].c = perspective_project_point(cube_vertices[cube_faces[f].c], scale2d);
    }
    // Points
    for (int i = 0; i < M_MESH_VERTICES; i++) {
        projected_points[i] = perspective_project_point(cube_vertices[i], scale2d);
    }
}


#pragma mark - 2D Drawing

void clear_screen(uint32_t color) {
	for (int i = 0; i < pixels_w * pixels_h; i++) {
		pixels[i] = color;
	}
}

void set_pixel(int x, int y, uint32_t color) {
	if (x < 0 || x >= pixels_w) return;
	if (y < 0 || y >= pixels_h) return;
	
	// Apply blending if color's alpha < 255
	int i = x + y * pixels_w;
	if ((color & 0xFF000000) != 0xFF000000) {
		color = blend_color(pixels[i], color);
	}
	pixels[i] = color;
}

void draw_centered_rect(int x, int y, int w, int h, uint32_t color) {
	int top = y - h / 2;
	int left = x - w / 2;
	for (int y1 = 0; y1 < h; y1++) {
		for (int x1 = 0; x1 < w; x1++) {
			set_pixel(left + x1, top + y1, color);
		}
	}
}

void draw_line(vec2_t a, vec2_t b, uint32_t color) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	float steps = fabs(dx) > fabs(dy)? fabs(dx) : fabs(dy);
	float sx = dx / steps;
	float sy = dy / steps;
	float x = a.x;
	float y = a.y;
	for (float i = 0.0f; i <= steps; i++) {
		set_pixel(floorf(x), floorf(y), color);
		x += sx;
		y += sy;
	}
}

#pragma mark - Init & Clean Up

bool initialize_windowing_system(void) {
	//fprintf(stdout, "initialize_windowing_system().\n");

	// Set up SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init() failed!\n");
		return false;
	}

 	// Window & Renderer
 	window_rect.x = window_rect.y = 0;
	window_rect.w = PIXELS_WIDTH * PIXELS_SCALE;
	window_rect.h = PIXELS_HEIGHT * PIXELS_SCALE;
	if (SDL_CreateWindowAndRenderer(window_rect.w, window_rect.h, 0, &sdl_window, &sdl_renderer) != 0) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer() failed!\n");
		return false;
	}

	// Texture
	pixels_w = PIXELS_WIDTH;
	pixels_h = PIXELS_HEIGHT;
	sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING, pixels_w, pixels_h);
		// Using ABGR pixel format is slightly faster (~10%) than using RGBA.
	if (!sdl_texture) {
		fprintf(stderr, "SDL_CreateTexture() failed!\n");
		return false;
	}

	// Allocate frame buffer
	pixels = (uint32_t*)malloc(pixels_w * pixels_h * sizeof(uint32_t));
	if (!pixels) {
		fprintf(stderr, "malloc() failed!\n");
		return false;
	}
	clear_screen(BLACK_COLOR);
	
	// Set up the renderer
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); // Use no interpolation
	// Clear the window
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_RenderPresent(sdl_renderer);

	// Reset the frame index
	frame_index = 0;

	// Debug logging: window * texture size
	fprintf(stdout, "Created window (%dx%d) and texture (%dx%d).\n", window_rect.w, window_rect.h, pixels_w, pixels_h);

	return true;
}

void clean_up_windowing_system(void) {
	free(pixels);
	SDL_DestroyTexture(sdl_texture);
	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

#pragma mark - Game Loop

void process_keyboard_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	// Keyboard interaction
	switch (event.type) {
	case SDL_QUIT: // when 'X' button is pressed in window titlebar
		// Exit program
		//is_running = false;
		fprintf(stdout, "Quit requested.\n");
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				// Exit program
				//is_running = false;
				fprintf(stdout, "Escape key pressed.\n");
				break;
            case SDLK_0:
                // Reset all state variables and positions
                mat3_get_identity(transform_2d);
                mat4_get_identity(transform_3d);
                angle = 0;
                pitch = 0;
                roll = 0;
                yaw = 0;
                break;
            case SDLK_e:
                // Roll right
                roll += 1.0f;
                break;
            case SDLK_q:
                // Roll left
                roll -= 1.0f;
                break;
            case SDLK_w:
                // Pitch down
                pitch += 1.0f;
                break;
            case SDLK_s:
                // Pitch up
                pitch -= 1.0f;
                break;
            case SDLK_a:
                // Yaw left
                yaw += 1.0f;
                break;
            case SDLK_d:
                // Yaw right
                yaw -= 1.0f;
                break;
            case SDLK_SPACE:
                // Stop movement
                pitch = 0.0f;
                roll = 0.0f;
                yaw = 0.0f;
                break;
 		}
		break;
	}
}

void update_state(void) {
    // Time variable
    float t = (float)(frame_index % 480) / 480.0f;

    // Update rotation
    float increment = (M_PI_F / 180.f) / 60.0f; // 1 deg/sec divided by 60 fps
    mat4_pitch(transform_3d, pitch * increment);
    mat4_roll(transform_3d, roll * increment);
    mat4_yaw(transform_3d, yaw * increment);
    
    // Update triangle colors
    int hue = frame_index % 360;
    triangle_line_color = color_from_hsv(hue, 1.0, 1.0, 1.0);
    triangle_point_color = color_from_hsv((hue + 60) % 360, 1.0, 1.0, 0.5);

	// Update frame index
	frame_index++;
}

void run_render_pipeline(void) {
	// Clear frame buffer
	clear_screen(BLACK_COLOR);
	
	// Draw each triangle as lines for a wireframe
    triangle_t *tri;
    project_model();
    for (int i = 0; i < M_MESH_FACES; i++) {
        tri = &projected_triangles[i];
        draw_line(tri->a, tri->b, triangle_line_color);
        draw_line(tri->b, tri->c, triangle_line_color);
        draw_line(tri->c, tri->a, triangle_line_color);
    }
    
    // Draw each point as a 5x5 rectangle
    for (int i = 0; i < M_MESH_VERTICES; i++) {
        vec2_t pt = projected_points[i];
        draw_centered_rect(pt.x, pt.y, 5, 5, triangle_point_color);
    }

	// Render frame buffer
	SDL_UpdateTexture(sdl_texture, NULL, pixels, pixels_w * sizeof(uint32_t));
	SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, &window_rect);
	SDL_RenderPresent(sdl_renderer);
}

void game_loop() {
	// Run one iteration of game loop
	process_keyboard_input();
	update_state();
	run_render_pipeline();
}

int main(int argc, const char * argv[]) {
	if (!initialize_windowing_system()) return 0;

	// Init geometry & transforms
    mat3_get_identity(transform_2d);
    mat4_get_identity(transform_3d);
    angle = 0;
    pitch = 0;
    roll = 0;
    yaw = 0;


	emscripten_set_main_loop(game_loop, 0, 1);

	return 0;
}
