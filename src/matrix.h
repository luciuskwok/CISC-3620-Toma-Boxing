//
//  matrix.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/21/24.
//

#ifndef matrix_h
#define matrix_h

#include "vector.h"

// Matrix types

typedef struct {
	float m[3][3];
} mat3_t;

typedef struct {
	float m[4][4];
} mat4_t;

// 2D Matrix Functions
mat3_t mat3_get_identity(void);
mat3_t mat3_translate(mat3_t m, vec2_t t);
mat3_t mat3_scale(mat3_t m, vec2_t s);
mat3_t mat3_rotate(mat3_t m, float a);
mat3_t mat3_multiply(const mat3_t a, const mat3_t b);
vec2_t vec2_mat3_multiply(const vec2_t v, const mat3_t m);

// 3D Matrix Functions
mat4_t mat4_get_identity(void);
mat4_t mat4_translate(mat4_t m, vec3_t t);
mat4_t mat4_scale(mat4_t m, vec3_t s);
mat4_t mat4_pitch(mat4_t m, float a);
mat4_t mat4_roll(mat4_t m, float a);
mat4_t mat4_yaw(mat4_t m, float a);
mat4_t mat4_apply_euler_angles(mat4_t m, vec3_t a);
mat4_t mat4_multiply(const mat4_t a, const mat4_t b);
vec3_t vec3_mat4_multiply(const vec3_t v, const mat4_t m);

#endif /* matrix_h */
