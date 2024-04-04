//
//  scene_instructions.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_instructions.h"
#include "atari_text.h"
#include "color.h"
#include "drawing.h"
#include "image.h"
#include "scene_manager.h"

#include "mesh.h"
#include "mesh_cube.h"


// Globals
image_t *instr_track_image = NULL;
mesh_t *instr_cube = NULL;


void instructions_init(void) {
	instr_track_image = load_bmp_image("assets/track.bmp");
	instr_cube = create_mesh_cube();
}

void instructions_start(void) {
	add_mesh(instr_cube);
	
	// Have cube spin by itself
	mesh_reset_transform(instr_cube);
	mesh_reset_momentum(instr_cube);
	mesh_add_yaw(instr_cube, 5.0f);
	mesh_add_pitch(instr_cube, 1.0f);
}

bool instructions_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// Go to gameplay scene
				set_scene_index(SCENE_GAMEPLAY);
				return true;
		}
	}
	
	return false;
}

void instructions_update(double delta_time) {
	double t = get_scene_lifetime();
	
	// Color cycling
	int hue = (int)round(t * 60.0) % 360;
	uint32_t line_color = color_from_hsv(hue, 1.0, 1.0, 1.0);
	uint32_t point_color = color_from_hsv((hue + 60) % 360, 1.0, 1.0, 0.5);
	
	instr_cube->line_color = line_color;
	instr_cube->point_color = point_color;

}

void instructions_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(COLOR_BLACK);
	fill_screen();

	// Draw 2 tracks
	p.x = scr_w / 4 - instr_track_image->w / 2;
	p.y = scr_h / 2 - instr_track_image->h / 2;
	move_to(p);
	draw_image(instr_track_image);
	p.x = scr_w * 3 / 4 - instr_track_image->w / 2;
	move_to(p);
	draw_image(instr_track_image);
	
	// Draw text
	// "Instructions"
	p.x = scr_w / 2;
	p.y = 8;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_centered_text("Instructions", 2);
	
	// Body
	int left_margin = scr_w / 4 + 16;
	p.x = left_margin;
	p.y = scr_h / 2 - 16;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Use ", 1);
	set_fill_color(COLOR_RED);
	atari_draw_text("A ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("& ", 1);
	set_fill_color(COLOR_RED);
	atari_draw_text("D ", 1);
	
	set_fill_color(COLOR_WHITE);
	p.y += 10;
	move_to(p);
	atari_draw_text("to punch", 1);
	p.y += 10;
	move_to(p);
	atari_draw_text("left & right", 1);

	p.x = scr_w - 2;
	p.y = scr_h - 10;
	move_to(p);
	atari_draw_right_justified_text("Press Space to Play", 1);
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();
}

