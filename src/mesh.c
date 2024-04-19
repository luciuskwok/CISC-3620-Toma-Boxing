//
//  mesh.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/2/24.
//

#include "mesh.h"
#include "color.h"
#include "drawing.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


// Global list of 3D meshes
#define MESHES_MAX (256)
mesh_t *meshes[MESHES_MAX];
int mesh_count = 0;


mesh_t *mesh_new(int face_count) {
	mesh_t *mesh = malloc(sizeof(mesh_t));
	if (!mesh) {
		fprintf(stderr, "Unable to allocate mesh!\n");
		return NULL;
	}
	mesh_face_t *faces = malloc(sizeof(mesh_face_t) * (size_t)face_count);
	if (!faces) {
		fprintf(stderr, "Unable to allocate mesh faces!\n");
		free(mesh);
		return NULL;
	}
	
	mesh->face_count = face_count;
	mesh->faces = faces;
	
	// Colors
	mesh->line_color = COLOR_WHITE;
	mesh->point_color = COLOR_WHITE;
	
	// Physics
	mesh->scale = vec3_identity();
	mesh->position = vec3_zero(); // meters
	mesh->linear_momentum = vec3_zero(); // meters/second
	mesh->rotation = vec3_zero(); // radians
	mesh->angular_momentum = vec3_zero(); // radians/second
	mesh->lifetime = 0.0;
	return mesh;
}

void mesh_destroy(mesh_t *mesh) {
	free(mesh->faces);
	free(mesh);
}

void mesh_update(mesh_t *mesh, double delta_time) {
	// Update posiiton & rotation
	float increment = (float)((M_PI / 180.0) * delta_time); // 1 deg/sec
	float dt = (float)delta_time;
	
	// Acceleration due to gravity
	if (mesh->gravity) {
		mesh->linear_momentum.y -= 9.8f * dt;
	}
	
	mesh->position = vec3_add(mesh->position, vec3_mul(mesh->linear_momentum, dt));
	mesh->rotation = vec3_add(mesh->rotation, mesh->angular_momentum);
	mesh->lifetime += delta_time;
}

void mesh_draw(mesh_t *mesh) {
	vec3_t a3, b3, c3;
	vec2_t a2, b2, c2;
	vec3_t vab, vac, normal, camera_ray;
	float dot_normal_camera;
	const vec3_t camera_pos = get_camera_position();
	const int point_w = 5;
	
	// Color
	set_line_color(mesh->line_color);
	set_fill_color(mesh->point_color);
	
	// Tranformation matrix
	mat4_t tr = mat4_get_identity();
	tr = mat4_translate(tr, mesh->position);
	tr = mat4_apply_euler_angles(tr, mesh->rotation);
	tr = mat4_scale(tr, mesh->scale);
	
	for (int i = 0; i < mesh->face_count; i++) {
		mesh_face_t face = mesh->faces[i];
		
		a3 = vec3_mat4_multiply(face.a, tr);
		b3 = vec3_mat4_multiply(face.b, tr);
		c3 = vec3_mat4_multiply(face.c, tr);
		
		// Backface culling
		vab = vec3_sub(b3, a3);
		vac = vec3_sub(c3, a3);
		normal = vec3_cross(vab, vac);
		camera_ray = vec3_sub(a3, camera_pos);
		dot_normal_camera = vec3_dot(camera_ray, normal);
		
		if (dot_normal_camera > 0.0) {
			// Project to 2D
			a2 = perspective_project_point(a3);
			b2 = perspective_project_point(b3);
			c2 = perspective_project_point(c3);
			
			// Lines
			move_to(a2);
			line_to(b2);
			line_to(c2);
			line_to(a2);
			
			// Points
			fill_centered_rect((int)a2.x, (int)a2.y, point_w, point_w);
			fill_centered_rect((int)b2.x, (int)b2.y, point_w, point_w);
			fill_centered_rect((int)c2.x, (int)c2.y, point_w, point_w);
		}
	}
}


#pragma mark - Momentum

void mesh_reset_momentum(mesh_t *mesh) {
	mesh->linear_momentum = vec3_zero();
	mesh->angular_momentum = vec3_zero();
}



#pragma mark - Global list of meshes

void add_mesh(mesh_t *mesh) {
	if (mesh_count < MESHES_MAX) {
		meshes[mesh_count] = mesh;
		mesh_count++;
	} else {
		fprintf(stderr, "Meshes array is full!\n");
	}
}

void remove_all_meshes(void) {
	// Caller is responsible for freeing memory
	mesh_count = 0;
}

int get_mesh_count(void) {
	return mesh_count;
}

mesh_t **get_meshes(void) {
	return meshes;
}
