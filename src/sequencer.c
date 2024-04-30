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

#define END_TIME (205.0)
#define START_FADE_TIME (0.5)
#define END_FADE_TIME (1.0)

typedef enum : uint32_t {
	MoveShape, 		MoveMesh,
	RotateShape,	RotateMesh,
	ScaleShape, 	ScaleMesh,
	ShowShape,		ShowMesh,
	HideShape,		HideMesh,
} op_commands;

typedef struct {
	op_commands command;
	uint32_t target;
	float64_t t0;
	float64_t t1;
	vec3_t p0;
	vec3_t p1;
} sequence_op;

sequence_op seq_ops[] = {
	{ .command = MoveShape, .target = 0, .t0 = 1.0, .t1 = 2.0, .p0 = {0, 0, 0}, .p1 = {1, 0, 0} }
};

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
	// Fade in at start of song
	if (current_time <= START_FADE_TIME) {
		double x = round(current_time / START_FADE_TIME * 255.0);
		scene->bg_color = blend_color(COLOR_ABGR_BLACK, rgba_to_abgr(COLOR_RGB_YELLOW_2, (uint32_t)x));
	}
	
	// Fade out at end of song
	if (current_time >= END_TIME - END_FADE_TIME) {
		double x = round((END_TIME - current_time) / END_FADE_TIME * 255.0);
		scene->bg_color = blend_color(COLOR_ABGR_BLACK, rgba_to_abgr(COLOR_RGB_YELLOW_2, (uint32_t)x));
	}

	
}
