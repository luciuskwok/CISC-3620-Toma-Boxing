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

#include "audio_player.h"
#include "color.h"
#include "drawing.h"
#include "matrix.h"
#include "cube_mesh.h"
#include "vector.h"

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
			case SDLK_0:
				cube_reset_transform();
				break;
			case SDLK_e:
				// Roll right
				cube_add_roll(1.0f);
				break;
			case SDLK_q:
				// Roll left
				cube_add_roll(-1.0f);
				break;
			case SDLK_w:
				// Pitch down
				cube_add_pitch(1.0f);
				break;
			case SDLK_s:
				// Pitch up
				cube_add_pitch(-1.0f);
				break;
			case SDLK_a:
				// Yaw left
				cube_add_yaw(1.0f);
				break;
			case SDLK_d:
				// Yaw right
				cube_add_yaw(-1.0f);
				break;
			case SDLK_SPACE:
				// Stop movement
				cube_reset_momentum();
				break;
			case SDLK_p:
				// Start audio player
				start_audio();
				break;
			case SDLK_o:
				// Pause/Unpause audio player
				pause_audio(!is_audio_paused());
				break;
			case SDLK_i:
				// Stop audio player
				stop_audio();
				break;
		}
		break;
	}
}

void update_state(void) {
	update_cube(frame_index);
	
	// Update frame index
	frame_index++;
}

void run_render_pipeline(void) {
	set_fill_color(BLACK_COLOR);
	fill_screen();
	
	render_cube();
	render_to_screen();
}

void run_game_loop(void) {
	// Run one iteration of game loop
	process_keyboard_input();
	update_state();
	run_render_pipeline();
}

int main(int argc, const char * argv[]) {
	if (!init_window(PIXELS_WIDTH, PIXELS_HEIGHT, PIXELS_SCALE)) return 0;
	if (!init_audio()) return 0;
	
	init_projection();
	init_cube();

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
	destroy_window();
#endif

	return 0;
}
