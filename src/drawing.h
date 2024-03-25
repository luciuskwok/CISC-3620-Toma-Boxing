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
#include "vector.h"

// SDL Interface
bool init_window(int width, int height, int scale);
void destroy_window(void);
void render_to_screen(void);

// Drawing 2D
void fill_screen(void);

void set_line_color(uint32_t color);
void set_fill_color(uint32_t color);

void move_to(vec2_t a);
void line_to(vec2_t a);
void fill_rect(int x, int y, int w, int h);
void fill_centered_rect(int x, int y, int w, int h);

void set_pixel(int x, int y, uint32_t color);

// Projection 3D
void init_projection(void);
vec2_t orthographic_project_point(vec3_t pt3d);
vec2_t perspective_project_point(vec3_t pt3d);


#endif /* drawing_h */
