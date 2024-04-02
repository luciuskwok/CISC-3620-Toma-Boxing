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
#include "mesh_cube.h"
#include "scene_manager.h"
#include "ui_progress_bar.h"


// Globals
image_t *track_image = NULL;
double scene_gameplay_lifetime = 0.0;
mesh_t *gameplay_cube = NULL;


void gameplay_init(void) {
	track_image = load_bmp_image("assets/track.bmp");
	gameplay_cube = create_mesh_cube();
}

void gameplay_start(void) {
	remove_all_meshes();
	add_mesh(gameplay_cube);
}

bool gameplay_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_0:
				mesh_reset_transform(gameplay_cube);
				mesh_reset_momentum((gameplay_cube));
				return true;
			case SDLK_e:
				// Roll right
				mesh_add_roll(gameplay_cube, 1.0f);
				return true;
			case SDLK_q:
				// Roll left
				mesh_add_roll(gameplay_cube, -1.0f);
				return true;
			case SDLK_w:
				// Pitch down
				mesh_add_pitch(gameplay_cube, 1.0f);
				return true;
			case SDLK_s:
				// Pitch up
				mesh_add_pitch(gameplay_cube, -1.0f);
				return true;
			case SDLK_a:
				// Yaw left
				mesh_add_yaw(gameplay_cube, 1.0f);
				return true;
			case SDLK_d:
				// Yaw right
				mesh_add_yaw(gameplay_cube, -1.0f);
				return true;
			case SDLK_SPACE:
				// Stop movement
				mesh_reset_momentum(gameplay_cube);
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

void gameplay_update(double delta_time) {
	
	// Color cycling
	int hue = (int)round(scene_gameplay_lifetime * 60.0) % 360;
	uint32_t line_color = color_from_hsv(hue, 1.0, 1.0, 1.0);
	uint32_t point_color = color_from_hsv((hue + 60) % 360, 1.0, 1.0, 0.5);
	
	gameplay_cube->line_color = line_color;
	gameplay_cube->point_color = point_color;

	// Update song progress bar
	double x = 0.0f;
	if (is_music_playing()) {
		double total = get_music_duration();
		double progress = get_music_position();
		if (total > 0.0) {
			x = progress / total;
		}
	}
	set_progress_value(x);
	
	scene_gameplay_lifetime += delta_time;
}

void gameplay_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(COLOR_BLACK);
	fill_screen();

	// Draw 2 tracks
	p.x = scr_w / 4 - track_image->w / 2;
	p.y = scr_h / 2 - track_image->h / 2;
	move_to(p);
	draw_image(track_image);
	p.x = scr_w * 3 / 4 - track_image->w / 2;
	move_to(p);
	draw_image(track_image);
	
	// Draw cube
	draw_meshes();
	
	// Draw song progress bar
	draw_progress_bar();
	
	// Draw text
	set_fill_color(COLOR_WHITE);

	p.x = scr_w / 2;
	p.y = 12;
	move_to(p);
	atari_draw_centered_text("Cube", 2);

	p.x = scr_w / 2;
	p.y = scr_h - 32;
	move_to(p);
	atari_draw_centered_text("Controls: QWE/ASD/IOP/L", 1);
}
