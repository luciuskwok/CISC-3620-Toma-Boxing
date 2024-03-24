// main.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <SDL.h>
#include <emscripten.h>


// Constants
#define BLACK_COLOR (0xFF000000)
#define WHITE_COLOR (0xFFFFFFFF)
#define FPS (60)
#define FRAME_TARGET_TIME (1000 / FPS)
#define PIXELS_WIDTH (320)
#define PIXELS_HEIGHT (200)
#define PIXELS_SCALE (2)

// Globals for SDL
SDL_Window* sdl_window;
SDL_Rect window_rect;
SDL_Renderer* sdl_renderer;
SDL_Texture* sdl_texture;
uint32_t* pixels;
int pixels_w, pixels_h;
uint64_t frame_index;


#pragma mark - 2D Drawing

void clear_screen(uint32_t color) {
	for (int i = 0; i < pixels_w * pixels_h; i++) {
		pixels[i] = color;
	}
}

#pragma mark - Init & Clean Up

bool initialize_windowing_system(void) {
	//fprintf(stdout, "initialize_windowing_system().\n");

	// Set up SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init() failed!\n");
		return false;
	}

 	// Window & Renderer
 	window_rect.x = window_rect.y = 0;
	window_rect.w = PIXELS_WIDTH * PIXELS_SCALE;
	window_rect.h = PIXELS_HEIGHT * PIXELS_SCALE;
	if (SDL_CreateWindowAndRenderer(window_rect.w, window_rect.h, 0, &sdl_window, &sdl_renderer) != 0) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer() failed!\n");
		return false;
	}

	// Texture
	pixels_w = PIXELS_WIDTH;
	pixels_h = PIXELS_HEIGHT;
	sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING, pixels_w, pixels_h);
		// Using ABGR pixel format is slightly faster (~10%) than using RGBA.
	if (!sdl_texture) {
		fprintf(stderr, "SDL_CreateTexture() failed!\n");
		return false;
	}

	// Allocate frame buffer
	pixels = (uint32_t*)malloc(pixels_w * pixels_h * sizeof(uint32_t));
	if (!pixels) {
		fprintf(stderr, "malloc() failed!\n");
		return false;
	}
	clear_screen(BLACK_COLOR);
	
	// Set up the renderer
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); // Use no interpolation
	// Clear the window
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_RenderPresent(sdl_renderer);

	// Reset the frame index
	frame_index = 0;

	// Debug logging: window * texture size
	fprintf(stdout, "Created window (%dx%d) and texture (%dx%d).\n", window_rect.w, window_rect.h, pixels_w, pixels_h);

	return true;
}

void clean_up_windowing_system(void) {
	free(pixels);
	SDL_DestroyTexture(sdl_texture);
	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

#pragma mark - Game Loop

void process_keyboard_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	// Keyboard interaction
	switch (event.type) {
	case SDL_QUIT: // when 'X' button is pressed in window titlebar
		// Exit program
		//is_running = false;
		fprintf(stdout, "Quit requested.\n");
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				// Exit program
				//is_running = false;
				fprintf(stdout, "Escape key pressed.\n");
				break;
		}
		break;
	}
}

void update_state(void) {
	// TODO: add sequencing of animations
	
	// Update frame index
	frame_index++;
}

void run_render_pipeline(void) {
	// Clear frame buffer
	clear_screen(BLACK_COLOR);
	
	// Render frame buffer
	SDL_UpdateTexture(sdl_texture, NULL, pixels, pixels_w * sizeof(uint32_t));
	SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, &window_rect);
	SDL_RenderPresent(sdl_renderer);
}

void game_loop() {
	// Run one iteration of game loop
	process_keyboard_input();
	update_state();
	run_render_pipeline();
}

int main(int argc, const char * argv[]) {
	if (!initialize_windowing_system()) return 0;

	emscripten_set_main_loop(game_loop, 0, 1);

	return 0;
}
