// vector.h

#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

// Basic vector types

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

// 2D Functions
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mul(vec2_t a, float b);
vec2_t vec2_div(vec2_t a, float b);
vec2_t vec2_rotate(vec2_t p, float a);
float vec2_length(vec2_t v);

// 3D Functions
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mul(vec3_t a, float b);
vec3_t vec3_div(vec3_t a, float b);
float vec3_length(vec3_t v);

vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_dot(vec3_t a, vec3_t b);

#endif /* VECTOR_H */
