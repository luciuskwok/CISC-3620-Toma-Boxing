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


// Globals
image_t *instr_track_image = NULL;


void instructions_init(void) {
	instr_track_image = load_bmp_image("assets/track.bmp");
}

void instructions_start(void) {
	
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
	p.x = scr_w / 2;
	p.y = 8;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_centered_text("Instructions", 2);
	
	p.x = 32;
	p.y = 24;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Use ", 1);
	set_fill_color(COLOR_RED);
	atari_draw_text("A ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("& ", 1);
	set_fill_color(COLOR_RED);
	atari_draw_text("D ", 1);
	
	p.x = 32;
	p.y = 32;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("to punch left and right ", 1);

	p.x = scr_w - 2;
	p.y = scr_h - 10;
	move_to(p);
	atari_draw_right_justified_text("Press Space to Play", 1);
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();
}

