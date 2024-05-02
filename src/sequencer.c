//
//  sequencer.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/29/24.
//

#include "sequencer.h"
#include "array_list.h"
#include "audio_player.h"
#include "color.h"
#include "mesh.h"
#include "mesh_creation.h"
#include "shape.h"
#include "shape_creation.h"

#include <stdio.h>

#pragma mark - Data

#define RADIANF ((float)M_PI * 2.0f)
#define POSITION_FACTOR (0.010f)
#define SCALE_FACTOR (0.010f)
#define ROTATION_FACTOR ((float)M_PI / 180.0f)

// Background color timeline
typedef struct {
	double t;
	uint32_t rgb;
} bg_color_keyframe_t;

bg_color_keyframe_t bg_color_timeline[] = {
	{ .t = 0, .rgb = COLOR_RGB_BLACK },
	{ .t = 0.5, .rgb = COLOR_RGB_YELLOW_2 },
	{ .t = 41.6, .rgb = COLOR_RGB_YELLOW_2 },
	{ .t = 42.1, .rgb = COLOR_RGB_WHITE },
	{ .t = 42.6, .rgb = COLOR_RGB_WHITE },
	{ .t = 44.1, .rgb = COLOR_RGB_BLACK },
	{ .t = 73.6, .rgb = COLOR_RGB_BLACK },
	{ .t = 74.1, .rgb = COLOR_RGB_YELLOW_2 },
	{ .t = SONG_DURATION - 1.0, .rgb = COLOR_RGB_YELLOW_2 },
	{ .t = SONG_DURATION, .rgb = COLOR_RGB_BLACK },
	{ .t = INFINITY, .rgb = COLOR_RGB_BLACK }
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
	EaseInQuad,
	EaseOutQuad,
	EaseInCubic,
	EaseOutCubic,
	EaseOutBounce,
	EaseOutElastic,
	EaseBezier,
} easing_curve;

typedef enum : uint32_t {
	Numeral_1_Mesh = 0,
	Numeral_2_Mesh,
	Numeral_3_Mesh,
	Numeral_4_Mesh,
	Grid_Mesh,
	Mtn_1_Mesh,
	Mtn_2_Mesh,
	Mtn_3_Mesh,
	Mtn_4_Mesh,
	Mtn_5_Mesh,
	Radio_Tower_Mesh,
	Fireworks_1_Mesh,
	Fireworks_2_Mesh,
	UFO_Mesh,
	Ball_Mesh,
	Cone_1_Mesh,
	Cone_2_Mesh,
} seq_mesh_index;

typedef enum : uint32_t {
	Studio_Bkgnd_Shape = 0,
	Moon_Shape,
	Heart_Shape,
	Star_Shape,
	Envelope_Shape,
	Monitor_Shape,
	Smoke_1_Shape,
	Smoke_2_Shape,
	Smoke_3_Shape,
	CPU_Shape,
	Microphone_Shape,
	Smoke_4_Shape,
	Zap_1_Shape,
	Zap_2_Shape,
	Explosion_Shape,

} seq_shape_index;

typedef struct {
	event_commands cmd;
	uint32_t target;
	easing_curve ease;
	double t0;
	double t1;
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
	
#define T_END (42.1)
	// 2D Studio
#define T0 (2.5)
	{ ShowShape, Studio_Bkgnd_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,-80,0}, .p1 = {0,0,0} },
	{ MoveShape, Studio_Bkgnd_Shape, EaseBezier, .t0 = T0, .t1 = T0+4.2, .p0 = {0,-80,0}, .p1 = {0,15,0} },
#undef T0
	
	// Moon
#define T0 (6.6)
	{ ShowShape, Moon_Shape, .t0 = T0-2, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Moon_Shape, EaseOutElastic, .t0 = T0-2, .t1 = T0+1.25, .p0 = {0,0,0}, .p1 = {33,33,0} },
	{ RotateShape, Moon_Shape, EaseBezier, .t0 = T0+1, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {405,0,0} },
	{ ScaleShape, Moon_Shape, EaseBezier, .t0 = T0+2, .t1 = T0+4, .p0 = {33,33,0}, .p1 = {12,12,0} },
	{ MoveShape, Moon_Shape, EaseBezier, .t0 = T0+3, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {0,18,0} },
#undef T0
	
	// Heart
