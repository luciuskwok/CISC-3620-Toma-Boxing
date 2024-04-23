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


// Indexes into vertices array that defines the 3 vertexes of the face triangle
typedef struct {
	int a, b, c;
} mesh_face_index_t;

// Number of faces: 6 for each cube face * 2 for triangles per face
#define CUBE_FACE_COUNT (6 * 2)

vec3_t cube_vertices[8] = {
    { -1, -1, -1 },
    { -1,  1, -1 },
    {  1,  1, -1 },
    {  1, -1, -1 },
    {  1,  1,  1 },
    {  1, -1,  1 },
    { -1,  1,  1 },
    { -1, -1,  1 }
};

mesh_face_index_t cube_faces[CUBE_FACE_COUNT] = {
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

mesh_t *mesh_create_cube(void) {
	// Create a set of faces that correspond to a cube
	mesh_t *mesh = mesh_new(CUBE_FACE_COUNT);
	if (!mesh) return NULL;
	
	for (int i = 0; i < CUBE_FACE_COUNT; i++) {
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

mesh_t *mesh_create_diamond(int sides, float top, float bottom) {
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

#pragma mark - Iconsahedron
// Icosahedron code from:
// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

#define ICO_T (1.618034f)

vec3_t icosahedron_vertices[12] = {
	{ -1,  ICO_T, 0 },
	{  1,  ICO_T, 0 },
	{ -1, -ICO_T, 0 },
	{  1, -ICO_T, 0 },
	{  0, -1,  ICO_T},
	{  0,  1,  ICO_T},
	{  0, -1, -ICO_T},
	{  0,  1, -ICO_T},
	{  ICO_T, 0, -1 },
	{  ICO_T, 0,  1 },
	{ -ICO_T, 0, -1 },
	{ -ICO_T, 0,  1 }
};

#define ICOSAHEDRON_FACE_COUNT (20)

// Faces are counter-clockwise in this array
mesh_face_index_t icosahedron_faces[ICOSAHEDRON_FACE_COUNT] = {
	// 5 faces around point 0
	{ 0,11, 5},
	{ 0, 5, 1},
	{ 0, 1, 7},
	{ 0, 7,10},
	{ 0,10,11},
	
	// 5 adjacent faces
	{ 1, 5, 9},
	{ 5,11, 4},
	{11,10, 2},
	{10, 7, 6},
	{ 7, 1, 8},
	
	// 5 faces around point 3
	{ 3, 9, 4},
	{ 3, 4, 2},
	{ 3, 2, 6},
	{ 3, 6, 8},
	{ 3, 8, 9},
	
	// 5 adjacent faces
	{ 4, 9, 5},
	{ 2, 4,11},
	{ 6, 2,10},
	{ 8, 6, 7},
	{ 9, 8, 1}
};

mesh_t *mesh_create_icosahedron(void) {
	mesh_t *mesh = mesh_new(ICOSAHEDRON_FACE_COUNT);
	if (!mesh) return NULL;
	
	for (int i = 0; i < ICOSAHEDRON_FACE_COUNT; i++) {
		mesh_face_t *face = &mesh->faces[i];
		// Swap b and c to make clockwise-direction faces
		face->a = icosahedron_vertices[icosahedron_faces[i].a];
		face->b = icosahedron_vertices[icosahedron_faces[i].c];
		face->c = icosahedron_vertices[icosahedron_faces[i].b];
	}
	return mesh;
}

mesh_t *mesh_create_sphere(int subdivisions) {
	mesh_t *mesh = mesh_create_icosahedron();
	if (!mesh) return NULL;
	
	
	
	return mesh;
}
