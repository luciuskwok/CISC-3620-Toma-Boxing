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
#include "mesh_creation.h"
#include "shape.h"

#include <stdio.h>

#pragma mark - Data

#define SONG_DURATION (205.897)
#define RADIANF ((float)M_PI * 2.0f)

// Background color timeline
typedef struct {
	double t;
	uint32_t rgb;
} bg_color_keyframe_t;

const int bg_color_timeline_len = 8;
bg_color_keyframe_t bg_color_timeline[bg_color_timeline_len] = {
	{ .t = 0, .rgb = COLOR_RGB_BLACK },
	{ .t = 0.5, .rgb = COLOR_RGB_YELLOW_2 },
	{ .t = 42.75, .rgb = COLOR_RGB_YELLOW_2 },
	{ .t = 43.0, .rgb = COLOR_RGB_WHITE },
	{ .t = 43.5, .rgb = COLOR_RGB_WHITE },
	{ .t = 44.5, .rgb = COLOR_RGB_BLACK },
	{ .t = SONG_DURATION - 1.0, .rgb = COLOR_RGB_YELLOW_2 },
	{ .t = SONG_DURATION, .rgb = COLOR_RGB_BLACK },
};

// Sequencer operations

typedef enum : uint32_t {
	EndSequence,

	ShowShape, 		// .p0: position (x, y), .p1:  rotation (.x: angle)
	ShowMesh, 		// .p0: position, .p1: rotation

	MoveShape, 		// .p0: starting position, .p1: ending position
	MoveMesh, 		// .p0: starting position, .p1: ending position
	RotateShape, 	// .p0: starting rotation, .p1: ending rotation (in fraction of a circle)
	RotateMesh, 	// .p0: starting rotation, .p1: ending rotation
	ScaleShape,  	// .p0: starting scale, .p1: ending scale
	ScaleMesh, 		// .p0: starting scale, .p1: ending scale
} event_commands;

typedef enum : uint32_t {
	Numeral1 = 0,
	Numeral2,
	Numeral3,
	Numeral4,
} seq_mesh_index;

typedef struct {
	event_commands cmd;
	uint32_t target;
	float64_t t0;
	float64_t t1;
	vec3_t p0;
	vec3_t p1;
} sequence_event;

