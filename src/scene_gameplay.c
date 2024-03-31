//
//  scene_gameplay.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_gameplay.h"
#include "atari_text.h"
#include "color.h"
#include "drawing.h"
#include "image.h"
#include "cube_mesh.h"
#include "audio_player.h"
#include "scene_manager.h"


void gameplay_init(void) {
	init_cube();
}

bool gameplay_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_0:
				cube_reset_transform();
				return true;
			case SDLK_e:
				// Roll right
				cube_add_roll(1.0f);
				return true;
			case SDLK_q:
				// Roll left
				cube_add_roll(-1.0f);
				return true;
			case SDLK_w:
				// Pitch down
				cube_add_pitch(1.0f);
				return true;
			case SDLK_s:
				// Pitch up
				cube_add_pitch(-1.0f);
				return true;
			case SDLK_a:
				// Yaw left
				cube_add_yaw(1.0f);
				return true;
			case SDLK_d:
				// Yaw right
				cube_add_yaw(-1.0f);
				return true;
			case SDLK_SPACE:
				// Stop movement
				cube_reset_momentum();
				return true;
			case SDLK_p:
				// Start audio player
				start_music();
				return true;
			case SDLK_o:
				// Pause/Unpause audio player
				pause_music(!is_music_paused());
				return true;
			case SDLK_i:
				// Stop audio player
				stop_music();
				return true;
			case SDLK_l:
				// Go to results scene
				set_scene_index(SCENE_RESULTS);
				return true;
		}
	}
	
	return false;
}

void gameplay_update(uint64_t delta_time) {
	update_cube(delta_time);

}

void gameplay_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(BLACK_COLOR);
	fill_screen();

	// Draw cube
	draw_cube();
	
	// Draw text
	set_fill_color(WHITE_COLOR);

	p.x = scr_w / 2;
	p.y = 8;
	move_to(p);
	atari_draw_centered_text("Cube", 2);

	p.x = scr_w / 2;
	p.y = scr_h - 32;
	move_to(p);
	atari_draw_centered_text("Controls: QWE/ASD/IOP/L", 1);
}
