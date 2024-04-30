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

#define SONG_DURATION (205.897)

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

void sequencer_update(gameplay_t *scene, double previous_time, double current_time) {
	sequencer_update_bgcolor(scene, current_time);
	

	
}
