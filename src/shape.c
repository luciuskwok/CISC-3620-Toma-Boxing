//
//  shape.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/2/24.
//

#include "shape.h"
#include "color.h"
#include "drawing.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>



// Global list of 3D meshes
#define SHAPES_MAX (256)
shape_t *shapes[SHAPES_MAX];
int shape_count = 0;


shape_t *shape_new(int point_count) {
	shape_t *shape = malloc(sizeof(shape_t));
	if (!shape) {
		fprintf(stderr, "Unable to allocate shape!\n");
		return NULL;
	}
	vec2_t *points = malloc(sizeof(vec2_t) * (size_t)point_count);
	if (!points) {
		fprintf(stderr, "Unable to allocate shape points!\n");
		free(shape);
		return NULL;
	}
	vec2_t *proj_pts = malloc(sizeof(vec2_t) * (size_t)point_count);
	if (!proj_pts) {
		fprintf(stderr, "Unable to allocate shape points!\n");
		free(shape);
		free(points);
		return NULL;
	}
	
	shape->point_count = point_count;
	shape->points = points;
	shape->projected_points = proj_pts;
	shape->is_closed = true;
	
	// Colors
	shape->line_color = COLOR_WHITE;
	shape->fill_color = COLOR_WHITE;
	
	// Physics
	shape->scale = vec2_identity();
	shape->position = vec2_zero(); // meters
	shape->linear_momentum = vec2_zero(); // meters/second
	shape->rotation = 0.0f; // radians
	shape->angular_momentum = 0.0f; // radians/second
	
	shape->lifetime = 0.0;
	return shape;
}

void shape_destroy(shape_t *shape) {
	free(shape->points);
	free(shape->projected_points);
	free(shape);
}

#pragma mark -

shape_t *create_rectangle_shape(float w, float h) {
	shape_t *s = shape_new(4);
	if (s) {
		s->is_closed = true;
		vec2_t *p = s->points;
		float x1 = -w / 2.0f;
		float x2 = w / 2.0f;
		float y1 = -h / 2.0f;
		float y2 = h / 2.0f;
		p[0].x = x1;	p[0].y = y1;
		p[1].x = x2;	p[1].y = y1;
		p[2].x = x2;	p[2].y = y2;
		p[3].x = x1;	p[3].y = y2;
	}
	return s;
}

shape_t *create_polygon_shape(int sides) {
	/* Creates a n-sided polygon starting from (1, 0) and going clockwise,
	   assuming a coordinate system with positive y is down.
	 */
	shape_t *s = shape_new(sides);
	if (!s) return NULL;
	s->is_closed = true;
	vec2_t *p = s->points;
	for (int i = 0; i < sides; i++) {
		float angle = (float)(M_PI * 2.0) * i / sides;
		p[i].x = cosf(angle);
		p[i].y = sinf(angle);
	}
	return s;
}

shape_t *create_star_shape(int points, float indent) {
	int n = points * 2;
	shape_t *s = shape_new(n);
	if (!s) return NULL;
	s->is_closed = true;
	vec2_t *p = s->points;
	for (int i = 0; i < n; i++) {
		float a = (float)(M_PI * 2.0) * i / n;
		float d = (i % 2 == 0)? 1.0f : indent;
		p[i].x = cosf(a) * d;
		p[i].y = sinf(a) * d;
	}
	return s;
}

#pragma mark -

void shape_update(shape_t *shape, double delta_time) {
	float dt = (float)delta_time;
	
	// Linear momentum
	vec2_t movement = vec2_mul(shape->linear_momentum, dt);
	shape->position = vec2_add(shape->position, movement);
	
	// Angular momentum
	shape->rotation += shape->angular_momentum * dt;
	
	shape->lifetime += delta_time;
}

void shape_draw(shape_t *shape) {
	set_line_color(shape->line_color);
	set_fill_color(shape->fill_color);
	
	if (shape->point_count < 2) return;
	
	// Calculate transform matrix
	mat3_t tr = mat3_get_identity();
	tr = mat3_translate(tr, shape->position);
	tr = mat3_rotate(tr, shape->rotation);
	tr = mat3_scale(tr, shape->scale);
	
	// Apply transforms
	vec2_t *pp = shape->projected_points;
	for (int i = 0; i < shape->point_count; i++) {
		pp[i] = apply_view_transform_2d(vec2_mat3_multiply(shape->points[i], tr));
	}

	// Fill
	if (shape->fill_color != 0) {
		if (shape->point_count >= 3) {
			fill_polygon(pp, shape->point_count);
		}
	}
	
	// Stroke
	if (shape->line_color != 0) {
		move_to(pp[0]);
		for (int i = 1; i < shape->point_count; i++) {
			line_to(pp[i]);
		}
		if (shape->is_closed) {
			line_to(pp[0]);
		}
	}
}

#pragma mark -

// Global list of 2d shapes
void add_shape(shape_t *shape) {
	if (shape_count < SHAPES_MAX) {
		shapes[shape_count] = shape;
		shape_count++;
	} else {
		fprintf(stderr, "Shapes array is full!\n");
	}
}

void remove_all_shapes(void) {
	shape_count = 0;
}

int get_shape_count(void) {
	return shape_count;
}

shape_t **get_shapes(void) {
	return shapes;
}

