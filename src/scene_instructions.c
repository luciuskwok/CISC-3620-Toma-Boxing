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


void instructions_init(void) {
	
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

void instructions_update(uint64_t delta_time) {
	
}

void instructions_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(BLACK_COLOR);
	fill_screen();

	// Draw text
	set_fill_color(WHITE_COLOR);

	p.x = scr_w / 2;
	p.y = scr_h * 3 / 8;
	move_to(p);
	atari_draw_centered_text("Instructions", 2);

	p.x = scr_w / 2;
	p.y = scr_h - 32;
	move_to(p);
	atari_draw_centered_text("Press Space to Play", 1);
}

