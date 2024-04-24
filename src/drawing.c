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
	view_transform_2d = mat3_get_identity();
	view_transform_2d = mat3_translate(view_transform_2d, vec2_make(screen_w / 2, screen_h / 2));
	float scale2d = screen_h;
	view_transform_2d = mat3_scale(view_transform_2d, vec2_make(scale2d, scale2d));
	
	// Set default camera transform to -5 units
	camera_transform_3d = mat4_get_identity();
	camera_transform_3d = mat4_translate(camera_transform_3d, vec3_make(0, 0, -5));
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
	set_fill_color(COLOR_ABGR_BLACK);
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

void stroke_rect(rectangle_t r) {
	vec2_t a = { r.x, r.y };
	vec2_t b = { r.x + r.w - 1, r.y };
	vec2_t c = { r.x + r.w - 1, r.y + r.h - 1 };
	vec2_t d = { r.x, r.y + r.h - 1 };
	
	move_to(a);
	line_to(b);
	line_to(c);
	line_to(d);
	line_to(a);
}

void fill_rect(rectangle_t r) {
	int x0 = (int)round(r.x);
	int x1 = (int)round(r.x + r.w);
	int y0 = (int)round(r.y);
	int y1 = (int)round(r.y + r.h);
	
	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			set_pixel(x, y, fill_color);
		}
	}
}

void fill_centered_rect(int x, int y, int w, int h) {
	rectangle_t r = { x - w / 2, y - h / 2, w, h };
	fill_rect(r);
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

float sign_3vec2(vec2_t a, vec2_t b, vec2_t c) {
	return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}

bool point_in_triangle(vec2_t p, vec2_t a, vec2_t b, vec2_t c) {
	float d = sign_3vec2(p, a, b);
	float e = sign_3vec2(p, b, c);
	float f = sign_3vec2(p, c, a);
	bool has_neg = (d < 0) || (e < 0) || (f < 0);
	bool has_pos = (d > 0) || (e > 0) || (f > 0);
	return !(has_neg && has_pos);
}

void fill_triangle(vec2_t a, vec2_t b, vec2_t c) {
	// Fill triangle by scanline by testing if each point is within the half-planes formed by triangle.
	// https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle

	float x0 = (a.x < b.x)? a.x : b.x;
	x0 = (x0 < c.x)? x0 : c.x;
	float x1 = (a.x > b.x)? a.x: b.x;
	x1 = (x1 > c.x)? x1 : c.x;

	float y0 = (a.y < b.y)? a.y : b.y;
	y0 = (y0 < c.y)? y0 : c.y;
	float y1 = (a.y > b.y)? a.y: b.y;
	y1 = (y1 > c.y)? y1 : c.y;

	for (float y = floorf(y0); y <= ceilf(y1); y++) {
		for (float x = floorf(x0); x <= ceilf(x1); x++) {
			vec2_t p = { x + 0.5f, y + 0.5f };
			if (point_in_triangle(p, a, b, c)) {
				set_pixel((int)x, (int)y, fill_color);
			}
		}
	}
}

bool point_in_convex_hull(vec2_t a, vec2_t *p, int n) {
	bool has_neg = false;
	bool has_pos = false;
	for (int i = 0; i < n; i++) {
		float sign = sign_3vec2(a, p[i], p[(i+1)%n]);
		if (sign < 0) {
			has_neg = true;
		} else {
			has_pos = true;
		}
	}
	return !(has_neg && has_pos);
}

rectangle_t bounding_rect(vec2_t *points, int n) {
	rectangle_t r = { 0, 0, 0, 0 };
	if (n < 1) return r;
	vec2_t a = points[0];
	vec2_t b = points[0];
	for (int i = 1; i < n; i++) {
		vec2_t c = points[i];
		if (a.x > c.x) a.x = c.x;
		if (b.x < c.x) b.x = c.x;
		if (a.y > c.y) a.y = c.y;
		if (b.y < c.y) b.y = c.y;
	}
	r.x = a.x;
	r.y = a.y;
	r.w = b.x - a.x;
	r.h = b.y - a.y;
	
	return r;
}

bool is_edge_crossing(float left, float right, float y, vec2_t a, vec2_t b) {
	if (a.y < b.y) {
		if (y < a.y || y >= b.y) return false;
	} else {
		if (y < b.y || y >= a.y) return false;
	}
	
	float slope = (b.y - a.y) / (b.x - a.x);
	float dy = y - a.y;
	float dx = dy / slope;
	float x = a.x + dx;
	return (left <= x) && (x < right);
}

int edge_crossings(float left, float right, float y, vec2_t *p, int n) {
	int sum = 0;
	for (int i = 0; i < n; i++) {
		vec2_t a = p[i];
		vec2_t b = p[(i+1)%n];
		if (is_edge_crossing(left, right, y, a, b)) sum++;
	}
	return sum;
}

void fill_polygon(vec2_t *points, int n) {
	// Fill polygon by scanline by counting the number of edge crossings
	// https://stackoverflow.com/questions/65573101/draw-a-filled-polygon-using-scanline-loop
	if (n < 3) return;
	if (n == 3) {
		fill_triangle(points[0], points[1], points[2]);
	} else {
		rectangle_t b = bounding_rect(points, n);
		for (float y = floorf(b.y); y <= ceilf(b.y + b.h); y++) {
			int crossings = 0;
			for (float x = floorf(b.x); x <= ceilf(b.x + b.w); x++) {
				float left = x - 0.5f;
				float right = x + 0.5f;
				crossings += edge_crossings(left, right, y + 0.5f, points, n);
				if (crossings % 2 == 1) {
					set_pixel((int)x, (int)y, fill_color);
				}
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
