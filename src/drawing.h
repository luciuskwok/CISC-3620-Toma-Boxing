//
//  drawing.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/24/24.
//

#ifndef drawing_h
#define drawing_h

#include <stdio.h>
#include <stdbool.h>

#include "mesh.h"
#include "vector.h"

// Rectangle
typedef struct {
	float x, y;
	float w, h;
} rectangle_t;
rectangle_t inset_rect(rectangle_t r, int x, int y);

// SDL Interface
bool init_screen(int width, int height, int scale);
void destroy_screen(void);
void render_to_screen(void);

// Drawing 2D
void fill_screen(void);

void set_line_color(uint32_t color);
void set_fill_color(uint32_t color);

void move_to(vec2_t a);
void line_to(vec2_t a);
void stroke_rect(rectangle_t r);

void fill_rect(rectangle_t r);
void fill_centered_rect(int x, int y, int w, int h);
void fill_triangle(vec2_t a, vec2_t b, vec2_t c);
void fill_polygon(vec2_t *points, int n);

void set_pixel(int x, int y, uint32_t color);
vec2_t apply_view_transform_2d(vec2_t point);

vec2_t get_cursor(void);
uint32_t get_line_color(void);
uint32_t get_fill_color(void);
int get_screen_width(void);
int get_screen_height(void);

// Projection 3D
vec2_t orthographic_project_point(vec3_t pt3d);
vec2_t perspective_project_point(vec3_t pt3d);
vec3_t get_camera_position(void);

#endif /* drawing_h */
