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


typedef struct {
	vec3_t a, b, c;
} mesh_face_t;

typedef struct {
	float pitch, roll, yaw;
} momentum_t;

typedef struct {
	float alpha; 	// yaw
	float beta; 	// pitch
	float gamma;	// roll
} euler_angles_t;

typedef struct {
	int face_count;
	mesh_face_t *faces;
	uint32_t line_color;
	uint32_t point_color;
	mat4_t transform;
	momentum_t momentum;
	double lifetime;
} mesh_t;


mesh_t *mesh_new(int face_count);
void mesh_destroy(mesh_t *mesh);

void mesh_update(mesh_t *mesh, double delta_time);
void mesh_draw(mesh_t *mesh);

void mesh_reset_transform(mesh_t *mesh);
void mesh_reset_momentum(mesh_t *mesh);
void mesh_add_pitch(mesh_t *mesh, float x);
void mesh_add_roll(mesh_t *mesh, float x);
void mesh_add_yaw(mesh_t *mesh, float x);

// Global list of meshes
void add_mesh(mesh_t *mesh);
void remove_all_meshes(void);
int get_mesh_count(void);
mesh_t **get_meshes(void);

#endif /* mesh_h */
