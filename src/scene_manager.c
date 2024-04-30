//
//  scene_manager.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_manager.h"
#include "audio_player.h"
#include "array_list.h"
#include <stdio.h>


// Globals
SCENE_INDEX scene_index = SCENE_STARTUP;
double scene_lifetime = 0.0;
array_list_t *shape_list;
array_list_t *mesh_list;
bool is_scene_paused = false;


void scene_manager_init(void) {
	// Create array lists
	shape_list = array_list_new(16);
	mesh_list = array_list_new(16);
	if (!shape_list || !mesh_list) {
		fprintf(stderr, "Could not increase size of array_list.\n");
	}
	
	// Init all scenes
	title_init();
	instructions_init();
	gameplay_init();
	results_init();
}

void scene_add_mesh(mesh_t *mesh) {
	bool success = array_list_add(mesh_list, mesh);
	if (!success) {
		fprintf(stderr, "Could not add to mesh_list.\n");
	}
}

void scene_add_shape(shape_t *shape) {
	bool success = array_list_add(shape_list, shape);
	if (!success) {
		fprintf(stderr, "Could not add to shape_list.\n");
	}
}

void set_scene_index(SCENE_INDEX x) {
	array_list_remove_all(shape_list);
	array_list_remove_all(mesh_list);
	
	// Stop audio player
	stop_music();
		
	// Unpause
	is_scene_paused = false;

	scene_index = x;
	switch (scene_index) {
		case SCENE_TITLE:
			title_start();
			break;
		case SCENE_INSTRUCTIONS:
			instructions_start();
			break;
		case SCENE_GAMEPLAY:
			gameplay_start();
			break;
		case SCENE_RESULTS:
			results_start();
			break;
		case SCENE_STARTUP:
			break;
	}

	scene_lifetime = 0.0;
}

void update_scene(double delta_time) {
	switch (scene_index) {
		case SCENE_TITLE:
			title_update(delta_time);
			break;
		case SCENE_INSTRUCTIONS:
			instructions_update(delta_time);
			break;
		case SCENE_GAMEPLAY:
			gameplay_update(delta_time);
			break;
		case SCENE_RESULTS:
			results_update(delta_time);
			break;
		case SCENE_STARTUP:
			break;
	}
	
	if (!is_scene_paused) {
		mesh_t **m = (mesh_t **)mesh_list->array;
		int mn = mesh_list->length;
		for (int i = 0; i < mn; i++) {
			mesh_update(m[i], delta_time);
		}
		
		shape_t **s = (shape_t **)shape_list->array;
		int sn = shape_list->length;
		for (int i = 0; i < sn; i++) {
			shape_update(s[i], delta_time);
		}
	}

	scene_lifetime += delta_time;
}

void draw_scene(void) {
	switch (scene_index) {
		case SCENE_TITLE:
			title_render();
			break;
		case SCENE_INSTRUCTIONS:
			instructions_render();
			break;
		case SCENE_GAMEPLAY:
			gameplay_render();
			break;
		case SCENE_RESULTS:
			results_render();
			break;
		case SCENE_STARTUP:
			break;
	}
}

void draw_meshes(void) {
	mesh_t **m = (mesh_t **)mesh_list->array;
	int mn = mesh_list->length;
	mat4_t transform = mat4_identity();
	for (int i = 0; i < mn; i++) {
		mesh_draw(m[i], transform);
	}
}

void draw_shapes(void) {
	shape_t **s = (shape_t **)shape_list->array;
	int sn = shape_list->length;
	mat3_t transform = mat3_identity();

	for (int i = 0; i < sn; i++) {
		shape_draw(s[i], transform);
	}
}
