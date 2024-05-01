//
//  shape_creation.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 5/1/24.
//

#include "shape_creation.h"

#include "color.h"
#include "matrix.h"
#include "vector.h"
#include "array_list.h"
#include "point_list.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define RADIANF ((float)(M_PI * 2.0))


point_list_t *create_circle_arc_points(vec2_t center, float radius, float start_angle, float end_angle, int n) {
	point_list_t *pl = point_list_new(n);
	if (pl) {
		float angle_range = end_angle - start_angle;
		for (int i = 1; i < n; i++) {
			float angle = (float)i / (float)n * angle_range + start_angle;
			vec2_t pt;
			pt.x = center.x + cosf(angle) * radius;
			pt.y = center.y + sinf(angle) * radius;
			point_list_add(pl, pt);
		}
	}
	return pl;
}

#pragma mark -

shape_t *create_rectangle_shape(float w, float h) {
	shape_t *s = shape_new(4);
	if (s) {
		s->is_closed = true;
		float x1 = -w / 2.0f;
		float x2 = w / 2.0f;
		float y1 = -h / 2.0f;
		float y2 = h / 2.0f;
		shape_add_point(s, vec2_make(x1, y1));
		shape_add_point(s, vec2_make(x2, y1));
		shape_add_point(s, vec2_make(x2, y2));
		shape_add_point(s, vec2_make(x1, y2));
	}
	return s;
}

shape_t *create_rounded_rect_shape(float w, float h, float radius) {
	shape_t *s = shape_new(32);
	if (!s) return NULL;
	s->is_closed = true;
	
	point_list_t *arc;
	vec2_t center;
	
	// Top left corner
	center.x = -w / 2 + radius;
	center.y = -h / 2 + radius;
	arc = create_circle_arc_points(center, radius, 0.25f * RADIANF, 0.5f * RADIANF, 8);
	shape_add_points(s, arc);
	point_list_destroy(arc);
	
	// Bottom left corner
	center.x = -w / 2 + radius;
	center.y = h / 2 - radius;
	arc = create_circle_arc_points(center, radius, 0.5f * RADIANF, 0.75f * RADIANF, 8);
	shape_add_points(s, arc);
	point_list_destroy(arc);
	
	// Bottom right corner
	center.x = w / 2 - radius;
	center.y = h / 2 - radius;
	arc = create_circle_arc_points(center, radius, 0.75f * RADIANF, 1.0f * RADIANF, 8);
	shape_add_points(s, arc);
	point_list_destroy(arc);
	
	// Top right corner
	center.x = w / 2 - radius;
	center.y = -h / 2 + radius;
	arc = create_circle_arc_points(center, radius, 0.0f * RADIANF, 0.25f * RADIANF, 8);
	shape_add_points(s, arc);
	point_list_destroy(arc);

	return s;
}

shape_t *create_polygon_shape(int sides, float radius) {
	/* Creates a n-sided polygon starting from (1, 0) and going clockwise,
	   assuming a coordinate system with positive y is down.
	 */
	shape_t *s = shape_new(sides);
	if (!s) return NULL;
	s->is_closed = true;
	for (int i = 0; i < sides; i++) {
		float angle = RADIANF * i / sides;
		shape_add_point(s, vec2_make(cosf(angle) * radius, sinf(angle) * radius));
	}
	return s;
}

shape_t *create_star_shape(int points, float radius, float indent) {
	int n = points * 2;
	shape_t *s = shape_new(n);
	if (!s) return NULL;
	s->is_closed = true;
	for (int i = 0; i < n; i++) {
		float a = RADIANF * i / n;
		float d = radius * (i % 2 == 0? 1.0f : indent);
		shape_add_point(s, vec2_make(cosf(a) * d, sinf(a) * d));
	}
	return s;
}

shape_t *create_heart_shape(void) {
	const int sides = 32;
	shape_t *s = shape_new(sides);
	if (!s) return NULL;
	s->is_closed = true;
	
	// Make bottom point
	shape_add_point(s, vec2_make(0.0f, -0.75f));
	
	// Make symmetrical arcs of circles
	vec2_t center = { .x = 0.325f, .y = 0.125f };
	float start_angle = RADIANF * -0.125f;
	float end_angle = RADIANF * 0.5f;
	point_list_t *arc = create_circle_arc_points(center, 0.333f, start_angle, end_angle, 15);
	
	// Add non-mirrored arc
	shape_add_points(s, arc);
	
	// Add mirrored arc
	mat3_t mirror = mat3_scale(mat3_identity(), vec2_make(-1.0f, 1.0f));
	for (int i = arc->length - 1; i >= 0; i--) {
		shape_add_point(s, vec2_mat3_multiply(arc->array[i], mirror));
	}
	
	// Dispose of arc
	point_list_destroy(arc);

	return s;
}