#define T0 (10.6)
	{ ShowShape, Heart_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Heart_Shape, EaseOutElastic, .t0 = T0, .t1 = T0+1.25, .p0 = {0,0,0}, .p1 = {50,50,0} },
	{ RotateShape, Heart_Shape, EaseBezier, .t0 = T0+1, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {-380,0,0} },
	{ ScaleShape, Heart_Shape, EaseBezier, .t0 = T0+2, .t1 = T0+4, .p0 = {50,50,0}, .p1 = {18,18,0} },
	{ MoveShape, Heart_Shape, EaseBezier, .t0 = T0+3, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {-50,20,0} },
#undef T0

	// Star
#define T0 (14.6)
	{ ShowShape, Star_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Star_Shape, EaseOutElastic, .t0 = T0, .t1 = T0+1.25, .p0 = {0,0,0}, .p1 = {35,35,0} },
	{ RotateShape, Star_Shape, EaseBezier, .t0 = T0+1, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {542,0,0} },
	{ ScaleShape, Star_Shape, EaseBezier,  .t0 = T0+2, .t1 = T0+4, .p0 = {35,35,0}, .p1 = {10,10,0} },
	{ MoveShape, Star_Shape, EaseBezier,   .t0 = T0+3, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {50,20,0} },
#undef T0

	// Microphone with stand
#define T0 (18.6)
	{ ShowShape, Microphone_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Microphone_Shape, EaseOutElastic, .t0 = T0, .t1 = T0+1.25, .p0 = {0,0,0}, .p1 = {35,35,0} },
	{ RotateShape, Microphone_Shape, EaseBezier, .t0 = T0+1, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {-360,0,0} },
	{ ScaleShape, Microphone_Shape, EaseBezier,  .t0 = T0+2, .t1 = T0+4, .p0 = {35,35,0}, .p1 = {18,18,0} },
	{ MoveShape, Microphone_Shape, EaseBezier,   .t0 = T0+3, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {15,-23.5,0} },
#undef T0
	// Monitor
#define T0 (22.6)
	{ ShowShape, Monitor_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, Monitor_Shape, EaseOutElastic, .t0 = T0, .t1 = T0+1.25, .p0 = {0,0,0}, .p1 = {100,100,0} },
	{ RotateShape, Monitor_Shape, EaseBezier, .t0 = T0+1, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {360,0,0} },
	{ ScaleShape, Monitor_Shape, EaseBezier,  .t0 = T0+2, .t1 = T0+4, .p0 = {100,100,0}, .p1 = {80,80,0} },
	{ MoveShape, Monitor_Shape, EaseBezier,   .t0 = T0+3, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {-30,-17,0} },
#undef T0
	
	// CPU
#define T0 (26.6)
	{ ShowShape, CPU_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleShape, CPU_Shape, EaseOutElastic, .t0 = T0, .t1 = T0+1.25, .p0 = {0,0,0}, .p1 = {100,100,0} },
	{ RotateShape, CPU_Shape, EaseBezier, .t0 = T0+1, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {-360,0,0} },
	{ ScaleShape, CPU_Shape, EaseBezier,  .t0 = T0+2, .t1 = T0+4, .p0 = {100,100,0}, .p1 = {80,80,0} },
	{ MoveShape, CPU_Shape, EaseBezier,   .t0 = T0+3, .t1 = T0+4, .p0 = {0,0,0}, .p1 = {50,-19.5,0} },
#undef T0
	
	// Zap
