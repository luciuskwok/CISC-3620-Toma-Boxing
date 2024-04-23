//
//  mesh_creation.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/10/24.
//

#include "mesh_creation.h"
#include "color.h"
#include "drawing.h"
#include "matrix.h"
#include "vector.h"
#include <math.h>


#pragma mark - Cube

/*
 Vertex numbering
   6--4
  /| /|
 1--2 |
 | 7|-5
 |/ |/
 0--3
 */


// Indexes of cube_vertices[] that defines the vertexes of the face
typedef struct {
	int a, b, c;
} cube_face_index_t;

// Number of points in the mesh
#define MESH_CUBE_VERTICES (8)

// Number of faces: 6 for each cube face * 2 for triangles per face
#define MESH_CUBE_FACES (6 * 2)

vec3_t cube_vertices[MESH_CUBE_VERTICES] = {
    { -1, -1, -1 },
    { -1,  1, -1 },
    {  1,  1, -1 },
    {  1, -1, -1 },
    {  1,  1,  1 },
    {  1, -1,  1 },
    { -1,  1,  1 },
    { -1, -1,  1 }
};

cube_face_index_t cube_faces[MESH_CUBE_FACES] = {
    // front
    { 0, 2, 1 },
    { 0, 3, 2 },
    // right
    { 2, 3, 4 },
    { 3, 5, 4 },
    // back
    { 4, 5, 6 },
    { 5, 7, 6 },
    // left
    { 0, 1, 6 },
    { 0, 6, 7 },
    // top
    { 1, 2, 6 },
    { 2, 4, 6 },
    // bottom
    { 0, 7, 3 },
    { 3, 7, 5 }
};

mesh_t *create_mesh_cube(void) {
	// Create a set of faces that correspond to a cube
	mesh_t *mesh = mesh_new(MESH_CUBE_FACES);
	if (!mesh) {
		return NULL;
	}
	
	for (int i = 0; i < MESH_CUBE_FACES; i++) {
		mesh_face_t *face = &mesh->faces[i];
		face->a = cube_vertices[cube_faces[i].a];
		face->b = cube_vertices[cube_faces[i].b];
		face->c = cube_vertices[cube_faces[i].c];
	}
	return mesh;
}

#pragma mark - Diamond/Pyramid

vec2_t coordinates_for_side(int i, int n) {
	float angle = 2.0f * (float)M_PI * (float)(i) / (float)n;
	vec2_t v;
	v.x = cosf(angle);
	v.y = sinf(angle);
	return v;
}

mesh_t *create_mesh_diamond(int sides, float top, float bottom) {
	mesh_t *mesh = mesh_new(sides * 2);
	if (!mesh) return NULL;

	vec3_t vt = { 0, top, 0 };
	vec3_t vb = { 0, -bottom, 0 };
	
	for (int i = 0; i < sides; i++) {
		vec2_t c0 = coordinates_for_side(i, sides);
		vec2_t c1 = coordinates_for_side(i+1, sides);
		vec3_t v0 = { c0.x, 0, c0.y };
		vec3_t v1 = { c1.x, 0, c1.y };

		// Top face
		mesh_face_t *ft = &mesh->faces[i * 2];
		ft->a = vt;
		ft->b = v0;
		ft->c = v1;

		// Bottom face
		mesh_face_t *fb = &mesh->faces[i * 2 + 1];
		fb->a = vb;
		fb->b = v1;
		fb->c = v0;
	}
	
	return mesh;
}

#pragma mark - Sphere

mesh_t *create_mesh_sphere(int sides) {
	mesh_t *mesh = mesh_new(sides * 2);
	if (!mesh) return NULL;
	return mesh;
}
