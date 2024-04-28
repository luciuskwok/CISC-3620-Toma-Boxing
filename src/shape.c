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
#include <math.h>

#define RADIANF ((float)(M_PI * 2.0))


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
	shape->children = NULL;
	
	// Visuals
	shape->is_visible = true;
	shape->line_color = COLOR_ABGR_WHITE;
	shape->fill_color = COLOR_ABGR_WHITE;
	
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
	
	free(shape->points);
	free(shape->projected_points);
	free(shape);
}

bool shape_add_child(shape_t *shape, shape_t *child) {
	if (!shape->children) {
		shape->children = array_list_new(4);
		if (!shape->children) return false;
	}
	return array_list_add(shape->children, child);
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

shape_t *create_polygon_shape(int sides, float radius) {
	/* Creates a n-sided polygon starting from (1, 0) and going clockwise,
	   assuming a coordinate system with positive y is down.
	 */
	shape_t *s = shape_new(sides);
	if (!s) return NULL;
	s->is_closed = true;
	vec2_t *p = s->points;
	for (int i = 0; i < sides; i++) {
		float angle = (float)(M_PI * 2.0) * i / sides;
		p[i].x = cosf(angle) * radius;
		p[i].y = sinf(angle) * radius;
	}
	return s;
}

shape_t *create_star_shape(int points, float radius, float indent) {
	int n = points * 2;
	shape_t *s = shape_new(n);
	if (!s) return NULL;
	s->is_closed = true;
	vec2_t *p = s->points;
	for (int i = 0; i < n; i++) {
		float a = (float)(M_PI * 2.0) * i / n;
		float d = radius * (i % 2 == 0? 1.0f : indent);
		p[i].x = cosf(a) * d;
		p[i].y = sinf(a) * d;
	}
	return s;
}

shape_t *create_tomato_top_shape(void) {
	// Tomato body
	shape_t *body = create_polygon_shape(15, 1.0f);
	if (!body) return NULL;
	body->line_color = COLOR_ABGR_WHITE;
	body->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_TOP_PINK);
	
	// Tomato top or leaves
	const float top_scale = 7.0f/13.0f;
	shape_t *top = create_star_shape(5, 1.0f, 0.25f);
	if (!top) {
		shape_destroy(body);
		return NULL;
	}
	top->scale = vec2_make(top_scale, top_scale);
	top->line_color = 0;
	top->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_TOP_GREEN);
	
	// Add top to body
	shape_add_child(body, top);
	
	return body;
}

const int tomato_leaves_points_len = 19;
vec2_t tomato_leaves_points[tomato_leaves_points_len] = {
	{50, 16},
	{62, 16},
	{67, 19},
	{64, 21},
	{53, 23},
	{59, 25},
	{64, 29},
	{63, 31},
	{58, 31},
	{50, 26},
	
	{46 ,29},
	{42, 32},
	{39, 30},
	{41, 26},
	{45, 22},
	{32, 24},
	{29, 21},
	{38, 17},
	{46, 16}
};

const int tomato_stem_points_len = 4;
vec2_t tomato_stem_points[tomato_stem_points_len] = {
	{47, 18},
	{41, 6},
	{49, 5},
	{51, 18}
};

shape_t *create_tomato_side_shape(void) {
	// Tomato body
	shape_t *body = create_polygon_shape(32, 0.4f);
	if (!body) return NULL;
	body->line_color = rgb_to_abgr(COLOR_RGB_TOMATO_SIDE_OUTLINE);
	body->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_SIDE_FILL);
	
	// Transform for leaves and stem
	mat3_t tr = mat3_identity();
	tr = mat3_scale(tr, vec2_make(0.01f, 0.01f));
	tr = mat3_translate(tr, vec2_make(-50, -60));
	
	// Leaves
	shape_t *leaves = shape_new(tomato_leaves_points_len);
	if (leaves) {
		leaves->line_color = rgb_to_abgr(COLOR_RGB_TOMATO_LEAF_OUTLINE);
		leaves->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_LEAF_FILL);
		for (int i=0; i<tomato_leaves_points_len; i++) {
			leaves->points[i] = vec2_mat3_multiply(tomato_leaves_points[i], tr);
		}
		shape_add_child(body, leaves);
	}
	
	// Stem
	shape_t *stem = shape_new(tomato_stem_points_len);
	if (stem) {
		stem->line_color = rgb_to_abgr(COLOR_RGB_TOMATO_LEAF_OUTLINE);
		stem->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_LEAF_FILL);
		stem->is_closed = false;
		for (int i=0; i<tomato_stem_points_len; i++) {
			stem->points[i] = vec2_mat3_multiply(tomato_stem_points[i], tr);
		}
		shape_add_child(body, stem);
	}

	return body;
}

const int microphone_points_len = 7;
vec2_t microphone_points[microphone_points_len] = {
	{50, 97},
	{61, 84},
	{83, 80},
	{82, 93},
	{78, 104},
	{70, 279},
	{55, 280}
};

const int microphone_upper_line_points_len = 5;
vec2_t microphone_upper_line_points[microphone_upper_line_points_len] = {
	{22, 104},
	{40, 104},
	{50, 101},
	{60, 104},
	{78, 104}
};

const int microphone_lower_band_points_len = 6;
vec2_t microphone_lower_band_points[microphone_lower_band_points_len] = {
	{50, 250},
	{62, 256},
	{71, 251},
	{71, 241},
	{63, 245},
	{50, 240}
};

