//
//  shape.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/2/24.
//

#ifndef shape_h
#define shape_h

#include "matrix.h"
#include "vector.h"
#include "array_list.h"
#include "point_list.h"

#include <stdbool.h>


typedef struct {
	// Geometry
	point_list_t *points;
	bool is_closed;
	array_list_t *children;
	
	// Visuals
	bool is_visible;
	uint32_t line_color;
	uint32_t fill_color;
	float opacity;
	
	// Physics
	vec2_t scale;
	vec2_t position; // meters
	vec2_t linear_momentum; // meters/second
	float rotation; // radians
	float angular_momentum; // radians/second
	double lifetime;
} shape_t;


shape_t *shape_new(int initial_capacity);
void shape_destroy(shape_t *shape);

bool shape_add_point(shape_t *shape, vec2_t point);
bool shape_add_points(shape_t *shape, point_list_t *points);
bool shape_add_child(shape_t *shape, shape_t *child);

shape_t *create_rectangle_shape(float w, float h);
shape_t *create_rounded_rect_shape(float w, float h, float radius);
shape_t *create_polygon_shape(int sides, float radius);
shape_t *create_star_shape(int points, float radius, float indent);
shape_t *create_heart_shape(void);
shape_t *create_crescent_moon_shape(void);

shape_t *create_tomato_top_shape(void);
shape_t *create_tomato_side_shape(void);
shape_t *create_microphone_shape(void);
shape_t *create_stand_shape(void);
shape_t *create_microphone_with_stand_shape(void);

shape_t *create_toemaniac_shape(void);

shape_t *create_envelope_shape(uint32_t line_color);

void shape_update(shape_t *shape, double delta_time);
void shape_draw(shape_t *shape);
void shape_draw_recursive(shape_t *shape, mat3_t transform, float opacity);

#endif /* shape_h */
