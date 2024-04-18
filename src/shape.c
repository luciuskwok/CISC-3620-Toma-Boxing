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
	
	shape->point_count = point_count;
	shape->points = points;
	shape->is_closed = true;
	shape->line_color = COLOR_WHITE;
	shape->fill_color = COLOR_WHITE;
	shape_reset_transform(shape);
	shape_reset_momentum(shape);
	shape->lifetime = 0.0;
	return shape;
}

void shape_destroy(shape_t *shape) {
	free(shape->points);
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
	shape_t *s = shape_new(sides);
	if (s) {
		s->is_closed = true;
		vec2_t *p = s->points;
		for (int i = 0; i < sides; i++) {
			float angle = (float)(M_PI * 2.0) * i /  sides;
			p[i].x = cosf(angle);
			p[i].y = sinf(angle);
		}
	}
	return s;

}

#pragma mark -

void shape_update(shape_t *shape, double delta_time) {
	momentum2d_t *m = &shape->momentum;
	
	float increment = (float)((M_PI / 180.0) * delta_time); // 1 deg/sec
	mat3_rotate(shape->transform, m->rotation * increment);
	
	float t = (float)delta_time;
	mat3_translate(shape->transform, m->x * t, m->y * t);
	
	shape->lifetime += delta_time;
}

void shape_draw(shape_t *shape) {
	set_line_color(shape->line_color);
	set_fill_color(shape->fill_color);
	
	if (shape->point_count < 2) return;
	
	// Fill
	if (shape->point_count >= 3) {
		vec2_t d = apply_view_transform_2d(vec2_mat3_multiply(shape->points[0], shape->transform));
		vec2_t e = apply_view_transform_2d(vec2_mat3_multiply(shape->points[1], shape->transform));
		vec2_t f = apply_view_transform_2d(vec2_mat3_multiply(shape->points[2], shape->transform));
		fill_triangle(d, e, f);
	}
	
	// Stroke
	vec2_t a = vec2_mat3_multiply(shape->points[0], shape->transform);
	a = apply_view_transform_2d(a);
	move_to(a);

	for (int i = 1; i < shape->point_count; i++) {
		vec2_t b = vec2_mat3_multiply(shape->points[i], shape->transform);
		b = apply_view_transform_2d(b);
		line_to(b);
	}
	if (shape->is_closed) {
		line_to(a);
	}
}

void shape_reset_transform(shape_t *shape) {
	mat3_get_identity(shape->transform);
}

void shape_reset_momentum(shape_t *shape) {
	momentum2d_t *m = &shape->momentum;
	m->x = m->y = 0.0f;
	m->rotation = 0;
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