#define T0 (30.6)
	{ ShowShape, Zap_1_Shape, .t0 = T0, .t1 = T0+0.5, .p0 = {-30,-17,0}, .p1 = {0,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+1, .t1 = T0+1.5, .p0 = {-20,-12,0}, .p1 = {-22,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+2, .t1 = T0+2.5, .p0 = {-40,-15,0}, .p1 = {25,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+3, .t1 = T0+3.5, .p0 = {-30,-17,0}, .p1 = {-45,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+4, .t1 = T0+4.5, .p0 = {-30,-17,0}, .p1 = {0,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+5, .t1 = T0+5.5, .p0 = {-20,-12,0}, .p1 = {-22,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+6, .t1 = T0+6.5, .p0 = {-30,-17,0}, .p1 = {0,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+7, .t1 = T0+7.5, .p0 = {-40,-12,0}, .p1 = {22,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+8, .t1 = T0+8.5, .p0 = {-30,-17,0}, .p1 = {-22,0,0} },
	{ ShowShape, Zap_1_Shape, .t0 = T0+9, .t1 = T0+9.5, .p0 = {-40,-12,0}, .p1 = {45,0,0} },

	{ ShowShape, Zap_2_Shape, .t0 = T0+0.75, .t1 = T0+1.25, .p0 = {45,7,0}, .p1 = {22,0,0} },
	{ ShowShape, Zap_2_Shape, .t0 = T0+1.75, .t1 = T0+2.25, .p0 = {55,12,0}, .p1 = {-22,0,0} },
#undef T0

	// Smoke
	{ ShowShape, Smoke_1_Shape, 			.t0 = 31.6, .t1 = T_END, .p0 = {50,-25,0}, .p1 = {0,0,0} },
	{ ShowShape, Smoke_2_Shape, 			.t0 = 32.6, .t1 = T_END, .p0 = {45,-25,0}, .p1 = {10,0,0} },
	{ ShowShape, Smoke_3_Shape, 			.t0 = 33.6, .t1 = T_END, .p0 = {55,-25,0}, .p1 = {-10,0,0} },

	{ MoveShape, Smoke_1_Shape, EaseInQuad, .t0 = 31.6, .t1 = 33.6, .p0 = {50,-25,0}, .p1 = {50,55,0} },
	{ ScaleShape, Smoke_1_Shape, EaseOutQuad, .t0 = 31.6, .t1 = 33.6, .p0 = {80,80,0}, .p1 = {160,160,0} },
	{ MoveShape, Smoke_2_Shape, EaseInQuad, .t0 = 32.6, .t1 = 34.6, .p0 = {45,-25,0}, .p1 = {40,55,0} },
	{ ScaleShape, Smoke_2_Shape, EaseOutQuad, .t0 = 32.6, .t1 = 34.6, .p0 = {80,80,0}, .p1 = {160,160,0} },
	{ MoveShape, Smoke_3_Shape, EaseInQuad, .t0 = 33.6, .t1 = 35.6, .p0 = {55,-25,0}, .p1 = {60,55,0} },
	{ ScaleShape, Smoke_3_Shape, EaseOutQuad, .t0 = 33.6, .t1 = 35.6, .p0 = {80,80,0}, .p1 = {160,160,0} },

	{ ShowShape, Smoke_4_Shape, 			.t0 = 35.6, .t1 = T_END, .p0 = {50,-15,0}, .p1 = {0,0,0} },

	{ MoveShape, Smoke_1_Shape, EaseInQuad, .t0 = 34.1, .t1 = 36.0, .p0 = {50,-15,0}, .p1 = {50,50,0} },
	{ ScaleShape, Smoke_1_Shape, EaseOutQuad, .t0 = 34.1, .t1 = 36.0, .p0 = {120,120,0}, .p1 = {240,240,0} },
	{ MoveShape, Smoke_2_Shape, EaseInQuad, .t0 = 34.6, .t1 = 36.5, .p0 = {40,-15,0}, .p1 = {20,50,0} },
	{ ScaleShape, Smoke_2_Shape, EaseOutQuad, .t0 = 34.6, .t1 = 36.5, .p0 = {120,120,0}, .p1 = {240,240,0} },
	{ MoveShape, Smoke_3_Shape, EaseInQuad, .t0 = 35.1, .t1 = 37.0, .p0 = {60,-15,0}, .p1 = {80,50,0} },
	{ ScaleShape, Smoke_3_Shape, EaseOutQuad, .t0 = 35.1, .t1 = 37.0, .p0 = {120,120,0}, .p1 = {240,240,0} },
	{ MoveShape, Smoke_4_Shape, EaseInQuad, .t0 = 35.6, .t1 = 37.5, .p0 = {50,-15,0}, .p1 = {40,50,0} },
	{ ScaleShape, Smoke_4_Shape, EaseOutQuad, .t0 = 35.6, .t1 = 37.5, .p0 = {120,120,0}, .p1 = {240,240,0} },

	{ MoveShape, Smoke_1_Shape, EaseInQuad, .t0 = 36.1, .t1 = T_END, .p0 = {40,-15,0}, .p1 = {50,50,0} },
	{ ScaleShape, Smoke_1_Shape, EaseOutQuad, .t0 = 36.1, .t1 = T_END, .p0 = {160,160,0}, .p1 = {320,320,0} },
	{ MoveShape, Smoke_2_Shape, EaseInQuad, .t0 = 36.6, .t1 = T_END, .p0 = {30,-15,0}, .p1 = {20,50,0} },
	{ ScaleShape, Smoke_2_Shape, EaseOutQuad, .t0 = 36.6, .t1 = T_END, .p0 = {160,160,0}, .p1 = {320,320,0} },
	{ MoveShape, Smoke_3_Shape, EaseInQuad, .t0 = 37.1, .t1 = T_END, .p0 = {50,-15,0}, .p1 = {80,50,0} },
	{ ScaleShape, Smoke_3_Shape, EaseOutQuad, .t0 = 37.1, .t1 = T_END, .p0 = {160,160,0}, .p1 = {320,320,0} },
	{ MoveShape, Smoke_4_Shape, EaseInQuad, .t0 = 37.6, .t1 = T_END, .p0 = {0,-15,0}, .p1 = {-20,50,0} },
	{ ScaleShape, Smoke_4_Shape, EaseOutQuad, .t0 = 37.6, .t1 = T_END, .p0 = {160,160,0}, .p1 = {320,320,0} },

	// Explosion
	{ ShowShape, Explosion_Shape, 				.t0 = T_END-0.5, .t1 = T_END+0.25, .p0 = {50,-19,0}, .p1 = {0,0,0} },
	{ ScaleShape, Explosion_Shape, EaseOutQuad, .t0 = T_END-0.5, .t1 = T_END, .p0 = {1,1,1}, .p1 = {200, 200,0} },
#undef T_END
	
	// -- 3D; Chorus --
	// Grid
#define T0 (43.1)
	{ ShowMesh, Grid_Mesh, 				.t0 = T0, .t1 = 73.6, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ ScaleMesh, Grid_Mesh, 			.t0 = T0, .t1 = T0, .p0 = {400,400,400}, .p1 = {400,400,400} },
	{ SetMeshOpacity, Grid_Mesh, EaseBezier, .t0 = T0, .t1 = T0+1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ RotateMesh, Grid_Mesh, EaseOutBounce, .t0 = T0+1, .t1 = T0+5, .p0 = {0,0,0}, .p1 = {90,0,0} },
	{ MoveMesh, Grid_Mesh, EaseOutBounce, .t0 = T0+1, .t1 = T0+5, .p0 = {0,0,0}, .p1 = {0,-100,100} },
	
	// Mtn 1
#undef T0
#define T0 (47.1)
	{ ShowMesh, Mtn_1_Mesh, 				.t0 = T0, .t1 = 73.6, .p0 = {75,-100,650}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Mtn_1_Mesh, EaseBezier, .t0 = T0, .t1 = T0+1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ ScaleMesh, Mtn_1_Mesh, 				.t0 = T0, .t1 = T0, .p0 = {200,1,100}, .p1 = {200,1,100} },
	{ ScaleMesh, Mtn_1_Mesh, EaseOutElastic, .t0 = T0+0.5, .t1 = T0+2, .p0 = {200,1,100}, .p1 = {200,200,100} },
	
	// Mtn 2
#undef T0
#define T0 (48.1)
	{ ShowMesh, Mtn_2_Mesh, 				.t0 = T0, .t1 = 73.6, .p0 = {-300,-100,650}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Mtn_2_Mesh, EaseBezier, .t0 = T0, .t1 = T0+1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ ScaleMesh, Mtn_2_Mesh, 				.t0 = T0, .t1 = T0, .p0 = {200,1,100}, .p1 = {200,1,100} },
	{ ScaleMesh, Mtn_2_Mesh, EaseOutElastic, .t0 = T0+0.5, .t1 = T0+2, .p0 = {200,1,100}, .p1 = {200,125,100} },
	
	// Mtn 3
#undef T0
#define T0 (49.1)
	{ ShowMesh, Mtn_3_Mesh, 				.t0 = T0, .t1 = 73.6, .p0 = {350,-100,650}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Mtn_3_Mesh, EaseBezier, .t0 = T0, .t1 = T0+1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ ScaleMesh, Mtn_3_Mesh, 				.t0 = T0, .t1 = T0, .p0 = {100,1,100}, .p1 = {100,1,100} },
	{ ScaleMesh, Mtn_3_Mesh, EaseOutElastic, .t0 = T0+0.5, .t1 = T0+2, .p0 = {100,1,100}, .p1 = {100,75,100} },
	
	// Mtn 4
#undef T0
#define T0 (50.1)
	{ ShowMesh, Mtn_4_Mesh, 				.t0 = T0, .t1 = 73.6, .p0 = {550,-100,400}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Mtn_4_Mesh, EaseBezier, .t0 = T0, .t1 = T0+1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ ScaleMesh, Mtn_4_Mesh, 				.t0 = T0, .t1 = T0, .p0 = {100,1,200}, .p1 = {100,1,200} },
	{ ScaleMesh, Mtn_4_Mesh, EaseOutElastic, .t0 = T0+0.5, .t1 = T0+2, .p0 = {100,1,200}, .p1 = {100,75,200} },
	
	// Mtn 5
#undef T0
#define T0 (51.1)
	{ ShowMesh, Mtn_5_Mesh, 				.t0 = T0, .t1 = 73.6, .p0 = {-550,-100,350}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Mtn_5_Mesh, EaseBezier, .t0 = T0, .t1 = T0+1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ ScaleMesh, Mtn_5_Mesh, 				.t0 = T0, .t1 = T0, .p0 = {100,1,300}, .p1 = {100,1,300} },
	{ ScaleMesh, Mtn_5_Mesh, EaseOutElastic, .t0 = T0+0.5, .t1 = T0+2, .p0 = {100,1,300}, .p1 = {100,75,300} },
	
	// Radio Tower
#undef T0
#define T0 (52.1)
	{ ShowMesh, Radio_Tower_Mesh, 				.t0 = T0, .t1 = 73.6, .p0 = {-200,-100,100}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Radio_Tower_Mesh, EaseBezier, .t0 = T0, .t1 = T0+1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ ScaleMesh, Radio_Tower_Mesh, 				.t0 = T0, .t1 = T0, .p0 = {12,1,12}, .p1 = {12,1,12} },
	{ ScaleMesh, Radio_Tower_Mesh, EaseOutElastic, .t0 = T0+0.5, .t1 = T0+2, .p0 = {12,1,12}, .p1 = {12,200,12} },
	
	// Fireworks
#undef T0
#define S0 (2)
#define S1 (200)
#define M1 Fireworks_1_Mesh
#define M2 Fireworks_2_Mesh

#define T0 (54.1)
	{ ShowMesh, M1, 		.t0 = T0, .t1 = 73.6, .p0 = {200,100,100}, .p1 = {0,0,0} },
	{ SetMeshOpacity, M1, 	.t0 = T0, .t1 = T0+2, .p0 = {0.5,0,0}, .p1 = {0,0,0} },
	{ ScaleMesh, M1, EaseOutCubic, .t0 = T0, .t1 = T0+2, .p0 = {S0,S0,S0}, .p1 = {S1,S1,S1} },
#undef T0
#define T0 (56.1)
	{ ShowMesh, M2, 		.t0 = T0, .t1 = 73.6, .p0 = {-350,80,200}, .p1 = {0,0,0} },
	{ SetMeshOpacity, M2, 	.t0 = T0, .t1 = T0+2, .p0 = {0.5,0,0}, .p1 = {0,0,0} },
	{ ScaleMesh, M2, EaseOutCubic, .t0 = T0, .t1 = T0+2, .p0 = {S0,S0,S0}, .p1 = {S1,S1,S1} },
#undef T0
#define T0 (58.1)
	{ ShowMesh, M1, 		.t0 = T0, .t1 = 73.6, .p0 = {350,104,150}, .p1 = {0,0,0} },
	{ SetMeshOpacity, M1, 	.t0 = T0, .t1 = T0+2, .p0 = {0.5,0,0}, .p1 = {0,0,0} },
	{ ScaleMesh, M1, EaseOutCubic, .t0 = T0, .t1 = T0+2, .p0 = {S0,S0,S0}, .p1 = {S1,S1,S1} },
#undef T0
#define T0 (58.5)
	{ ShowMesh, M2, 		.t0 = T0, .t1 = 73.6, .p0 = {-200,80,120}, .p1 = {0,0,0} },
	{ SetMeshOpacity, M2, 	.t0 = T0, .t1 = T0+2, .p0 = {0.5,0,0}, .p1 = {0,0,0} },
	{ ScaleMesh, M2, EaseOutCubic, .t0 = T0, .t1 = T0+2, .p0 = {S0,S0,S0}, .p1 = {S1,S1,S1} },
#undef T0
#define T0 (59.0)
	{ ShowMesh, M1, 		.t0 = T0, .t1 = 73.6, .p0 = {0,100,120}, .p1 = {0,0,0} },
	{ SetMeshOpacity, M1, 	.t0 = T0, .t1 = T0+2, .p0 = {0.5,0,0}, .p1 = {0,0,0} },
	{ ScaleMesh, M1, EaseOutCubic, .t0 = T0, .t1 = T0+2, .p0 = {S0,S0,S0}, .p1 = {S1,S1,S1} },
#undef T0
#define T0 (59.5)
	{ ShowMesh, M2, 		.t0 = T0, .t1 = 73.6, .p0 = {200,120,120}, .p1 = {0,0,0} },
	{ SetMeshOpacity, M2, 	.t0 = T0, .t1 = T0+2, .p0 = {0.5,0,0}, .p1 = {0,0,0} },
	{ ScaleMesh, M2, EaseOutCubic, .t0 = T0, .t1 = T0+2, .p0 = {S0,S0,S0}, .p1 = {S1,S1,S1} },
#undef T0

#undef S0
#undef S1
#undef M1
#undef M2
	// End Fireworks
	
	// UFO
#define M1 UFO_Mesh
#define T0 (59.0)
	{ ShowMesh, M1, 		.t0 = T0, .t1 = 73.6, .p0 = {600,200,200}, .p1 = {0,0,30} },
	{ ScaleMesh, M1, EaseBezier, .t0 = T0, .t1 = T0, .p0 = {100,100,100}, .p1 = {100,100,100} },
	{ MoveMesh, M1, EaseBezier, .t0 = T0, .t1 = T0+4, .p0 = {600,200,200}, .p1 = {-200,200,100} },
	{ RotateMesh, M1, EaseBezier, .t0 = T0, .t1 = T0+4, .p0 = {0,0,30}, .p1 = {0,0,-30} },
	{ MoveMesh, M1, EaseBezier, .t0 = T0+4, .t1 = T0+6, .p0 = {-200,200,100}, .p1 = {-200,125,100} },
	{ RotateMesh, M1, EaseBezier, .t0 = T0+4, .t1 = T0+6, .p0 = {0,0,-30}, .p1 = {0,0,0} },
	{ MoveMesh, M1, EaseBezier, .t0 = T0+6, .t1 = T0+8, .p0 = {-200,125,100}, .p1 = {-100,225,25} },
	{ RotateMesh, M1, EaseBezier, .t0 = T0+6, .t1 = T0+8, .p0 = {0,0,0}, .p1 = {-60,0,0} },
	{ MoveMesh, M1, EaseBezier, .t0 = T0+8, .t1 = T0+10, .p0 = {-100,225,25}, .p1 = {0,25,4} },
	{ RotateMesh, M1, EaseBezier, .t0 = T0+8, .t1 = T0+10, .p0 = {-60,0,0}, .p1 = {0,0,0} },
	{ MoveMesh, M1, EaseBezier, .t0 = T0+11, .t1 = 73.6, .p0 = {0,25,4}, .p1 = {0,0,-200} },
	{ RotateMesh, M1, EaseBezier, .t0 = T0+11, .t1 = 73.6, .p0 = {0,0,0}, .p1 = {-90,0,0} },
	{ ScaleMesh, M1, EaseBezier, .t0 = T0+8, .t1 = 73.6, .p0 = {100,100,100}, .p1 = {200,200,200} },

#undef T0
#undef M1
	// End UFO
	
	// -- 2D; Verse --
	// SONG_DURATION
	
	// Restore part of previous verse scene
#define T0 (74.1)
#define T1 (T0+1)
#define T_END (SONG_DURATION)
	{ ShowShape, Studio_Bkgnd_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,15,0}, .p1 = {0,0,0} },
	{ ShowShape, Moon_Shape, .t0 = T0, .t1 = T_END, .p0 = {0,18,0}, .p1 = {0,0,0} },
	{ ShowShape, Heart_Shape, .t0 = T0, .t1 = T_END, .p0 = {-50,20,0}, .p1 = {0,0,0} },
	{ ShowShape, Star_Shape, .t0 = T0, .t1 = T_END, .p0 = {50,20,0}, .p1 = {0,0,0} },
	// Fade In
	{ SetShapeOpacity, Studio_Bkgnd_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ SetShapeOpacity, Moon_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ SetShapeOpacity, Heart_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {0,0,0}, .p1 = {1,0,0} },
	{ SetShapeOpacity, Star_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {0,0,0}, .p1 = {1,0,0} },

#undef T0
#undef T1

	// Drop a 3D ball
#define T0 (75.1)
	{ ShowMesh, Ball_Mesh, .t0 = T0, .t1 = T_END, .p0 = {0,400,-100}, .p1 = {0,0,0} },
	{ MoveMesh, Ball_Mesh, EaseOutBounce, .t0 = T0, .t1 = T0+4, .p0 = {0,400,-100}, .p1 = {0,50,-150} },
#undef T0

	// Drop traffic cone 1
#define T0 (77.1)
	{ ShowMesh, Cone_1_Mesh, .t0 = T0, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ MoveMesh, Cone_1_Mesh, EaseOutBounce, .t0 = T0, .t1 = T0+4, .p0 = {-300,400,150}, .p1 = {-300,-200,150} },
#undef T0

	// Drop traffic cone 2
#define T0 (78.1)
	{ ShowMesh, Cone_2_Mesh, .t0 = T0, .t1 = T_END, .p0 = {0,0,0}, .p1 = {0,0,0} },
	{ MoveMesh, Cone_2_Mesh, EaseOutBounce, .t0 = T0, .t1 = T0+4, .p0 = {300,400,150}, .p1 = {300,-200,150} },
#undef T0

	// Fade Out
#define T0 (SONG_DURATION - 1.0)
#define T1 (SONG_DURATION)
	{ SetShapeOpacity, Studio_Bkgnd_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {1,0,0}, .p1 = {1,0,0} },
	{ SetShapeOpacity, Moon_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {1,0,0}, .p1 = {0,0,0} },
	{ SetShapeOpacity, Heart_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {1,0,0}, .p1 = {0,0,0} },
	{ SetShapeOpacity, Star_Shape, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {1,0,0}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Ball_Mesh, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {1,0,0}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Cone_1_Mesh, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {1,0,0}, .p1 = {0,0,0} },
	{ SetMeshOpacity, Cone_2_Mesh, EaseBezier, .t0 = T0, .t1 = T1, .p0 = {1,0,0}, .p1 = {0,0,0} },

#undef T0
#undef T1
#undef T_END

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
	mesh_t *m;
	
	// Numerals
	add_numeral_mesh(scene, '1');  // Numeral_1_Mesh
	add_numeral_mesh(scene, '2');  // Numeral_2_Mesh
	add_numeral_mesh(scene, '4');  // Numeral_3_Mesh
	add_numeral_mesh(scene, '2');  // Numeral_4_Mesh
	
	// Grid_Mesh
	m = mesh_create_grid(16);
	m->line_color = rgb_to_abgr(COLOR_RGB_RED_2);
	array_list_add(scene->meshes, m);
	
	// Mtn_#_Mesh
	for (int i=0; i<5; i++) {
		m = mesh_create_pyramid();
		m->line_color = rgb_to_abgr(COLOR_RGB_RED_3);
		array_list_add(scene->meshes, m);
	}
	
	// Radio_Tower_Mesh
	m = mesh_create_pyramid();
	m->line_color = rgb_to_abgr(COLOR_RGB_GREEN_1);
	array_list_add(scene->meshes, m);

	// Fireworks_#_Mesh
	for (int i=0; i<2; i++) {
		m = mesh_create_sphere(3);
		m->use_backface_culling = false;
		m->point_color = rgba_to_abgr(COLOR_RGB_GREEN_2, 127);
		m->line_color = 0;
		mesh_set_angular_momentum_degrees(m, vec3_make(0.1f, 0.5f, 0));
		array_list_add(scene->meshes, m);
	}
	
	// UFO_Mesh
	array_list_add(scene->meshes, mesh_create_ufo());

	// Ball_Mesh
	m = mesh_create_sphere(3);
	m->line_color = rgb_to_abgr(COLOR_RGB_WHITE);
	mesh_set_angular_momentum_degrees(m, vec3_make(0, -0.25, 0));
	array_list_add(scene->meshes, m);
	
	// Cone_1_Mesh: nested inside another mesh for rotations
	m = mesh_new(0);
	if (m) {
		mesh_set_angular_momentum_degrees(m, vec3_make(0, 0.5f, 0));
		mesh_t *cone = mesh_create_traffic_cone();
		if (cone) {
			mesh_set_rotation_degrees(cone, vec3_make(15, 15, 0));
			mesh_add_child(m, cone);
		}
		array_list_add(scene->meshes, m);
	}
	
	// Cone_2_Mesh
	m = mesh_new(0);
	if (m) {
		mesh_set_angular_momentum_degrees(m, vec3_make(0, -0.75f, 0));
		mesh_t *cone = mesh_create_traffic_cone();
		if (cone) {
			mesh_set_rotation_degrees(cone, vec3_make(5, -30, 0));
			mesh_add_child(m, cone);
		}
		array_list_add(scene->meshes, m);
	}

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
	array_list_add(scene->shapes, create_monitor_shape());

	// Smoke_1_Shape, Smoke_2_Shape, Smoke_3_Shape,
	array_list_add(scene->shapes, create_smoke_circle_shape());
	array_list_add(scene->shapes, create_smoke_circle_shape());
	array_list_add(scene->shapes, create_smoke_circle_shape());

	// CPU_Shape,
	array_list_add(scene->shapes, create_cpu_shape());

	// Microphone_Shape,
	array_list_add(scene->shapes, create_microphone_with_stand_shape());

	// Smoke_4_Shape
	array_list_add(scene->shapes, create_smoke_circle_shape());

	// Bolt_1_Shape, Bolt_2_Shape
	array_list_add(scene->shapes, create_lighting_bolt_shape());
	s = create_lighting_bolt_shape();
	s->line_color = rgb_to_abgr(COLOR_RGB_OUTLINE);
	array_list_add(scene->shapes, s);

	// Explosion_Shape,
	s = create_polygon_shape(32, 1.0);
	s->line_color = 0;
	s->fill_color = COLOR_ABGR_WHITE;
	array_list_add(scene->shapes, s);

}

void sequencer_start(gameplay_t *scene) {
	// Start sequence by setting background to black
	scene->bg_color = COLOR_ABGR_BLACK;

	// Hide all objects in scene; reset opacity
	shape_t **s = (shape_t **)scene->shapes->array;
	int sn = scene->shapes->length;
	for (int i=0; i<sn; i++) {
		s[i]->is_visible = false;
		s[i]->opacity = 1.0f;
	}
	mesh_t **m = (mesh_t **)scene->meshes->array;
	int mn = scene->meshes->length;
	for (int i=0; i<mn; i++) {
		m[i]->is_visible = false;
		m[i]->opacity = 1.0f;
	}

}

void sequencer_update_bgcolor(gameplay_t *scene, double current_time) {
	if (current_time <= 0.0) {
		scene->bg_color = rgb_to_abgr(bg_color_timeline[0].rgb);
	} else {
		int i = 1;
		while (bg_color_timeline[i].t < current_time) {
			i++;
		}
		double t0 = bg_color_timeline[i - 1].t;
		double t1 = bg_color_timeline[i].t;
		double x = round((current_time - t0) / (t1 - t0) * 255);
		uint32_t c0 = rgb_to_abgr(bg_color_timeline[i - 1].rgb);
		uint32_t c1 = rgba_to_abgr(bg_color_timeline[i].rgb, (uint8_t)x);
		scene->bg_color = blend_color(c0, c1);
	}
}

float apply_easing_curve(easing_curve ease, float x) {
	// https://easings.net/
	const float c4 = 2 * (float)M_PI / 3;
	const float v = 6.0f;
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	switch (ease) {
		case EaseInQuad:
			x = x * x;
			break;
		case EaseOutQuad:
			x = (1.0f - x);
			x = 1.0f - x * x;
			break;
		case EaseInCubic:
			x = x * x * x;
			break;
		case EaseOutCubic:
			x = (1.0f - x);
			x = 1.0f - x * x * x;
			break;
		case EaseOutElastic:
			if (x <= 0.0f) {
				x = 0.0f;
			} else if (x >= 1.0f) {
				x = 1.0f;
			} else {
				x = powf(2, -v * x) * sinf((x * v - 0.75f) * c4) + 1;
			}
			break;
		case EaseOutBounce:
			if (x < 1.0f / d1) {
				x = n1 * x * x;
			} else if (x < 2.0f / d1) {
				x -= 1.5f / d1;
				x = n1 * x * x + 0.75f;
			} else if (x < 2.5f / d1) {
				x -= 2.25f / d1;
				x = n1 * x * x + 0.9375f;
			} else {
				x -= 2.625f / d1;
				x = n1 * x * x + 0.984375f;
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

float interpolate_float(float a, float b, float x) {
	return a * (1 - x) + b * x;
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
		case SetShapeOpacity:
		case SetMeshOpacity:
			// Do nothing
			break;
		default:
			fprintf(stderr, "Unknown event command, start!\n");
			break;
	}
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
			shape->rotation = interpolate_float(event->p0.x, event->p1.x, x) * ROTATION_FACTOR;
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
		case SetShapeOpacity:
			shape = scene->shapes->array[event->target];
			shape->opacity = interpolate_float(event->p0.x, event->p1.x, x);
			break;
		case SetMeshOpacity:
			mesh = scene->meshes->array[event->target];
			mesh->opacity = interpolate_float(event->p0.x, event->p1.x, x);
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
		case SetShapeOpacity:
		case SetMeshOpacity:
			// Do nothing
			break;
		default:
			fprintf(stderr, "Unknown event command, at end!\n");
			break;
	}
}

void sequencer_update(gameplay_t *scene, double previous_time, double current_time) {
	sequencer_update_bgcolor(scene, current_time);
	
	// Color cycling for UFO
	int hue = (int)round(current_time * 60.0) % 360;
	uint32_t line_color = color_from_hsv(hue, 1.0, 0.5, 1.0);
	uint32_t point_color = color_from_hsv((hue + 90) % 360, 1.0, 1.0, 0.5);
	mesh_t *ufo = scene->meshes->array[UFO_Mesh];
	mesh_set_children_color(ufo, line_color, point_color);

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
