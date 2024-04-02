//
//  cube_mesh.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/10/24.
//

#include "mesh_cube.h"
#include "color.h"
#include "drawing.h"
#include "matrix.h"
#include "vector.h"
#include <math.h>

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

typedef struct {
	vec2_t a, b, c;
} triangle_t;

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

#pragma mark - Fuctions

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