shape_t *create_crescent_moon_shape(void) {
	const int sides = 32;
	shape_t *s = shape_new(sides);
	if (!s) return NULL;
	s->is_closed = true;
	
	// Trace two circles through 3/4 of a circle
	const float inner_offset = 0.25f;
	const float inner_radius = 0.775f;
	int n = sides / 2;
	for (int i = 0; i < n; i++) {
		float angle = RADIANF * ((float)i / (float)(n - 1) * 0.75f + 0.125f);
		shape_add_point(s, vec2_make(cosf(angle), -sinf(angle)));
	}

	for (int i = 0; i < n; i++) {
		float angle = RADIANF * ((float)i / (float)(n - 1) * 0.75f + 0.125f);
		vec2_t p;
		p.x = cosf(angle) * inner_radius + inner_offset;
		p.y = sinf(angle) * inner_radius;
		shape_add_point(s, p);
	}

	return s;
}

#pragma mark -

shape_t *create_tomato_top_shape(void) {
	// Tomato body
	shape_t *body = create_polygon_shape(15, 1.0f);
	if (!body) return NULL;
	body->line_color = COLOR_ABGR_WHITE;
	body->fill_color = rgb_to_abgr(COLOR_RGB_PINK_3);
	
	// Tomato top or leaves
	const float top_scale = 7.0f/13.0f;
	shape_t *top = create_star_shape(5, 1.0f, 0.25f);
	if (!top) {
		shape_destroy(body);
		return NULL;
	}
	top->scale = vec2_make(top_scale, top_scale);
	top->line_color = 0;
	top->fill_color = rgb_to_abgr(COLOR_RGB_GREEN_1);
	
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
	body->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	body->fill_color = rgb_to_abgr(COLOR_RGB_RED_1);
	
	// Transform for leaves and stem
	mat3_t tr = mat3_identity();
	tr = mat3_scale(tr, vec2_make(0.01f, -0.01f));
	tr = mat3_translate(tr, vec2_make(-50, -60));
	
	// Leaves
	shape_t *leaves = shape_new(tomato_leaves_points_len);
	if (leaves) {
		leaves->line_color = rgb_to_abgr(COLOR_RGB_GREEN_3);
		leaves->fill_color = rgb_to_abgr(COLOR_RGB_GREEN_1);
		for (int i=0; i<tomato_leaves_points_len; i++) {
			shape_add_point(leaves, vec2_mat3_multiply(tomato_leaves_points[i], tr));
		}
		shape_add_child(body, leaves);
	}
	
	// Stem
	shape_t *stem = shape_new(tomato_stem_points_len);
	if (stem) {
		stem->line_color = rgb_to_abgr(COLOR_RGB_GREEN_3);
		stem->fill_color = rgb_to_abgr(COLOR_RGB_GREEN_1);
		stem->is_closed = false;
		for (int i=0; i<tomato_stem_points_len; i++) {
			shape_add_point(stem, vec2_mat3_multiply(tomato_stem_points[i], tr));
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
	shape_t *group = shape_new(0);
	if (!group) return NULL;
	
	shape_t *tomato = create_tomato_side_shape();
	if (!tomato) return NULL;
	tomato->position = vec2_make(0, 0.8f);
	shape_add_child(group, tomato);
	
	// Transform for points
	mat3_t tr = mat3_identity();
	tr = mat3_scale(tr, vec2_make(0.01f, -0.01f));
	tr = mat3_translate(tr, vec2_make(-50, -140));
	
	mat3_t mirror = mat3_scale(mat3_identity(), vec2_make(-1.0f, 1.0f));

	// Microphone shape
	shape_t *mic = shape_new(microphone_points_len * 2);
	if (mic) {
		mic->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
		mic->fill_color = rgb_to_abgr(COLOR_RGB_PINK_1);
		// Add non-mirrored points
		int n = microphone_points_len;
		for (int i=0; i<n; i++) {
			vec2_t pt = vec2_mat3_multiply(microphone_points[i], tr);
			shape_add_point(mic, pt);
		}
		// Add mirrored points
		vec2_t *p = mic->points->array;
		for (int i=0; i<n; i++) {
			shape_add_point(mic, vec2_mat3_multiply(p[n - i - 1], mirror));
		}
		shape_add_child(group, mic);
	}
	
	// Microphone upper line
	shape_t *upper_line = shape_new(microphone_upper_line_points_len);
	if (upper_line) {
		upper_line->line_color = rgb_to_abgr(COLOR_RGB_RED_3);
		upper_line->fill_color = 0;
		upper_line->is_closed = false;
		for (int i=0; i<microphone_upper_line_points_len; i++) {
			shape_add_point(upper_line, vec2_mat3_multiply(microphone_upper_line_points[i], tr));
		}
		shape_add_child(group, upper_line);
	}
	
	// Microphone lower band
	shape_t *lower_band = shape_new(microphone_lower_band_points_len * 2);
	if (lower_band) {
		lower_band->line_color = rgb_to_abgr(COLOR_RGB_GREEN_3);
		lower_band->fill_color = rgb_to_abgr(COLOR_RGB_GREEN_1);
		int n = microphone_lower_band_points_len;
		// Add non-mirrored points
		for (int i=0; i<n; i++) {
			vec2_t pt = vec2_mat3_multiply(microphone_lower_band_points[i], tr);
			shape_add_point(lower_band, pt);
		}
		// Add mirrored points
		vec2_t *p = lower_band->points->array;
		for (int i=0; i<n; i++) {
			shape_add_point(lower_band, vec2_mat3_multiply(p[n - i - 1], mirror));
		}
		shape_add_child(group, lower_band);
	}
	
	return group;
}

const int stand_pts_len = 5;
vec2_t stand_pts[stand_pts_len] = {
	{59, 0},
	{59, 100},
	{86, 104},
	{99, 106},
	{99, 120},
};

shape_t *create_stand_shape(void) {
	shape_t *s = shape_new(stand_pts_len * 2);
	if (!s) return NULL;
	
	// Transforms for points
	mat3_t tr = mat3_identity();
	tr = mat3_scale(tr, vec2_make(0.01f, -0.01f));
	tr = mat3_translate(tr, vec2_make(-50, 0));
	mat3_t mirror = mat3_scale(mat3_identity(), vec2_make(-1.0f, 1.0f));

	// Add non-mirrored points
	int n = stand_pts_len;
	for (int i=0; i<n; i++) {
		vec2_t pt = vec2_mat3_multiply(stand_pts[i], tr);
		shape_add_point(s, pt);
	}
	// Add mirrored points
	vec2_t *p = s->points->array;
	for (int i=0; i<n; i++) {
		shape_add_point(s, vec2_mat3_multiply(p[n - i - 1], mirror));
	}
	
	return s;
}

shape_t *create_microphone_with_stand_shape(void) {
	shape_t *group = shape_new(0);
	if (!group) return NULL;
	
	shape_t *stand = create_stand_shape();
	stand->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	stand->fill_color = rgb_to_abgr(COLOR_RGB_GRAY_60);
	shape_add_child(group, stand);

	shape_t *mic = create_microphone_shape();
	if (!mic) return NULL;
	mic->position = vec2_make(0, 0.2f);
	mic->rotation = RADIANF / 8.0f;
	shape_add_child(group, mic);

	return group;
}

#pragma mark -

shape_t *create_monitor_shape(void) {
	shape_t *group = shape_new(0);
	if (!group) return NULL;
	
	// Using pixel art as reference:
	// Center: 50, 45
	shape_t *stand = create_rectangle_shape(0.06f, 0.09f);
	if (!stand) return NULL;
	stand->position = vec2_make(0, -0.29f); // 50, 74
	stand->line_color = 0;
	stand->fill_color = rgb_to_abgr(COLOR_RGB_GRAY_40);
	shape_add_child(group, stand);
	
	shape_t *base = create_rectangle_shape(0.30f, 0.02f);
	if (!base) return NULL;
	base->position = vec2_make(0, -0.34f); // 0, 78
	base->line_color = 0;
	base->fill_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	shape_add_child(group, base);
	
	shape_t *bezel = create_rectangle_shape(0.82f, 0.52f);
	if (!bezel) return NULL;
	bezel->line_color = rgb_to_abgr(COLOR_RGB_WHITE_3);
	bezel->fill_color = rgb_to_abgr(COLOR_RGB_WHITE_2);
	shape_add_child(group, bezel);
	
	shape_t *screen = create_rectangle_shape(0.80f, 0.45f);
	if (!screen) return NULL;
	screen->position = vec2_make(0, 0.025f);
	screen->line_color = rgb_to_abgr(COLOR_RGB_GRAY_10);
	screen->fill_color = rgb_to_abgr(COLOR_RGB_GRAY_40);
	shape_add_child(group, screen);
		
	return group;
}

shape_t *create_cpu_shape(void) {
	shape_t *group = shape_new(0);
	if (!group) return NULL;
	
	// Using pixel art as reference:
	// Center: 50, 48
	shape_t *side = create_rectangle_shape(0.06f, 0.60f);
	if (!side) return NULL;
	side->position = vec2_make(0.15f, -0.02f);
	side->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	side->fill_color = rgb_to_abgr(COLOR_RGB_WHITE_3);
	shape_add_child(group, side);

	shape_t *base = create_rectangle_shape(0.26f, 0.64f);
	if (!base) return NULL;
	base->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	base->fill_color = rgb_to_abgr(COLOR_RGB_WHITE_2);
	shape_add_child(group, base);

	shape_t *cdrom = create_rectangle_shape(0.16f, 0.01f);
	if (!cdrom) return NULL;
	cdrom->position = vec2_make(0, 0.27f);
	cdrom->line_color = 0;
	cdrom->fill_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	shape_add_child(group, cdrom);

	shape_t *floppy = create_rectangle_shape(0.08f, 0.01f);
	if (!floppy) return NULL;
	floppy->position = vec2_make(0, 0.17f);
	floppy->line_color = 0;
	floppy->fill_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	shape_add_child(group, floppy);
	
	return group;
}

const int lightning_bolt_pts_len = 11;
vec2_t lightning_bolt_pts[lightning_bolt_pts_len] = {
	{48, 21},
	{45, 30},
	{49, 30},
	{45, 39},
	{49, 39},
	{46, 49},
	{53, 37},
	{50, 37},
	{54, 28},
	{52, 28},
	{55, 21}
};

shape_t *create_lighting_bolt_shape(void) {
	shape_t *s = shape_new(6);
	if (!s) return NULL;
	s->line_color = COLOR_ABGR_WHITE;
	s->fill_color = rgba_to_abgr(COLOR_RGB_YELLOW_1, 255);
	s->is_closed = false;
	
	// Transform for points
	mat3_t tr = mat3_identity();
	tr = mat3_scale(tr, vec2_make(0.01f, -0.01f));
	tr = mat3_translate(tr, vec2_make(-50, -34));

	// Add points
	for (int i=0; i<lightning_bolt_pts_len; i++) {
		vec2_t pt = vec2_mat3_multiply(lightning_bolt_pts[i], tr);
		shape_add_point(s, pt);
	}
	
	return s;
}

shape_t *create_smoke_circle_shape(void) {
	shape_t *s = shape_new(6);
	if (!s) return NULL;
	s->line_color = rgb_to_abgr(COLOR_RGB_GRAY_20);
	s->fill_color = rgba_to_abgr(COLOR_RGB_GRAY_50, 127);
	s->is_closed = false;

	// Arc
	point_list_t *arc = create_circle_arc_points(vec2_zero(), 0.2f, -0.125f * RADIANF, 0.625f * RADIANF, 24);
	shape_add_points(s, arc);
	point_list_destroy(arc);
	
	return s;

}

#pragma mark -

shape_t *create_toemaniac_shape(void) {
	// TODO: implement create_toemaniac_shape
	return NULL;
}

shape_t *create_envelope_shape(uint32_t line_color) {
	shape_t *s = create_rectangle_shape(1.0f, 0.6f);
	if (!s) return NULL;
	s->line_color = line_color;
	
	// Add line for flap
	shape_t *flap = shape_new(3);
	if (flap) {
		flap->is_closed = false;
		flap->fill_color = 0;
		flap->line_color = line_color;
		shape_add_point(flap, vec2_make(-0.5f, -0.3f));
		shape_add_point(flap, vec2_make(0.0f, 0.0f));
		shape_add_point(flap, vec2_make(0.5f, -0.3f));
	}
	shape_add_child(s, flap);
	
	// TODO: Add tomato sticker
	
	return s;
}
