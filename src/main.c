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
#define PIXELS_SCALE (2)



// Globals
bool is_running = true;
uint64_t frame_index = 0;


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
				fprintf(stdout, "Escape key pressed.\n");
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
		}
		break;
	}
}

void update_state(void) {
	switch (get_scene_index()) {
		case SCENE_TITLE:
			title_update(frame_index);
			break;
		case SCENE_INSTRUCTIONS:
			instructions_update(frame_index);
			break;
		case SCENE_GAMEPLAY:
			gameplay_update(frame_index);
			break;
		case SCENE_RESULTS:
			results_update(frame_index);
			break;
	}
	
	// Update frame index
	frame_index++;
}

void render_volume_overlay(void) {
	// Draw volume overlay in bottom left of screen
	vec2_t p, scr;
	scr.x = get_screen_width();
	scr.y = get_screen_height();
	
	// Use 50% opaque black as overlay background
	set_fill_color(0x88000000);
	fill_rect(0, scr.y - 12, 4 * 8 + 4, 12);

	// Draw text
	set_fill_color(0xEEFFFFFF);
	p.x = 2;
	p.y = scr.y - 10;
	move_to(p);
	atari_draw_text("\06\01\02\07", 1);

}

void run_render_pipeline(void) {
	// Render scene
	switch (get_scene_index()) {
		case SCENE_TITLE:
			title_render();
			break;
		case SCENE_INSTRUCTIONS:
			instructions_render();
			break;
		case SCENE_GAMEPLAY:
			gameplay_render();
			break;
		case SCENE_RESULTS:
			results_render();
			break;
	}
	// Render overlays
	render_volume_overlay();
	
	render_to_screen();
}

void run_game_loop(void) {
	// Run one iteration of game loop
	process_keyboard_input();
	update_state();
	run_render_pipeline();
}

int main(int argc, const char * argv[]) {
	if (!init_screen(PIXELS_WIDTH, PIXELS_HEIGHT, PIXELS_SCALE)) return 0;
	if (!init_audio()) return 0;
	
	// Init all scenes
	title_init();
	instructions_init();
	gameplay_init();
	results_init();

	atari_renderer_init();
	init_projection();
	
	set_scene_index(SCENE_TITLE);

	// Instructions
	fprintf(stdout, "Controls:\n"
			"Rotation: Q, W, E, A, S, D\n"
			"Audio: P = start; O = pause; I = stop\n"
			);
	
#ifdef __EMSCRIPTEN__
	// WebAssembly version
	emscripten_set_main_loop(run_game_loop, 0, 1);
	
#else
	// Xcode version
	uint32_t startTime, ticks;
	while (is_running) {
		// Use SDL_GetTicks() to get how long each frame takes
		startTime = SDL_GetTicks();
		
		run_game_loop();
		
		// Delay to get to target frame time
		ticks = SDL_GetTicks() - startTime;
		if (ticks < FRAME_TARGET_TIME) {
			SDL_Delay(FRAME_TARGET_TIME - ticks);
		}
	}
	destroy_screen();
#endif

	return 0;
}
