//
//  drawing.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/24/24.
//

#include "drawing.h"
#include "color.h"
#include "matrix.h"
#include "vector.h"

#include <SDL2/SDL.h>


// Globals for SDL
SDL_Window* sdl_window;
SDL_Rect window_rect;
SDL_Renderer* sdl_renderer;
SDL_Texture* sdl_texture;
uint32_t* screen_pixels;
int screen_w;
int screen_h;
size_t screen_pitch;

// Drawing context
uint32_t line_color;
uint32_t fill_color;
vec2_t cursor;

// Transforms
mat3_t view_transform_2d;
mat4_t camera_transform_3d;



#pragma mark - SDL Interface

void init_projection(void) {
	// Set default view transform to center on and scale to screen
	mat3_get_identity(view_transform_2d);
	mat3_translate(view_transform_2d, screen_w / 2, screen_h / 2);
	float scale2d = screen_h;
	mat3_scale(view_transform_2d, scale2d, scale2d);
	
	// Set default camera transform to -5 units
	mat4_get_identity(camera_transform_3d);
	mat4_translate(camera_transform_3d, 0, 0, -5);
}

bool init_screen(int width, int height, int scale) {
	//fprintf(stdout, "initialize_windowing_system().\n");
	
	// Set up SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init(SDL_INIT_VIDEO) failed: %s\n", SDL_GetError());
		return false;
	}
	
	// Store dimensions in globals
	screen_w = width;
	screen_h = height;
	screen_pitch = (size_t)width * sizeof(uint32_t);
	window_rect.x = window_rect.y = 0;
	window_rect.w = width * scale;
	window_rect.h = height * scale;

	// Window & Renderer
	if (SDL_CreateWindowAndRenderer(window_rect.w, window_rect.h, 0, &sdl_window, &sdl_renderer) != 0) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
		return false;
	}

	// Texture: Using ABGR pixel format is slightly faster (~10%) than using RGBA.
	sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!sdl_texture) {
		fprintf(stderr, "SDL_CreateTexture() failed: %s\n", SDL_GetError());
		return false;
	}

	// Allocate frame buffer
	screen_pixels = (uint32_t*)malloc((size_t)(height) * screen_pitch);
	if (!screen_pixels) {
		fprintf(stderr, "malloc() failed!\n");
		return false;
	}
	set_fill_color(COLOR_BLACK);
	fill_screen();
	
	// Set up the renderer
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); // Use no interpolation
	// Clear the window
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_RenderPresent(sdl_renderer);

	// Debug logging: window * texture size
	fprintf(stdout, "Created window (%dx%d) and texture (%dx%d).\n", window_rect.w, window_rect.h, screen_w, screen_h);
	
	// Set up default transforms
	init_projection();

	return true;
}

void destroy_screen(void) {
	free(screen_pixels);
	SDL_DestroyTexture(sdl_texture);
	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

void render_to_screen(void) {
	// Render frame buffer
	SDL_UpdateTexture(sdl_texture, NULL, screen_pixels, (int)screen_pitch);
	SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, &window_rect);
	SDL_RenderPresent(sdl_renderer);
}

#pragma mark - Drawing 2D

void fill_screen(void) {
	for (int i = 0; i < screen_w * screen_h; i++) {
		screen_pixels[i] = fill_color;
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
	float steps = fabsf(dx) > fabsf(dy)? fabsf(dx) : fabsf(dy);
	float sx = dx / steps;
	float sy = dy / steps;
	float x = cursor.x;
	float y = cursor.y;
	for (float i = 0.0f; i <= steps; i++) {
		set_pixel((int)floorf(x), (int)floorf(y), line_color);
		x += sx;
		y += sy;
	}
	cursor = a;
}

void stroke_rect(int x, int y, int w, int h) {
	vec2_t a = { x, y };
	vec2_t b = { x + w - 1, y };
	vec2_t c = { x + w - 1, y + h - 1 };
	vec2_t d = { x, y + h - 1 };
	
	move_to(a);
	line_to(b);
	line_to(c);
	line_to(d);
	line_to(a);
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
	if (x < 0 || x >= screen_w) return;
	if (y < 0 || y >= screen_h) return;
	
	// Apply blending if color's alpha < 255
	int i = x + y * screen_w;
	if ((color & 0xFF000000) != 0xFF000000) {
		color = blend_color(screen_pixels[i], color);
	}
	screen_pixels[i] = color;
}

void swap_vec2(vec2_t *x, vec2_t *y) {
	vec2_t tmp = *x;
	*y = *x;
	*x = tmp;
}

void fill_triangle(vec2_t a, vec2_t b, vec2_t c) {
/*	// Sort vertices by x-position
	if (a.y > b.y) {
		swap_vec2(&a, &b);
	}
	if (b.y > c.y) {
		swap_vec2(&b, &c);
	}
	if (a.y > b.y) {
		swap_vec2(&a, &b);
	}
*/
	float x0 = (a.x < b.x)? a.x : b.x;
	x0 = (x0 < c.x)? x0 : c.x;
	float x1 = (a.x > b.x)? a.x: b.x;
	x1 = (x1 > c.x)? x0 : c.x;

	float y0 = (a.y < b.y)? a.y : b.y;
	y0 = (y0 < c.y)? y0 : c.y;
	float y1 = (a.y > b.y)? a.y: b.y;
	y1 = (y1 > c.y)? y1 : c.y;

	vec2_t v1 = { b.x - a.x, b.y - a.y };
	vec2_t v2 = { c.x - a.x, c.y - a.y };
	
	int x0i = (int)floorf(x0);
	int x1i = (int)ceilf(x1);
	int y0i = (int)floorf(y0);
	int y1i = (int)ceilf(y1);
	
	float cross12 = vec2_cross(v1, v2);

	for (int y = y0i; y <= y1i; y++) {
		for (int x = x0i; x <= x1i; x++) {
			vec2_t q = { x - a.x, y - a.y };
			float s = vec2_cross(q, v2) / cross12;
			float t = vec2_cross(v1, q) / cross12;
			if ((s >= 0.0f) && (t >= 0.0f) && (s + t <= 1.0f)) {
				set_pixel(x, y, fill_color);
			}
		}
	}
}

vec2_t apply_view_transform_2d(vec2_t point) {
	return vec2_mat3_multiply(point, view_transform_2d);
}

#pragma mark - Getters

vec2_t get_cursor(void) { return cursor; }
uint32_t get_line_color(void) { return line_color; }
uint32_t get_fill_color(void) { return fill_color; }
int get_screen_width(void) { return screen_w; }
int get_screen_height(void) { return screen_h; }

#pragma mark - Projection 3D

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

vec3_t get_camera_position(void) {
	vec3_t a = { 0, 0, 0 };
	vec3_t b = vec3_mat4_multiply(a, camera_transform_3d);
	return vec3_sub(a, b);
}
