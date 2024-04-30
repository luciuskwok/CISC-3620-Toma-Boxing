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
#include "sequencer.h"
#include "ui_progress_bar.h"


// Globals
double time_remaining = -1.0;
double last_music_position = 0.0;

// Scene objects and parameters
gameplay_t *gameplay_scene_data = NULL;


void gameplay_init(void) {
	gameplay_scene_data = malloc(sizeof(gameplay_t));
	if (!gameplay_scene_data) {
		fprintf(stderr, "Could not allocate memory for g_gameplay!\n");
		return;
	}
	gameplay_scene_data->bg_color = COLOR_ABGR_BLACK;

	// Allocate memory to hold all the meshes and shapes that will be used in this scene.
	gameplay_scene_data->shapes = array_list_new(256);
	if (!gameplay_scene_data->shapes) {
		fprintf(stderr, "Could not allocate memory for shapes!\n");
		return;
	}
	gameplay_scene_data->meshes = array_list_new(256);
	if (!gameplay_scene_data->meshes) {
		fprintf(stderr, "Could not allocate memory for meshes!\n");
		return;
	}
	
	sequencer_init(gameplay_scene_data);
}

void gameplay_start(void) {
	time_remaining = -1.0;
	last_music_position = -1.0;
	scene_lifetime = 0.0;
	
	// Add shapes and meshes to list of objects in scene
	shape_t **s = (shape_t **)gameplay_scene_data->shapes->array;
	int sn = gameplay_scene_data->shapes->length;
	for (int i=0; i<sn; i++) {
		scene_add_shape(s[i]);
	}
	mesh_t **m = (mesh_t **)gameplay_scene_data->meshes->array;
	int mn = gameplay_scene_data->meshes->length;
	for (int i=0; i<mn; i++) {
		scene_add_mesh(m[i]);
	}
	
	// Restart sequencer
	sequencer_start(gameplay_scene_data);

	// Start playing music
	start_music();
	is_scene_paused = false;
}

bool gameplay_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_a:
				// Punch left
				return true;
			case SDLK_d:
				// Punch right
				return true;
			case SDLK_ESCAPE:
			case SDLK_p:
				// Pause game
				is_scene_paused = !is_scene_paused;
				pause_music(is_scene_paused);
				return true;
			case SDLK_q:
				// Quit to title if paused
				if (is_scene_paused) set_scene_index(SCENE_TITLE);
				return true;
			case SDLK_r:
				// Restart song if paused
				if (is_scene_paused) gameplay_start();
				return true;
			case SDLK_l:
				// Skip to results scene if paused
				if (is_scene_paused) set_scene_index(SCENE_RESULTS);
				return true;
		}
	}
	
	return false;
}

void gameplay_update(double delta_time) {
	// Do not update if paused
	if (is_scene_paused) return;

	// Update song progress bar
	double fraction = 0.0;
	time_remaining = -1.0;
	if (is_music_playing()) {
		double total = get_music_duration();
		if (total > 0.0) {
			double position = get_music_position();
			time_remaining = total - position;
			fraction = position / total;
			sequencer_update(gameplay_scene_data, last_music_position, position);
			last_music_position = position;
		}
	} else {
		// Advance to results scene if song has played through
		if (last_music_position > 0.0) {
			set_scene_index(SCENE_RESULTS);
		}
	}
	set_progress_value(fraction);
}

void gameplay_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color_abgr(gameplay_scene_data->bg_color);
	fill_screen();
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();

	// Draw song progress bar
	draw_progress_bar();
	
	// Draw remaining time text
	uint8_t br = color_brightness(gameplay_scene_data->bg_color);
	uint32_t text_color = (br > 127 || scene_lifetime < 1.0)? COLOR_RGB_BLACK : COLOR_RGB_WHITE;
	set_fill_color_rgba(text_color, 127);
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
	p.x = scr_w / 2 - 21 * 4;
	p.y = 12;
	move_to(p);
	set_fill_color_rgba(text_color, 127);
	atari_draw_text("Toma - All Night Radio", 1);

//	p.x = scr_w - 9 * 8 - 4;
//	p.y = scr_h - 10;
//	move_to(p);
//	set_fill_color_rgba(COLOR_RGB_RED, 127);
//	atari_draw_text("A", 1);
//	set_fill_color_rgba(COLOR_RGB_BLACK, 127);
//	atari_draw_text("/", 1);
//	set_fill_color_rgba(COLOR_RGB_RED, 127);
//	atari_draw_text("D", 1);
//	set_fill_color_rgba(COLOR_RGB_BLACK, 127);
//	atari_draw_text(":Punch", 1);
	
	// Draw pause menu
	if (is_scene_paused) {
		
		rectangle_t text_rect;
		text_rect.w = 15 * 8;
		text_rect.h = 9 * 10;
		text_rect.x = scr_w/2 - 15 * 4;
		text_rect.y = scr_h/2 - 4 * 9;
		vec2_t text_origin = vec2_make(text_rect.x, text_rect.y);
		text_rect = inset_rect(text_rect, -4, -4);
		
		// Draw box
		set_fill_color_rgba(COLOR_RGB_BLACK, 192);
		fill_rect(text_rect);
		set_line_color_rgba(COLOR_RGB_WHITE, 192);
		stroke_rect(text_rect);
		
		// Draw text
		move_to(text_origin);
		set_fill_color_rgba(COLOR_RGB_WHITE, 255);
		draw_key_text_line(NULL, "Pause Menu");
		set_key_text_color(rgba_to_abgr(COLOR_RGB_RED, 255));
		set_fill_color_rgba(COLOR_RGB_GRAY_80, 255);
		draw_key_text_line("Esc", "unpause");
		draw_key_text_line("R", "restart song");
		draw_key_text_line("Q", "quit to title");
		draw_key_text_line(NULL, NULL);
		set_fill_color_rgba(COLOR_RGB_WHITE, 255);
		draw_key_text_line(NULL, "Music");
		set_fill_color_rgba(COLOR_RGB_GRAY_80, 255);
		draw_key_text_line("<", "softer");
		draw_key_text_line(">", "louder");
		draw_key_text_line("M", "mute");
	}
}
