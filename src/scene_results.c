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

void results_update(uint64_t delta_time) {
	
}

void results_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(COLOR_BLACK);
	fill_screen();

	// Draw text
	set_fill_color(COLOR_WHITE);

	p.x = scr_w / 2;
	p.y = 8;
	move_to(p);
	atari_draw_centered_text("Results", 2);

	p.x = scr_w / 2;
	p.y = scr_h - 12;
	move_to(p);
	atari_draw_centered_text("Press Space to Return to Title", 1);
}
