//
//  mesh.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/2/24.
//

#include "mesh.h"
#include "color.h"
#include "drawing.h"
#include "array_list.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


// Global list of 3D meshes
array_list_t *mesh_list = NULL;

mesh_face_t mesh_face_make(vec3_t a, vec3_t b, vec3_t c) {
	mesh_face_t face = { a, b, c };
	return face;
}

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
	mesh->is_visible = true;
	
	// Colors
	mesh->line_color = COLOR_ABGR_WHITE;
	mesh->point_color = COLOR_ABGR_WHITE;
	
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
	if (!mesh->is_visible) return;
	
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

void init_mesh_list(void) {
	mesh_list = make_array_list(16);
}

void add_mesh(mesh_t *mesh) {
	if (mesh_list) {
		bool success = array_list_add(mesh_list, mesh);
		if (!success) {
			fprintf(stderr, "Unable to add mesh!\n");
		}
	}
}

void remove_mesh(mesh_t *mesh) {
	if (mesh_list) {
		bool success = array_list_remove(mesh_list, mesh);
		if (!success) {
			fprintf(stderr, "Unable to remove mesh!\n");
		}
	}
}

void remove_all_meshes(void) {
	// Caller is responsible for freeing memory
	if (mesh_list) {
		array_list_remove_all(mesh_list);
	}
}

int get_mesh_count(void) {
	if (mesh_list) {
		return array_list_length(mesh_list);
	}
	return 0;
}

mesh_t **get_meshes(void) {
	if (mesh_list) {
		return (mesh_t **)array_list_array(mesh_list);
	}
	return NULL;
}