sequence_event seq_events[] = {
	{ ShowMesh, Numeral1, .t0 = 0.5, .t1 = 2.5, .p0 = {-3.0, -1.0, 0}, .p1 = {0, 0, 0} },
	{ ShowMesh, Numeral2, .t0 = 1.0, .t1 = 2.5, .p0 = {-1.0, 0, 0}, .p1 = {0, 0, 0} },
	{ ShowMesh, Numeral3, .t0 = 1.5, .t1 = 2.5, .p0 = {1.0, 1.0, 0}, .p1 = {0, 0, 0} },
	{ ShowMesh, Numeral4, .t0 = 2.0, .t1 = 2.5, .p0 = {3.0, 0, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral1, .t0 = 0.5, .t1 = 0.75, .p0 = {0, 0.75, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral2, .t0 = 1.0, .t1 = 1.25, .p0 = {0, 0.75, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral3, .t0 = 1.5, .t1 = 1.75, .p0 = {0, 0.75, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral4, .t0 = 2.0, .t1 = 2.25, .p0 = {0, 0.75, 0}, .p1 = {0, 0, 0} },
	{ .cmd = EndSequence, }
};

#pragma mark - Functions

void add_numeral_mesh(gameplay_t *scene, char c) {
	mesh_t *mesh;
	mesh = mesh_create_3d_character(c);
	mesh_set_children_color(mesh, rgb_to_abgr(COLOR_RGB_RED_1), 0);
	array_list_add(scene->meshes, mesh);
}

void sequencer_init(gameplay_t *scene) {
	// Create all the objects used by sequence
	
	// Numerals
	add_numeral_mesh(scene, '1');
	add_numeral_mesh(scene, '2');
	add_numeral_mesh(scene, '4');
	add_numeral_mesh(scene, '2');
	
}

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

void sequencer_update_bgcolor(gameplay_t *scene, double current_time) {
	const int n = bg_color_timeline_len;
	if (current_time <= 0.0) {
		scene->bg_color = rgb_to_abgr(bg_color_timeline[0].rgb);
	} else if (current_time >= SONG_DURATION) {
		scene->bg_color = rgb_to_abgr(bg_color_timeline[n - 1].rgb);
	} else {
		for (int i=1; i<n; i++) {
			if (bg_color_timeline[i].t >= current_time) {
				double t0 = bg_color_timeline[i - 1].t;
				double t1 = bg_color_timeline[i].t;
				double x = round((current_time - t0) / (t1 - t0) * 255);
				uint32_t c0 = rgb_to_abgr(bg_color_timeline[i - 1].rgb);
				uint32_t c1 = rgba_to_abgr(bg_color_timeline[i].rgb, (uint8_t)x);
				scene->bg_color = blend_color(c0, c1);
				return;
			}
		}
	}
}

void seq_event_start(gameplay_t *scene, sequence_event *event, double time) {
	mesh_t *mesh;
	shape_t *shape;
	
	switch (event->cmd) {
		case ShowMesh:  // show mesh and set its position and rotation
			mesh = scene->meshes->array[event->target];
			mesh->is_visible = true;
			mesh->position = event->p0;
			mesh->rotation = event->p1;
			break;
		case ShowShape:  // show shape and set its position and rotation
			shape = scene->shapes->array[event->target];
			shape->is_visible = true;
			shape->position = vec3_to_vec2(event->p0);
			shape->rotation = event->p1.x;
			break;
		case RotateShape:
			shape = scene->shapes->array[event->target];
			shape->rotation = event->p0.x * RADIANF;
			break;
		case RotateMesh:
			mesh = scene->meshes->array[event->target];
			mesh->rotation = vec3_mul(event->p0, RADIANF);
			break;
		default:
			fprintf(stderr, "Unknown event command!\n");
			break;
	}
}

void seq_event_update(gameplay_t *scene, sequence_event *event, double time) {
	mesh_t *mesh;
	shape_t *shape;
	float x = (float)((time - event->t0) / (event->t1 - event->t0));
	switch (event->cmd) {
		case ShowMesh: // do nothing
			break;
		case ShowShape: // do nothing
			break;
		case RotateShape:
			shape = scene->shapes->array[event->target];
			shape->rotation = event->p0.x * (1 - x) + event->p1.x * x * RADIANF;
			break;
		case RotateMesh:
			mesh = scene->meshes->array[event->target];
			vec3_t r0 = vec3_mul(event->p0, 1 - x);
			vec3_t r1 = vec3_mul(event->p1, x);
			mesh->rotation = vec3_mul(vec3_add(r0, r1), RADIANF);
			break;
		default:
			fprintf(stderr, "Unknown event command!\n");
			break;
	}
}

void seq_event_end(gameplay_t *scene, sequence_event *event, double time) {
	mesh_t *mesh;
	shape_t *shape;
	switch (event->cmd) {
		case ShowMesh: // hide mesh
			mesh = scene->meshes->array[event->target];
			mesh->is_visible = false;
			break;
		case ShowShape:
			shape = scene->shapes->array[event->target];
			shape->is_visible = false;
			break;
		case RotateShape:
			shape = scene->shapes->array[event->target];
			shape->rotation = event->p1.x * RADIANF;
			break;
		case RotateMesh:
			mesh = scene->meshes->array[event->target];
			mesh->rotation = vec3_mul(event->p1, RADIANF);
			break;
		default:
			fprintf(stderr, "Unknown event command!\n");
			break;
	}
}

void sequencer_update(gameplay_t *scene, double previous_time, double current_time) {
	sequencer_update_bgcolor(scene, current_time);
	
	// Events starting within time period
	for (int i = 0; seq_events[i].cmd != EndSequence; i++) {
		double t0 = seq_events[i].t0;
		if (previous_time < t0 && t0 <= current_time) {
			seq_event_start(scene, &seq_events[i], current_time);
		}
	}

	// Events updating within time period
	for (int i = 0; seq_events[i].cmd != EndSequence; i++) {
		double t0 = seq_events[i].t0;
		double t1 = seq_events[i].t1;
		if (previous_time < t1 && t0 <= current_time) {
			seq_event_update(scene, &seq_events[i], current_time);
		}
	}
	
	// Events ending within time period
	for (int i = 0; seq_events[i].cmd != EndSequence; i++) {
		double t1 = seq_events[i].t1;
		if (previous_time < t1 && t1 <= current_time) {
			seq_event_end(scene, &seq_events[i], current_time);
		}
	}

	
}
