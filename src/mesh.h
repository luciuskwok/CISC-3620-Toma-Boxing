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
	bool faces_are_lines;
	array_list_t *children;
	
	// Visuals
	bool is_visible;
	bool use_backface_culling;
	uint32_t line_color;
	uint32_t point_color;
	float opacity;

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
bool mesh_add_child(mesh_t *mesh, mesh_t *child);
void mesh_set_children_color(mesh_t *mesh, uint32_t line, uint32_t point);

void mesh_reset_momentum(mesh_t *mesh);
void mesh_set_rotation_degrees(mesh_t *mesh, vec3_t deg);
void mesh_set_angular_momentum_degrees(mesh_t *mesh, vec3_t deg);

void mesh_update(mesh_t *mesh, double delta_time);
void mesh_draw(mesh_t *mesh);
void mesh_draw_recursive(mesh_t *mesh, mat4_t transform, float opacity);

#endif /* mesh_h */
