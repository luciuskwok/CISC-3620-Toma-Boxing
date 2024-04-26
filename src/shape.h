//
//  shape.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/2/24.
//

#ifndef shape_h
#define shape_h

#include <stdbool.h>

#include "matrix.h"
#include "vector.h"


typedef struct {
	int point_count;
	vec2_t *points;
	vec2_t *projected_points;
	bool is_closed;
	bool is_visible;
	
	// Colors
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
shape_t *create_polygon_shape(int sides);
shape_t *create_star_shape(int points, float indent);

void shape_update(shape_t *shape, double delta_time);
void shape_draw(shape_t *shape);

#endif /* shape_h */
