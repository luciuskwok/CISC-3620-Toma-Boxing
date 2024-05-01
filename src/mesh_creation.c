//
//  mesh_creation.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/10/24.
//

#include "mesh_creation.h"
#include "atari_text.h"
#include "color.h"
#include "drawing.h"
#include "matrix.h"
#include "vector.h"

#include <math.h>
#include <stdlib.h>


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

const vec3_t cube_vertices[8] = {
    { -1, -1, -1 },
    { -1,  1, -1 },
    {  1,  1, -1 },
    {  1, -1, -1 },
    {  1,  1,  1 },
    {  1, -1,  1 },
    { -1,  1,  1 },
    { -1, -1,  1 }
};

// Faces use right-hand rule and should be counter-clockwise.
const mesh_face_index_t cube_faces[CUBE_FACE_COUNT] = {
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
	const float radian = (float)(2.0 * M_PI);
	float angle = radian * (float)i / (float)n;
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
// ICO_T = (1.0 + sqrt(5.0)) / 2.0
#define ICO_T (1.618034f)

const vec3_t icosahedron_vertices[12] = {
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
const mesh_face_index_t icosahedron_faces[ICOSAHEDRON_FACE_COUNT] = {
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

vec3_t project_on_unit_sphere(vec3_t a) {
	float len = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
	vec3_t b = { a.x/len, a.y/len, a.z/len };
	return b;
}

mesh_t *mesh_create_icosahedron(void) {
	mesh_t *mesh = mesh_new(ICOSAHEDRON_FACE_COUNT);
	if (!mesh) return NULL;
	
	const vec3_t *v = icosahedron_vertices;
	const mesh_face_index_t *f = icosahedron_faces;
	
	for (int i = 0; i < ICOSAHEDRON_FACE_COUNT; i++) {
		mesh_face_t *face = &mesh->faces[i];
		
		// Project onto unit sphere for consistent size
		// Swap b and c to make clockwise-direction faces
		face->a = project_on_unit_sphere(v[f[i].a]);
		face->b = project_on_unit_sphere(v[f[i].c]);
		face->c = project_on_unit_sphere(v[f[i].b]);
	}
	return mesh;
}

vec3_t sphere_middle_point(vec3_t a, vec3_t b) {
	// Gets the middle point between two points on a sphere, projected onto the unit sphere
	vec3_t c;
	c.x = (a.x + b.x) / 2.0f;
	c.y = (a.y + b.y) / 2.0f;
	c.z = (a.z + b.z) / 2.0f;
	return c;
}

mesh_t *mesh_create_sphere(int subdivisions) {
	mesh_t *mesh = mesh_create_icosahedron();
	if (!mesh) return NULL;
	
	for (int i = 0; i < subdivisions; i++) {
		// Subdivide triangles
		int n = mesh->face_count;
		mesh_face_t *new_faces = malloc((size_t)n * 4 * sizeof(mesh_face_t));
		if (!new_faces) return mesh;
		
		for (int j = 0; j < mesh->face_count; j++) {
			mesh_face_t face = mesh->faces[j];
			vec3_t a = face.a;
			vec3_t b = face.b;
			vec3_t c = face.c;
			vec3_t d = project_on_unit_sphere(sphere_middle_point(a, b));
			vec3_t e = project_on_unit_sphere(sphere_middle_point(b, c));
			vec3_t f = project_on_unit_sphere(sphere_middle_point(c, a));
			
			new_faces[j * 4 + 0] = mesh_face_make(a, d, f);
			new_faces[j * 4 + 1] = mesh_face_make(b, e, d);
			new_faces[j * 4 + 2] = mesh_face_make(c, f, e);
			new_faces[j * 4 + 3] = mesh_face_make(d, e, f);
		}
		
		// Replace old faces with new faces
 		free(mesh->faces);
		mesh->face_count = n * 4;
		mesh->faces = new_faces;
	}
	
	return mesh;
}

#pragma mark -

mesh_t *mesh_create_grid(int subdivisions) {
	mesh_t *m = mesh_new(2 * (subdivisions + 1));
	if (!m) return NULL;
	m->faces_are_lines = true;
	m->use_backface_culling = false;
	
	mesh_face_t *f = m->faces;
	for (int i=0; i<=subdivisions; i++) {
		float x = (float)i / (float)subdivisions * 2.0f - 1.0f;
		f[i*2].a = vec3_make(-1, x, 0);
		f[i*2].b = vec3_make(1, x, 0);
		f[i*2].c = vec3_zero();
		
		f[i*2+1].a = vec3_make(x, -1, 0);
		f[i*2+1].b = vec3_make(x, 1, 0);
		f[i*2+1].c = vec3_zero();
	}
	
	return m;
}

mesh_t *mesh_create_pyramid(void) {
	mesh_t *m = mesh_new(6);
	if (!m) return NULL;

	vec3_t a = {0, 1, 0};
	vec3_t b = {-1, 0, 1};
	vec3_t c = {1, 0, 1};
	vec3_t d = {1, 0, -1};
	vec3_t e = {-1, 0, -1};
	mesh_face_t *f = m->faces;
	
	f[0] = mesh_face_make(a, c, b);
	f[1] = mesh_face_make(a, d, c);
	f[2] = mesh_face_make(a, e, d);
	f[3] = mesh_face_make(a, b, e);
	f[4] = mesh_face_make(b, c, e);
	f[5] = mesh_face_make(d, e, c);
	
	return m;
}


#pragma mark -

mesh_t *mesh_create_3d_character(char c) {
	mesh_t *group = mesh_new(0);
	atari_char_data_t d = atari_get_char_data(c);
	const float thickness = 1.0f / 32.0f;
	const float eighth = 1.0f / 8.0f;
	const float fourth = 1.0f / 4.0f;
	const float dx = fourth * 3.5f;
	const float dy = fourth * 3.5f;

	for (int i=0; i<8; i++) {
		uint8_t x = d.a[i];
		for (int j=0; j<8; j++) {
			if (x & 1) {
				mesh_t *block = mesh_create_cube();
				block->scale = vec3_make(eighth, eighth, thickness);
				block->position = vec3_make(dx - (float)j * fourth, dy - (float)i * fourth, 0);
				block->point_color = 0;
				mesh_add_child(group, block);
			}
			x = x >> 1;
		}
	}
	
	return group;
}
