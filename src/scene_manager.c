//
//  scene_manager.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_manager.h"
#include "mesh.h"


// Globals
SCENE_INDEX scene_index;


SCENE_INDEX get_scene_index(void) {
	return scene_index;
}

void set_scene_index(SCENE_INDEX x) {
	if (scene_index != x) {
		scene_index = x;
		switch (scene_index) {
			case SCENE_TITLE:
				
				break;
			case SCENE_INSTRUCTIONS:
				
				break;
			case SCENE_GAMEPLAY:
				gameplay_start();
				break;
			case SCENE_RESULTS:
				break;
		}
	}
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
	}
	
	mesh_t **meshes = get_meshes();
	int mesh_count = get_mesh_count();
	for (int i = 0; i < mesh_count; i++) {
		mesh_t *mesh = meshes[i];
		mesh_update(mesh, delta_time);
	}
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
	}
}

void draw_meshes(void) {
	mesh_t **meshes = get_meshes();
	int mesh_count = get_mesh_count();
	for (int i = 0; i < mesh_count; i++) {
		mesh_t *mesh = meshes[i];
		mesh_draw(mesh);
	}
}
