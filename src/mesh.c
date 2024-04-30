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
	if (face_count > 0) {
		mesh_face_t *faces = malloc(sizeof(mesh_face_t) * (size_t)face_count);
		if (!faces) {
			fprintf(stderr, "Unable to allocate mesh faces!\n");
			free(mesh);
			return NULL;
		}
		mesh->faces = faces;
	} else {
		mesh->faces = NULL;
	}
	
	mesh->face_count = face_count;
	mesh->children = NULL;
	
	// Visuals
	mesh->is_visible = true;
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
	if (mesh->children) {
		mesh_t **a = (mesh_t **)mesh->children->array;
		int n = mesh->children->length;
		for (int i=0; i<n; i++) {
			mesh_destroy(a[i]);
		}
		array_list_destroy(mesh->children);
	}

	if (mesh->faces) free(mesh->faces);
	free(mesh);
}

bool mesh_add_child(mesh_t *mesh, mesh_t *child) {
	if (!mesh->children) {
		mesh->children = array_list_new(4);
		if (!mesh->children) return false;
	}
	return array_list_add(mesh->children, child);
}

void mesh_set_children_color(mesh_t *mesh, uint32_t line, uint32_t point) {
	mesh->line_color = line;
	mesh->point_color = point;

	if (mesh->children) {
		mesh_t **a = (mesh_t **)mesh->children->array;
		int n = mesh->children->length;
		for (int i=0; i<n; i++) {
			mesh_set_children_color(a[i], line, point);
		}
	}
}

#pragma mark -

void mesh_update(mesh_t *mesh, double delta_time) {
	// Update children
	if (mesh->children) {
		mesh_t **a = (mesh_t **)mesh->children->array;
		int n = mesh->children->length;
		for (int i=0; i<n; i++) {
			mesh_update(a[i], delta_time);
		}
	}

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

void mesh_draw(mesh_t *mesh, mat4_t transform) {
	if (!mesh->is_visible) return;
	
	// Tranformation matrix
	transform = mat4_translate(transform, mesh->position);
	transform = mat4_apply_euler_angles(transform, mesh->rotation);
	transform = mat4_scale(transform, mesh->scale);

	if (mesh->face_count > 0 && mesh->faces) {
		vec3_t a3, b3, c3;
		vec2_t a2, b2, c2;
		vec3_t vab, vac, normal, camera_ray;
		float dot_normal_camera;
		const vec3_t camera_pos = get_camera_position();
		const int point_w = 5;
		
		// Color
		set_line_color_abgr(mesh->line_color);
		set_fill_color_abgr(mesh->point_color);
		
		for (int i = 0; i < mesh->face_count; i++) {
			mesh_face_t face = mesh->faces[i];
			
			a3 = vec3_mat4_multiply(face.a, transform);
			b3 = vec3_mat4_multiply(face.b, transform);
			c3 = vec3_mat4_multiply(face.c, transform);
			
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
				if (mesh->line_color != 0) {
					move_to(a2);
					line_to(b2);
					line_to(c2);
					line_to(a2);
				}
				
				// Points
				if (mesh->point_color != 0) {
					fill_centered_rect((int)a2.x, (int)a2.y, point_w, point_w);
					fill_centered_rect((int)b2.x, (int)b2.y, point_w, point_w);
					fill_centered_rect((int)c2.x, (int)c2.y, point_w, point_w);
				}
			}
		}
	}
	
	// Draw children
	if (mesh->children) {
		mesh_t **a = (mesh_t **)mesh->children->array;
		int n = mesh->children->length;
		for (int i=0; i<n; i++) {
			mesh_draw(a[i], transform);
		}
	}
}

#pragma mark -

void mesh_reset_momentum(mesh_t *mesh) {
	mesh->linear_momentum = vec3_zero();
	mesh->angular_momentum = vec3_zero();
}

