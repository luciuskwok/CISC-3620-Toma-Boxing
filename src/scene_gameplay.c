//
//  scene_gameplay.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_gameplay.h"
#include "atari_text.h"
#include "audio_player.h"
#include "color.h"
#include "drawing.h"
#include "image.h"
#include "mesh.h"
#include "mesh_creation.h"
#include "scene_manager.h"
#include "ui_progress_bar.h"


// Globals
double time_remaining = -1.0;
uint32_t background_color = COLOR_ABGR_BLACK;


void gameplay_init(void) {
	
}

void gameplay_start(void) {
	
	time_remaining = -1.0;
	
	// Start playing music
	start_music();
}

bool gameplay_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_0:
				return true;
			case SDLK_e:
				return true;
			case SDLK_q:
				return true;
			case SDLK_w:
				return true;
			case SDLK_s:
				return true;
			case SDLK_a:
				return true;
			case SDLK_d:
				return true;
			case SDLK_SPACE:
				return true;
			case SDLK_p:
				return true;
			case SDLK_o:
				return true;
			case SDLK_i:
				return true;
			case SDLK_l:
				// Go to results scene
				set_scene_index(SCENE_RESULTS);
				return true;
		}
	}
	
	return false;
}

void gameplay_update(double delta_time) {
	// Update song progress bar
	double fraction = 0.0;
	time_remaining = -1.0;
	if (is_music_playing()) {
		double total = get_music_duration();
		if (total > 0.0) {
			double position = get_music_position();
			time_remaining = total - position;
			fraction = position / total;
		}
	}
	set_progress_value(fraction);
}

void gameplay_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color_abgr(background_color);
	fill_screen();
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();

	// Draw song progress bar
	draw_progress_bar();
	
	// Draw remaining time text
	set_fill_color_abgr(color_set_alpha(COLOR_ABGR_WHITE, 127));
	if (time_remaining >= 0.0) {
		int seconds = (int)ceil(time_remaining);
		int minutes = seconds / 60;
		seconds = seconds % 60;
		char s[16];
		sprintf(s, "%d:%02d", minutes, seconds);
		p.x = scr_w - 2;
		p.y = 2;
		move_to(p);
		atari_draw_right_justified_text(s, 1);
	}

	// Draw text
	set_fill_color_abgr(COLOR_ABGR_WHITE);

	p.x = scr_w / 2 - 21 * 4;
	p.y = 12;
	move_to(p);
	set_fill_color_rgba(COLOR_RGB_PINK, 255);
	atari_draw_text("Toma", 1);
	set_fill_color_rgba(COLOR_RGB_LIGHT_GRAY, 255);
	atari_draw_text(" - ", 1);
	set_fill_color_rgba(COLOR_RGB_LIME, 255);
	atari_draw_text("All Night Radio", 1);

	p.x = scr_w - 9 * 8 - 4;
	p.y = scr_h - 10;
	move_to(p);
	set_fill_color_rgba(COLOR_RGB_RED, 255);
	atari_draw_text("A", 1);
	set_fill_color_rgba(COLOR_RGB_WHITE, 255);
	atari_draw_text("/", 1);
	set_fill_color_rgba(COLOR_RGB_RED, 255);
	atari_draw_text("D", 1);
	set_fill_color_rgba(COLOR_RGB_WHITE, 255);
	atari_draw_text(":Punch", 1);
}
