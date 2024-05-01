//
//  shape.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/2/24.
//

#include "shape.h"
#include "color.h"
#include "drawing.h"
#include "array_list.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>



const int projected_points_capacity = 256;
vec2_t projected_points[projected_points_capacity];


shape_t *shape_new(int initial_capacity) {
	shape_t *shape = malloc(sizeof(shape_t));
	if (!shape) {
		fprintf(stderr, "Unable to allocate shape!\n");
		return NULL;
	}
	if (initial_capacity > 0) {
		shape->points = point_list_new(initial_capacity);
		if (!shape->points) {
			fprintf(stderr, "Unable to allocate shape points!\n");
			free(shape);
			return NULL;
		}
	} else {
		shape->points = NULL;
	}
	
	shape->is_closed = true;
	shape->children = NULL;
	
	// Visuals
	shape->is_visible = true;
	shape->line_color = COLOR_ABGR_WHITE;
	shape->fill_color = COLOR_ABGR_WHITE;
	shape->opacity = 1.0f;
	
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
	if (shape->children) {
		shape_t **a = (shape_t **)shape->children->array;
		int n = shape->children->length;
		for (int i=0; i<n; i++) {
			shape_destroy(a[i]);
		}
		array_list_destroy(shape->children);
	}
	
	if (shape->points) {
		point_list_destroy(shape->points);
	}
	free(shape);
}

bool shape_add_point(shape_t *shape, vec2_t point) {
	if (!shape->points) {
		shape->points = point_list_new(4);
		if (!shape->points) {
			fprintf(stderr, "Unable to allocate shape points!\n");
			return false;
		}
	}
	return point_list_add(shape->points, point);
}

bool shape_add_points(shape_t *shape, point_list_t *points) {
	for (int i = 0; i < points->length; i++) {
		if (!shape_add_point(shape, points->array[i])) return false;
	}
	return true;
}

bool shape_add_child(shape_t *shape, shape_t *child) {
	if (!shape->children) {
		shape->children = array_list_new(4);
		if (!shape->children) return false;
	}
	return array_list_add(shape->children, child);
}

#pragma mark -

void shape_update(shape_t *shape, double delta_time) {
	if (shape->children) {
		shape_t **a = (shape_t **)shape->children->array;
		int n = shape->children->length;
		for (int i=0; i<n; i++) {
			shape_update(a[i], delta_time);
		}
	}

	float dt = (float)delta_time;
	
	// Linear momentum
	vec2_t movement = vec2_mul(shape->linear_momentum, dt);
	shape->position = vec2_add(shape->position, movement);
	
	// Angular momentum
	shape->rotation += shape->angular_momentum * dt;
	
	shape->lifetime += delta_time;
}

void shape_draw(shape_t *shape) {
	shape_draw_recursive(shape, mat3_identity(), 1.0f);
}

void shape_draw_recursive(shape_t *shape, mat3_t transform, float opacity) {
	if (!shape->is_visible) return;
	
	// Calculate transform matrix
	transform = mat3_translate(transform, shape->position);
	transform = mat3_rotate(transform, shape->rotation);
	transform = mat3_scale(transform, shape->scale);
	
	// Calculate opacity
	opacity = opacity * shape->opacity;

	if (shape->points) {
		if (shape->points->length >= 2) {
			set_line_color_abgr(color_mul_opacity(shape->line_color, opacity));
			set_fill_color_abgr(color_mul_opacity(shape->fill_color, opacity));
			
			// Apply transforms
			int n = shape->points->length < projected_points_capacity? shape->points->length : projected_points_capacity;
			vec2_t *pp = projected_points;
			for (int i = 0; i < n; i++) {
				vec2_t pt = vec2_mat3_multiply(shape->points->array[i], transform);
				pp[i] = vec2_mat3_multiply(pt, view_transform_2d);
			}
			
			// Fill
			if (shape->fill_color != 0 && n >= 3) {
				fill_polygon(pp, n);
			}
			
			// Stroke
			if (shape->line_color != 0) {
				move_to(pp[0]);
				for (int i = 1; i < n; i++) {
					line_to(pp[i]);
				}
				if (shape->is_closed) {
					line_to(pp[0]);
				}
			}
		}
	}
	
	// Children
	if (shape->children) {
		shape_t **a = (shape_t **)shape->children->array;
		int n = shape->children->length;
		for (int i=0; i<n; i++) {
			shape_draw_recursive(a[i], transform, opacity);
		}
	}
}

