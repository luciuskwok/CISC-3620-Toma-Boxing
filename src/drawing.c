//
//  drawing.c
//  CISC-3620-Midterm
//
//  Created by Lucius Kwok on 3/24/24.
//

#include "drawing.h"
#include "color.h"
#include "matrix.h"
#include "cube_mesh.h"
#include "vector.h"

#include <SDL2/SDL.h>


// Globals for SDL
SDL_Window* sdl_window;
SDL_Rect window_rect;
SDL_Renderer* sdl_renderer;
SDL_Texture* sdl_texture;
uint32_t* pixels;
int pixels_w, pixels_h;

// Drawing context
uint32_t line_color;
uint32_t fill_color;
vec2_t cursor;

// Transforms
mat3_t view_transform_2d;
mat4_t camera_transform_3d;



#pragma mark - SDL Interface

bool init_window(int width, int height, int scale) {
	//fprintf(stdout, "initialize_windowing_system().\n");
	
	// Set up SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init() failed!\n");
		return false;
	}
	
	// Store dimensions in globals
	pixels_w = width;
	pixels_h = height;
	window_rect.x = window_rect.y = 0;
	window_rect.w = width * scale;
	window_rect.h = height * scale;

	// Window & Renderer
	if (SDL_CreateWindowAndRenderer(window_rect.w, window_rect.h, 0, &sdl_window, &sdl_renderer) != 0) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer() failed!\n");
		return false;
	}

	// Texture: Using ABGR pixel format is slightly faster (~10%) than using RGBA.
	sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!sdl_texture) {
		fprintf(stderr, "SDL_CreateTexture() failed!\n");
		return false;
	}

	// Allocate frame buffer
	pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
	if (!pixels) {
		fprintf(stderr, "malloc() failed!\n");
		return false;
	}
	set_fill_color(BLACK_COLOR);
	fill_screen();
	
	// Set up the renderer
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); // Use no interpolation
	// Clear the window
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_RenderPresent(sdl_renderer);

	// Debug logging: window * texture size
	fprintf(stdout, "Created window (%dx%d) and texture (%dx%d).\n", window_rect.w, window_rect.h, pixels_w, pixels_h);

	return true;
}

void destroy_window(void) {
	free(pixels);
	SDL_DestroyTexture(sdl_texture);
	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

void render_to_screen(void) {
	// Render frame buffer
	SDL_UpdateTexture(sdl_texture, NULL, pixels, pixels_w * sizeof(uint32_t));
	SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, &window_rect);
	SDL_RenderPresent(sdl_renderer);
}

#pragma mark - Drawing 2D

void fill_screen(void) {
	for (int i = 0; i < pixels_w * pixels_h; i++) {
		pixels[i] = fill_color;
	}
}

void set_line_color(uint32_t color) {
	line_color = color;
}

void set_fill_color(uint32_t color) {
	fill_color = color;
}

void move_to(vec2_t a) {
	cursor = a;
}

void line_to(vec2_t a) {
	float dx = a.x - cursor.x;
	float dy = a.y - cursor.y;
	float steps = fabs(dx) > fabs(dy)? fabs(dx) : fabs(dy);
	float sx = dx / steps;
	float sy = dy / steps;
	float x = cursor.x;
	float y = cursor.y;
	for (float i = 0.0f; i <= steps; i++) {
		set_pixel(floorf(x), floorf(y), line_color);
		x += sx;
		y += sy;
	}
	cursor = a;
}

void fill_rect(int x, int y, int w, int h) {
	for (int y1 = 0; y1 < h; y1++) {
		for (int x1 = 0; x1 < w; x1++) {
			set_pixel(x + x1, y + y1, fill_color);
		}
	}
}

void fill_centered_rect(int x, int y, int w, int h) {
	fill_rect(x - w / 2, y - h / 2, w, h);
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


#pragma mark - Projection 3D

void init_projection(void) {
	// Set default view transform to center on and scale to screen
	mat3_get_identity(view_transform_2d);
	mat3_translate(view_transform_2d, pixels_w / 2, pixels_h / 2);
	mat3_scale(view_transform_2d, pixels_h, pixels_h);
	
	// Set default camera transform to -5 units
	mat4_get_identity(camera_transform_3d);
	mat4_translate(camera_transform_3d, 0, 0, -5);
}

vec2_t orthographic_project_point(vec3_t pt3d) {
	// Drop z
	vec2_t pt2d = { .x = pt3d.x, .y = pt3d.y };

	// Apply view transform
	pt2d = vec2_mat3_multiply(pt2d, view_transform_2d);
	return pt2d;
}

vec2_t perspective_project_point(vec3_t pt3d) {
	// Apply 3d transforms
	pt3d = vec3_mat4_multiply(pt3d, camera_transform_3d);

	vec2_t pt2d = { .x = pt3d.x / pt3d.z, .y = pt3d.y / pt3d.z };

	// Apply view transform
	pt2d = vec2_mat3_multiply(pt2d, view_transform_2d);

	return pt2d;
}