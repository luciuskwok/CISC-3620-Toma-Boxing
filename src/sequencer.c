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

#define SONG_DURATION (92.2371)
#define RADIANF ((float)M_PI * 2.0f)
#define POSITION_FACTOR (0.010f)
#define SCALE_FACTOR (0.010f)
#define ROTATION_FACTOR ((float)M_PI / 180.0f)

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
	SetShapeOpacity, // .p0.x, .p1.x: opacity
	SetMeshOpacity, // .p0.x, .p1.x: opacity

	MoveShape, 		// .p0: starting position, .p1: ending position (in percent)
	MoveMesh, 		// .p0: starting position, .p1: ending position
	RotateShape, 	// .p0: starting rotation, .p1: ending rotation (in degrees)
	RotateMesh, 	// .p0: starting rotation, .p1: ending rotation
	ScaleShape,  	// .p0: starting scale, .p1: ending scale (in percent)
	ScaleMesh, 		// .p0: starting scale, .p1: ending scale
} event_commands;

typedef enum: uint32_t {
	EaseLinear,
	EaseOutElastic,
	EaseBezier,
} easing_curve;

typedef enum : uint32_t {
	Numeral_1_Mesh = 0,
	Numeral_2_Mesh,
	Numeral_3_Mesh,
	Numeral_4_Mesh,
} seq_mesh_index;

typedef enum : uint32_t {
	Studio_Bkgnd_Shape = 0,
	Moon_Shape,
	Heart_Shape,
	Star_Shape,
	Envelope_Shape,
	Monitor_Shape,
	CPU_Shape,
	Microphone_Shape,

} seq_shape_index;

typedef struct {
	event_commands cmd;
	uint32_t target;
	easing_curve ease;
	float64_t t0;
	float64_t t1;
	vec3_t p0;
	vec3_t p1;
} sequence_event;

