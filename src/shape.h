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
	float x, y;
	float rotation;
} momentum2d_t;

typedef struct {
	int point_count;
	vec2_t *points;
	bool is_closed;
	uint32_t line_color;
	uint32_t fill_color;
	mat3_t transform;
	momentum2d_t momentum;
	double lifetime;
} shape_t;


shape_t *shape_new(int point_count);
void shape_destroy(shape_t *shape);

void shape_update(shape_t *shape, double delta_time);
void shape_draw(shape_t *shape);

void shape_reset_transform(shape_t *shape);
void shape_reset_momentum(shape_t *shape);

// Global list of 2d shapes
void add_shape(shape_t *shape);
void remove_all_shapes(void);
int get_shape_count(void);
shape_t **get_shapes(void);


#endif /* shape_h */
