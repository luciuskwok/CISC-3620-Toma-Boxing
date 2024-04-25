//
//  main.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/21/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "atari_text.h"
#include "audio_player.h"
#include "color.h"
#include "drawing.h"
#include "matrix.h"
#include "vector.h"

#include "scene_manager.h"

#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Constants
#define FRAME_TARGET_TIME (1000 / 60)
#define PIXELS_WIDTH (320)
#define PIXELS_HEIGHT (200)
#define PIXELS_SCALE (3)


// Globals
bool is_running = true;
uint64_t last_update_time = 0;


#pragma mark - Audio Volume

void adjust_music_volume(int delta) {
	int vol = get_music_volume();
	set_music_volume(vol + delta);
}


#pragma mark - Game Loop

void process_keyboard_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);
	bool handled = false;
	
	switch (get_scene_index()) {
		case SCENE_TITLE:
			handled = title_handle_keyboard(event);
			break;
		case SCENE_INSTRUCTIONS:
			handled = instructions_handle_keyboard(event);
			break;
		case SCENE_GAMEPLAY:
			handled = gameplay_handle_keyboard(event);
			break;
		case SCENE_RESULTS:
			handled = results_handle_keyboard(event);
			break;
		case SCENE_STARTUP:
			break;
	}
	if (handled) return;

	// Keyboard interaction
	switch (event.type) {
	case SDL_QUIT: // when 'X' button is pressed in window titlebar
		// Exit program
		is_running = false;
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				// Exit program
				is_running = false;
				break;
			case SDLK_p:
				// Start audio player
				start_music();
				break;
			case SDLK_o:
				// Pause/Unpause audio player
				pause_music(!is_music_paused());
				break;
			case SDLK_i:
				// Stop audio player
				stop_music();
				break;
			case SDLK_m:
				// Mute toggle
				set_music_muted(!is_music_muted());
				break;
			case SDLK_COMMA:
				// Decrease volume
				adjust_music_volume(-1);
				break;
			case SDLK_PERIOD:
				// Increase volume
				adjust_music_volume(1);
				break;
		}
		break;
	}
}

void update_state(uint64_t delta_time) {
	double x = (double)delta_time / 1000.0;
	update_scene(x);
}

void draw_volume_overlay(void) {
	// Draw volume overlay in bottom left of screen
	vec2_t p;
	// int scr_w = get_screen_width();
	int scr_h = get_screen_height();
	
	// Draw text
	set_fill_color(rgb_to_abgr(COLOR_RGB_DARK_GRAY));
	p.x = 2;
	p.y = scr_h - 10;
	move_to(p);
	
	char s[4];
	if (is_music_muted()) {
		sprintf(s, "\x02");
	} else {
		sprintf(s, "\x01%d", get_music_volume());
	}
	atari_draw_text(s, 1);
}

void run_render_pipeline(void) {
	// Draw scene
	draw_scene();

	// Draw overlays
	draw_volume_overlay();
	
	render_to_screen();
}

void run_game_loop(void) {
	// Run one iteration of game loop
	uint64_t update_start_time = SDL_GetTicks64();
	uint64_t delta_time = update_start_time - last_update_time;
	
	// Run one iteration of game loop
	process_keyboard_input();
	if (!is_running) return;
	if (delta_time > 0) {
		update_state(delta_time);
	}
	run_render_pipeline();
	
#ifndef __EMSCRIPTEN__
	// Xcode version: delay to cap FPS
	uint64_t update_end_time = SDL_GetTicks64();
	uint64_t elapsed_time = update_end_time - last_update_time;
	if (elapsed_time < FRAME_TARGET_TIME) {
		uint32_t delay_time = (uint32_t)(FRAME_TARGET_TIME - elapsed_time);
		// uint64_t delay_start = SDL_GetTicks64();
		SDL_Delay(delay_time);
		// uint64_t delay_end = SDL_GetTicks64();
//		uint32_t actual_delay_time = (uint32_t)(delay_end - delay_start);
//		if (actual_delay_time > delay_time * 2) {
//			fprintf(stdout, "Delay time: Request=%u, Actual=%ums.\n", delay_time, actual_delay_time);
//		}
	} else if (elapsed_time > 250){
		fprintf(stdout, "Lag frame elapsed time: %llums.\n", elapsed_time);
	}

#endif
	last_update_time = update_start_time;
}

int main(int argc, const char * argv[]) {
	if (!init_screen(PIXELS_WIDTH, PIXELS_HEIGHT, PIXELS_SCALE)) return 0;
	if (!init_audio()) return 0;
	if (!atari_text_init()) return 0;

	// Init all scenes
	init_scene_manager();
	
	set_scene_index(SCENE_TITLE);
	last_update_time = SDL_GetTicks64();

#ifdef __EMSCRIPTEN__
	// WebAssembly version
	emscripten_set_main_loop(run_game_loop, 0, 1);
#else
	// Xcode version
	while (is_running) {
		run_game_loop();
	}
	destroy_screen();
#endif

	return 0;
}