sequence_event seq_events[] = {
	// 1 2 4 2
	{ ShowMesh, Numeral_1_Mesh, .t0 = 0.5, .t1 = 7.0, .p0 = {-300, -100, 0}, .p1 = {0, 0, 0} },
	{ ShowMesh, Numeral_2_Mesh, .t0 = 1.0, .t1 = 7.0, .p0 = {-100, 0, 0}, .p1 = {0, 0, 0} },
	{ ShowMesh, Numeral_3_Mesh, .t0 = 1.5, .t1 = 7.0, .p0 = {100, 100, 0}, .p1 = {0, 0, 0} },
	{ ShowMesh, Numeral_4_Mesh, .t0 = 2.0, .t1 = 7.0, .p0 = {300, 0, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral_1_Mesh, EaseLinear, .t0 = 0.5, .t1 = 0.75, .p0 = {0, 135, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral_2_Mesh, EaseLinear, .t0 = 1.0, .t1 = 1.25, .p0 = {0, 135, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral_3_Mesh, EaseLinear, .t0 = 1.5, .t1 = 1.75, .p0 = {0, 135, 0}, .p1 = {0, 0, 0} },
	{ RotateMesh, Numeral_4_Mesh, EaseLinear, .t0 = 2.0, .t1 = 2.25, .p0 = {0, 135, 0}, .p1 = {0, 0, 0} },
	{ MoveMesh, Numeral_1_Mesh, EaseBezier, .t0 = 2.5, .t1 = 4.5, .p0 = {-300, -100, 0}, .p1 = {-300, 400, 0} },
	{ MoveMesh, Numeral_2_Mesh, EaseBezier, .t0 = 2.5, .t1 = 4.5, .p0 = {-100, 0, 0}, .p1 = {-100, 500, 0} },
	{ MoveMesh, Numeral_3_Mesh, EaseBezier, .t0 = 2.5, .t1 = 4.5, .p0 = {100, 100, 0}, .p1 = {100, 600, 0} },
	{ MoveMesh, Numeral_4_Mesh, EaseBezier, .t0 = 2.5, .t1 = 4.5, .p0 = {300, 0, 0}, .p1 = {300, 500, 0} },
	
	// 2D Studio
	{ ShowShape, Studio_Bkgnd_Shape, EaseBezier, .t0 = 2.5, .t1 = 44.0, .p0 = {0,-80,0}, .p1 = {0,0,0} },
	{ MoveShape, Studio_Bkgnd_Shape, EaseBezier, .t0 = 2.5, .t1 = 4.6, .p0 = {0,-80,0}, .p1 = {0,15,0} },
	
	// Moon
	{ ShowShape, Moon_Shape, .t0 = 4.6, .t1 = 44.0, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Moon_Shape, EaseOutElastic, .t0 = 4.6, .t1 = 5.6, .p0 = {0,0,0}, .p1 = {33,33,0} },
	{ RotateShape, Moon_Shape, EaseBezier, .t0 = 5.6, .t1 = 8.6, .p0 = {0,0,0}, .p1 = {405,0,0} },
	{ ScaleShape, Moon_Shape, EaseBezier, .t0 = 6.6, .t1 = 8.6, .p0 = {33,33,0}, .p1 = {12,12,0} },
	{ MoveShape, Moon_Shape, EaseBezier, .t0 = 7.6, .t1 = 8.6, .p0 = {0,0,0}, .p1 = {0,18,0} },

	// Heart
	{ ShowShape, Heart_Shape, .t0 = 8.6, .t1 = 44.0, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Heart_Shape, EaseOutElastic, .t0 = 8.6, .t1 = 9.6, .p0 = {0,0,0}, .p1 = {50,50,0} },
	{ RotateShape, Heart_Shape, EaseBezier, .t0 = 9.6, .t1 = 12.6, .p0 = {0,0,0}, .p1 = {340,0,0} },
	{ MoveShape, Heart_Shape, EaseBezier, .t0 = 10.6, .t1 = 12.6, .p0 = {0,0,0}, .p1 = {-50,20,0} },
	{ ScaleShape, Heart_Shape, EaseBezier, .t0 = 11.6, .t1 = 12.6, .p0 = {50,50,0}, .p1 = {18,18,0} },

	// Star
	{ ShowShape, Star_Shape, .t0 = 12.6, .t1 = 44.0, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Star_Shape, EaseOutElastic, .t0 = 12.6, .t1 = 13.6, .p0 = {0,0,0}, .p1 = {35,35,0} },
	{ RotateShape, Star_Shape, EaseBezier, .t0 = 13.6, .t1 = 16.6, .p0 = {0,0,0}, .p1 = {542,0,0} },
	{ MoveShape, Star_Shape, EaseBezier,   .t0 = 14.6, .t1 = 16.6, .p0 = {0,0,0}, .p1 = {50,20,0} },
	{ ScaleShape, Star_Shape, EaseBezier,  .t0 = 15.6, .t1 = 16.6, .p0 = {35,35,0}, .p1 = {10,10,0} },

	// Microphone with stand
	{ ShowShape, Microphone_Shape, .t0 = 16.6, .t1 = 44.0, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Microphone_Shape, EaseOutElastic, .t0 = 16.6, .t1 = 17.6, .p0 = {0,0,0}, .p1 = {35,35,0} },
	{ RotateShape, Microphone_Shape, EaseBezier, .t0 = 17.6, .t1 = 20.6, .p0 = {0,0,0}, .p1 = {360,0,0} },
	{ ScaleShape, Microphone_Shape, EaseBezier,  .t0 = 18.6, .t1 = 20.6, .p0 = {35,35,0}, .p1 = {18,18,0} },
	{ MoveShape, Microphone_Shape, EaseBezier,   .t0 = 19.8, .t1 = 20.6, .p0 = {0,0,0}, .p1 = {15,-23.5,0} },

	// Monitor
	{ ShowShape, Monitor_Shape, .t0 = 20.6, .t1 = 44.0, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Monitor_Shape, EaseOutElastic, .t0 = 20.6, .t1 = 21.6, .p0 = {0,0,0}, .p1 = {100,100,0} },
	{ RotateShape, Monitor_Shape, EaseBezier, .t0 = 21.6, .t1 = 24.6, .p0 = {0,0,0}, .p1 = {360,0,0} },
	{ ScaleShape, Monitor_Shape, EaseBezier,  .t0 = 22.6, .t1 = 24.6, .p0 = {100,100,0}, .p1 = {80,80,0} },
	{ MoveShape, Monitor_Shape, EaseBezier,   .t0 = 23.8, .t1 = 24.6, .p0 = {0,0,0}, .p1 = {-30,-17,0} },

	// CPU
	{ ShowShape, CPU_Shape, .t0 = 24.6, .t1 = 44.0, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, CPU_Shape, EaseOutElastic, .t0 = 24.6, .t1 = 25.6, .p0 = {0,0,0}, .p1 = {100,100,0} },
	{ RotateShape, CPU_Shape, EaseBezier, .t0 = 25.6, .t1 = 28.6, .p0 = {0,0,0}, .p1 = {360,0,0} },
	{ ScaleShape, CPU_Shape, EaseBezier,  .t0 = 26.6, .t1 = 28.6, .p0 = {100,100,0}, .p1 = {80,80,0} },
	{ MoveShape, CPU_Shape, EaseBezier,   .t0 = 27.8, .t1 = 28.6, .p0 = {0,0,0}, .p1 = {50,-19.5,0} },

	{ .cmd = EndSequence }
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
	
	// -- Meshes --
	// Numerals
	add_numeral_mesh(scene, '1');  // Numeral_1_Mesh
	add_numeral_mesh(scene, '2');  // Numeral_2_Mesh
	add_numeral_mesh(scene, '4');  // Numeral_3_Mesh
	add_numeral_mesh(scene, '2');  // Numeral_4_Mesh
	
	// -- Shapes --
	shape_t *s;
	
	// Studio_Bkgnd_Shape: combined window and desktop
	shape_t *window = create_rectangle_shape(0.375f, 0.5f);
	window->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	window->fill_color = rgb_to_abgr(COLOR_RGB_BLUE_1);
	shape_t *desktop = create_rectangle_shape(1.375f, 1.0f/32.0f);
	desktop->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	desktop->fill_color = rgb_to_abgr(COLOR_RGB_SKIN_2);
	desktop->position = vec2_make(0, -0.62f);
	shape_add_child(window, desktop);
	array_list_add(scene->shapes, window);

	// Moon_Shape,
	s = create_crescent_moon_shape();
	s->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	s->fill_color = rgba_to_abgr(COLOR_RGB_YELLOW_1, 228);
	array_list_add(scene->shapes, s);

	// Heart_Shape,
	s = create_heart_shape();
	s->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	s->fill_color = rgba_to_abgr(COLOR_RGB_RED_1, 127);
	array_list_add(scene->shapes, s);

	// Star_Shape,
	s = create_star_shape(5, 1, 0.5f);
	s->line_color = rgb_to_abgr(COLOR_RGB_GREEN_3);
	s->fill_color = rgba_to_abgr(COLOR_RGB_GREEN_1, 127);
	array_list_add(scene->shapes, s);

	// Envelope_Shape,
	s = create_envelope_shape(COLOR_RGB_OUTLINE);
	s->fill_color = rgba_to_abgr(COLOR_RGB_WHITE_3, 255);
	array_list_add(scene->shapes, s);
	
	// Monitor_Shape,
	s = create_monitor_shape();
	array_list_add(scene->shapes, s);

	// CPU_Shape,
	s = create_cpu_shape();
	array_list_add(scene->shapes, s);

	// Microphone_Shape,
	s = create_microphone_with_stand_shape();
	array_list_add(scene->shapes, s);

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
			mesh->position = vec3_mul(event->p0, POSITION_FACTOR);
			mesh->rotation = vec3_mul(event->p1, ROTATION_FACTOR);
			break;
		case ShowShape:  // show shape and set its position and rotation
			shape = scene->shapes->array[event->target];
			shape->is_visible = true;
			shape->position = vec2_mul(vec3_to_vec2(event->p0), POSITION_FACTOR);
			shape->rotation = event->p1.x * ROTATION_FACTOR;
			break;
		case MoveShape:
		case MoveMesh:
		case RotateShape:
		case RotateMesh:
		case ScaleShape:
		case ScaleMesh:
			// Do nothing
			break;
		default:
			fprintf(stderr, "Unknown event command, start!\n");
			break;
	}
}

float apply_easing_curve(easing_curve ease, float x) {
	switch (ease) {
		case EaseOutElastic:
			// https://easings.net/#easeOutElastic
			if (x <= 0.0f) {
				x = 0.0f;
			} else if (x >= 1.0f) {
				x = 1.0f;
			} else {
				const float c4 = 2 * (float)M_PI / 3;
				const float v = 6.0f;
				x = powf(2, -v * x) * sinf((x * v - 0.75f) * c4) + 1;
			}
			break;
		case EaseBezier:
			x = x * x * (3.0f - 2.0f * x);
			break;
		default:
			// Do nothing
			break;
	}
	
	return x;
}

void seq_event_update(gameplay_t *scene, sequence_event *event, double time) {
	mesh_t *mesh;
	shape_t *shape;
	float x = (float)((time - event->t0) / (event->t1 - event->t0));
	x = x <= 1.0? x : 1.0; // Limit to x <= 1.0
	if (event->ease != EaseLinear) {
		x = apply_easing_curve(event->ease, x);
	}
	
	switch (event->cmd) {
		case MoveShape:
			shape = scene->shapes->array[event->target];
			shape->position = vec2_mul(vec3_to_vec2(vec3_interpolate(event->p0, event->p1, x)), POSITION_FACTOR);
			break;
		case MoveMesh:
			mesh = scene->meshes->array[event->target];
			mesh->position = vec3_mul(vec3_interpolate(event->p0, event->p1, x), POSITION_FACTOR);
			break;
		case RotateShape:
			shape = scene->shapes->array[event->target];
			shape->rotation = (event->p0.x * (1 - x) + event->p1.x * x) * ROTATION_FACTOR;
			break;
		case RotateMesh:
			mesh = scene->meshes->array[event->target];
			mesh->rotation = vec3_mul(vec3_interpolate(event->p0, event->p1, x), ROTATION_FACTOR);
			break;
		case ScaleShape:
			shape = scene->shapes->array[event->target];
			shape->scale = vec2_mul(vec3_to_vec2(vec3_interpolate(event->p0, event->p1, x)), SCALE_FACTOR);
			break;
		case ScaleMesh:
			mesh = scene->meshes->array[event->target];
			mesh->scale = vec3_mul(vec3_interpolate(event->p0, event->p1, x), SCALE_FACTOR);
			break;
		case ShowMesh:
		case ShowShape:
			// Do nothing
			break;
		default:
			fprintf(stderr, "Unknown event command, in update!\n");
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
		case MoveShape:
		case MoveMesh:
		case RotateShape:
		case RotateMesh:
		case ScaleShape:
		case ScaleMesh:
			// Do nothing
			break;
		default:
			fprintf(stderr, "Unknown event command, at end!\n");
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