shape_t *create_microphone_shape(void) {
	shape_t *base = create_tomato_side_shape();
	if (!base) return NULL;
	
	// Transform for points
	mat3_t tr = mat3_identity();
	tr = mat3_scale(tr, vec2_make(0.01f, 0.01f));
	tr = mat3_translate(tr, vec2_make(-50, -60));
	
	mat3_t mirror = mat3_identity();
	mirror = mat3_scale(mirror, vec2_make(-1.0f, 1.0f));

	// Microphone shape
	shape_t *mic = shape_new(microphone_points_len * 2);
	if (mic) {
		mic->line_color = rgb_to_abgr(COLOR_RGB_TOMATO_SIDE_OUTLINE);
		mic->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_TOP_PINK);
		int n = microphone_points_len * 2;
		for (int i=0; i<microphone_points_len; i++) {
			vec2_t pt = vec2_mat3_multiply(microphone_points[i], tr);
			mic->points[i] = pt;
			mic->points[n - i - 1] = vec2_mat3_multiply(pt, mirror);
		}
		shape_add_child(base, mic);
	}
	
	// Microphone upper line
	shape_t *upper_line = shape_new(microphone_upper_line_points_len);
	if (upper_line) {
		upper_line->line_color = rgb_to_abgr(COLOR_RGB_TOMATO_SIDE_FILL);
		upper_line->fill_color = 0;
		upper_line->is_closed = false;
		for (int i=0; i<microphone_upper_line_points_len; i++) {
			upper_line->points[i] = vec2_mat3_multiply(microphone_upper_line_points[i], tr);
		}
		shape_add_child(base, upper_line);
	}
	
	// Microphone lower band
	shape_t *lower_band = shape_new(microphone_lower_band_points_len * 2);
	if (lower_band) {
		lower_band->line_color = rgb_to_abgr(COLOR_RGB_TOMATO_LEAF_OUTLINE);
		lower_band->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_LEAF_FILL);
		int n = microphone_lower_band_points_len * 2;
		for (int i=0; i<microphone_lower_band_points_len; i++) {
			vec2_t pt = vec2_mat3_multiply(microphone_lower_band_points[i], tr);
			lower_band->points[i] = pt;
			lower_band->points[n - i - 1] = vec2_mat3_multiply(pt, mirror);
		}
		shape_add_child(base, lower_band);
	}
	
	return base;
}

shape_t *create_toemaniac_shape(void) {
	// TODO: implement create_microphone_shape
	return NULL;
}

shape_t *create_heart_shape(void) {
	const int sides = 32;
	shape_t *s = shape_new(sides);
	if (!s) return NULL;
	s->is_closed = true;
	vec2_t *p = s->points;
	
	// Make pinch points
	p[0] = vec2_make(0.0f, 1.0f);
	p[sides/2] = vec2_make(0.0f, 0.0f);
	
	// Make symmetrical spiral points
	const float x0 = 0.325f;
	const float y0 = 0.125f;
	const float len = 0.333f;
	int n = sides / 2;
	for (int i = 1; i < n; i++) {
		float angle = RADIANF * ((float)i / (float)n * 0.625f - 0.125f);
		float x1 = x0 + cosf(angle) * len;
		float y1 = y0 - sinf(angle) * len;
		
		p[i] = vec2_make(x1, y1);
		p[sides - i] = vec2_make(-x1, y1);
	}
	
	return s;
}

shape_t *create_envelope_shape(uint32_t line_color, uint32_t fill_color) {
	shape_t *s = create_rectangle_shape(1.0f, 0.6f);
	if (!s) return NULL;
	s->line_color = line_color;
	s->fill_color = fill_color;
	
	// Add line for flap
	shape_t *flap = shape_new(3);
	flap->is_closed = false;
	flap->line_color = line_color;
	flap->fill_color = 0;
	vec2_t *p = flap->points;
	p[0] = vec2_make(-0.5f, -0.3f);
	p[1] = vec2_make(0.0f, 0.0f);
	p[2] = vec2_make(0.5f, -0.3f);
	shape_add_child(s, flap);
	
	// Add tomato sticker
	
	return s;
}

shape_t *create_crescent_moon_shape(void) {
	const int sides = 32;
	shape_t *s = shape_new(sides);
	if (!s) return NULL;
	s->is_closed = true;
	vec2_t *p = s->points;
	
	// Trace two circles through 3/4 of a circle
	const float inner_offset = 0.25f;
	const float inner_radius = 0.775f;
	int n = sides / 2;
	for (int i = 0; i < n; i++) {
		float angle = RADIANF * ((float)i / (float)(n - 1) * 0.75f + 0.125f);
		
		p[i].x = cosf(angle);
		p[i].y = -sinf(angle);
		
		int j = sides - i - 1;
		p[j].x = cosf(angle) * inner_radius + inner_offset;
		p[j].y = -sinf(angle) * inner_radius;
	}
	
	return s;
	return NULL;
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

void shape_draw(shape_t *shape, mat3_t transform) {
	if (!shape->is_visible) return;
	
	// Calculate transform matrix
	transform = mat3_translate(transform, shape->position);
	transform = mat3_rotate(transform, shape->rotation);
	transform = mat3_scale(transform, shape->scale);

	if (shape->point_count >= 2) {
		set_line_color_abgr(shape->line_color);
		set_fill_color_abgr(shape->fill_color);
		
		// Apply transforms
		vec2_t *pp = shape->projected_points;
		for (int i = 0; i < shape->point_count; i++) {
			pp[i] = apply_view_transform_2d(vec2_mat3_multiply(shape->points[i], transform));
		}
		
		// Fill
		if (shape->fill_color != 0 && shape->point_count >= 3) {
			fill_polygon(pp, shape->point_count);
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
	
	// Children
	if (shape->children) {
		shape_t **a = (shape_t **)shape->children->array;
		int n = shape->children->length;
		for (int i=0; i<n; i++) {
			shape_draw(a[i], transform);
		}
	}
}

