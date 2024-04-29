//
//  sequencer.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/29/24.
//

#include "sequencer.h"
#include "array_list.h"
#include "color.h"
#include "mesh.h"
#include "shape.h"

#pragma mark - Data



#pragma mark - Functions

void sequencer_start(gameplay_t *scene) {
	// Start sequence by setting background to black
	scene->bg_color = COLOR_ABGR_BLACK;

	// Hide all objects in scene
	shape_t **s = (shape_t **)scene->shapes->array;
	int sn = scene->shapes->length;
	for (int i=0; i<sn; i++) {
		s[i]->is_visible = false;
	}
	mesh_t **m = (mesh_t **)scene->meshes->array;
	int mn = scene->meshes->length;
	for (int i=0; i<mn; i++) {
		m[i]->is_visible = false;
	}

}

void sequencer_update(gameplay_t *scene, double previous_time, double current_time) {
	
}
