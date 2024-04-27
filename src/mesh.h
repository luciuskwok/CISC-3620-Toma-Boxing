//
//  mesh.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/2/24.
//

#ifndef mesh_h
#define mesh_h

#include "matrix.h"
#include "vector.h"
#include "array_list.h"

#include <stdbool.h>


typedef struct {
	vec3_t a, b, c;
} mesh_face_t;

typedef struct {
	// Geometry
	int face_count;
	mesh_face_t *faces;
	array_list_t *children;
	
	// Visuals
	bool is_visible;
	uint32_t line_color;
	uint32_t point_color;
	
	// Physics
	vec3_t scale;
	vec3_t position; // meters
	vec3_t linear_momentum; // meters/second
	vec3_t rotation; // radians
	vec3_t angular_momentum; // radians/second
	bool gravity;
	double lifetime;
} mesh_t;

mesh_face_t mesh_face_make(vec3_t a, vec3_t b, vec3_t c);

mesh_t *mesh_new(int face_count);
void mesh_destroy(mesh_t *mesh);

void mesh_update(mesh_t *mesh, double delta_time);
void mesh_draw(mesh_t *mesh, mat4_t transform);

void mesh_reset_momentum(mesh_t *mesh);
void mesh_add_linear_momentum(mesh_t *mesh, vec3_t momentum);
void mesh_add_pitch(mesh_t *mesh, float x);
void mesh_add_roll(mesh_t *mesh, float x);
void mesh_add_yaw(mesh_t *mesh, float x);

#endif /* mesh_h */
