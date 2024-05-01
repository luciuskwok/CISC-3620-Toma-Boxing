//
//  shape_creation.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 5/1/24.
//

#ifndef shape_creation_h
#define shape_creation_h

#include "shape.h"
#include <stdint.h>

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

shape_t *create_monitor_shape(void);
shape_t *create_cpu_shape(void);
shape_t *create_lighting_bolt_shape(void);
shape_t *create_smoke_circle_shape(void);

shape_t *create_toemaniac_shape(void);

shape_t *create_envelope_shape(uint32_t line_color);

#endif /* shape_creation_h */
