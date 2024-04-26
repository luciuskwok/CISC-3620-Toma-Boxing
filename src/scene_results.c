//
//  scene_results.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_results.h"
#include "atari_text.h"
#include "color.h"
#include "drawing.h"
#include "image.h"
#include "scene_manager.h"


void results_init(void) {
	
}

void results_start(void) {
	
}

bool results_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// Go to gameplay scene
				set_scene_index(SCENE_TITLE);
				return true;
		}
	}
	
	return false;
}

void results_update(double delta_time) {
	
}

void results_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();
	
	set_fill_color_abgr(COLOR_ABGR_BLACK);
	fill_screen();
	
	// Draw text
	set_fill_color_abgr(COLOR_ABGR_WHITE);
	
	const int left_margin = scr_w / 2 + 16;
	const int top_margin = 24;
	const int title_spacing = 32;
	p.x = left_margin;
	p.y = top_margin;
	move_to(p);
	atari_draw_text("Results", 2);
	
	// Left column: labels
	const int line_height = 12;
	set_fill_color_rgba(COLOR_RGB_PINK, 255);
	p.y = top_margin + title_spacing;
	move_to(p);
	atari_draw_text("Great", 1);
	p.y += line_height;
	move_to(p);
	atari_draw_text("OK", 1);
	p.y += line_height;
	move_to(p);
	atari_draw_text("Bad", 1);
	p.y += line_height;
	move_to(p);
	atari_draw_text("Miss", 1);
	p.y += line_height * 2;
	move_to(p);
	atari_draw_text("Combo", 1);
	p.y += line_height * 2;
	move_to(p);
	atari_draw_text("Score", 1);
	
	// Right column: alues
	p.x = left_margin + 14 * 8;
	p.y = top_margin + title_spacing;
	set_fill_color_rgba(COLOR_RGB_LIME, 255);
	move_to(p);
	atari_draw_right_justified_text("123", 1);
	p.y += line_height;
	move_to(p);
	atari_draw_right_justified_text("54", 1);
	p.y += line_height;
	move_to(p);
	atari_draw_right_justified_text("2", 1);
	p.y += line_height;
	move_to(p);
	atari_draw_right_justified_text("5", 1);
	p.y += line_height * 2;
	move_to(p);
	atari_draw_right_justified_text("36", 1);
	p.y += line_height * 2;
	move_to(p);
	atari_draw_right_justified_text("23456", 1);
	
	p.x = scr_w / 2;
	p.y = scr_h - 16;
	move_to(p);
	set_fill_color_abgr(COLOR_ABGR_WHITE);
	atari_draw_centered_text("Press Space to Return to Title", 1);
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();
}
