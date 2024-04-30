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

#include <stdbool.h>


typedef struct {
	// Geometry
	int point_count;
	vec2_t *points;
	bool is_closed;
	array_list_t *children;
	
	// Visuals
	bool is_visible;
	uint32_t line_color;
	uint32_t fill_color;
	
	// Physics
	vec2_t scale;
	vec2_t position; // meters
	vec2_t linear_momentum; // meters/second
	float rotation; // radians
	float angular_momentum; // radians/second
	double lifetime;
} shape_t;


shape_t *shape_new(int point_count);
void shape_destroy(shape_t *shape);

shape_t *create_rectangle_shape(float w, float h);
shape_t *create_polygon_shape(int sides, float radius);
shape_t *create_star_shape(int points, float radius, float indent);

shape_t *create_tomato_top_shape(void);
shape_t *create_tomato_side_shape(void);
shape_t *create_microphone_shape(void);
shape_t *create_toemaniac_shape(void);

shape_t *create_heart_shape(void);
shape_t *create_crescent_moon_shape(void);
shape_t *create_envelope_shape(uint32_t line_color, uint32_t fill_color);

void shape_update(shape_t *shape, double delta_time);
void shape_draw(shape_t *shape, mat3_t transform);

#endif /* shape_h */
