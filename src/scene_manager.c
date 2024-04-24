//
//  scene_manager.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_manager.h"
#include "audio_player.h"
#include "mesh.h"
#include "shape.h"


// Globals
SCENE_INDEX scene_index = SCENE_STARTUP;
double scene_lifetime = 0.0;


SCENE_INDEX get_scene_index(void) {
	return scene_index;
}

void set_scene_index(SCENE_INDEX x) {
	remove_all_meshes();
	remove_all_shapes();
	
	// Stop audio player
	stop_music();
	
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
	
	mesh_t **meshes = get_meshes();
	for (int i = 0; i < get_mesh_count(); i++) {
		mesh_update(meshes[i], delta_time);
	}
	
	shape_t **shapes = get_shapes();
	for (int i = 0; i < get_shape_count(); i++) {
		shape_update(shapes[i], delta_time);
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
	mesh_t **meshes = get_meshes();
	for (int i = 0; i < get_mesh_count(); i++) {
		mesh_draw(meshes[i]);
	}
}

void draw_shapes(void) {
	shape_t **shapes = get_shapes();
	for (int i = 0; i < get_shape_count(); i++) {
		shape_draw(shapes[i]);
	}
}

double get_scene_lifetime(void) {
	return scene_lifetime;
}
