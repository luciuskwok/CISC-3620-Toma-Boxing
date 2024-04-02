//
//  scene_title.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_title.h"
#include "atari_text.h"
#include "color.h"
#include "drawing.h"
#include "image.h"
#include "scene_manager.h"


// Globals
image_t *title_image = NULL;


void title_init(void) {
	title_image = load_bmp_image("assets/title.bmp");
}

bool title_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// Go to instructions scene
				set_scene_index(SCENE_INSTRUCTIONS);
				return true;
		}
	}
	
	return false;
}

void title_update(double delta_time) {
	
}

void title_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(COLOR_BLACK);
	fill_screen();

	// Draw image
	p.x = scr_w/2 - title_image->w/2;
	p.y = scr_h/2 - title_image->h/2;
	move_to(p);
	draw_image(title_image);

	// Draw text
	set_fill_color(COLOR_WHITE);
	p.x = scr_w / 2;
	p.y = scr_h - 32;
	move_to(p);
	atari_draw_centered_text("Press Space to Start", 2);
	
	// Test font
	// set_fill_color(0xFFCCFFCC);
	// atari_draw_test_text();
}

