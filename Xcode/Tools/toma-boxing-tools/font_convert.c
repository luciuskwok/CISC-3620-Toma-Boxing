//
//  font_convert.c
//  toma-boxing-tools
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "font_convert.h"
#include "atari_font_data.h"
#include "file_io.h"

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>


void atari_draw_char(SDL_Surface *surface, uint8_t *font, int font_len, char c, int x, int y) {
	const int num_chars = font_len / 8;
	uint32_t *pixels = surface->pixels;
	const int pitch = surface->pitch / sizeof(uint32_t);

	c = c % num_chars;
	for (int y1 = 0; y1 < 8; y1++) {
		uint8_t acc = font[c * 8 + y1];
		for (int x1 = 7; x1 >= 0; x1--) {
			int color = (acc & 1)? 0xFF000000 : 0xFFFFFFFF;
			pixels[(x + x1) + (y + y1) * pitch] = color;
			acc = acc >> 1;
		}
	}
}

void write_font_to_png(uint8_t *font, int font_len, const char *output) {
	const int n = font_len / 8;
	const int w = 16 * 8;
	const int h = ((n + 15) / 16) * 8;
	SDL_Surface *surface =SDL_CreateRGBSurfaceWithFormat (0, w, h, 32, SDL_PIXELFORMAT_ABGR8888);
	if (!surface) {
		fprintf(stderr, "SDL_CreateRGBSurfaceWithFormat() failed: %s\n", SDL_GetError());
		return;
	}
	
	for (int i = 0; i < n; i++) {
		int x = (i % 16) * 8;
		int y = (i / 16) * 8;
		atari_draw_char(surface, font, font_len, i, x, y);
	}
	
	if (IMG_SavePNG(surface, output) != 0) {
		fprintf(stderr, "IMG_SavePNG() failed: %s\n", SDL_GetError());
	}
	
	SDL_FreeSurface(surface);
	
	fprintf(stdout, "Saved PNG to %s\n", output);
}

void write_atari_font_to_png(const char *output) {
	write_font_to_png(atari_font_data, atari_font_data_len, output);
}

uint8_t scan_byte_from_surface(SDL_Surface *surface, int x, int y) {
	// Treat white pixels as 0, otherwise 1
	int pitch = surface->pitch / 4;
	uint32_t *pixels = (uint32_t *)(surface->pixels);
	uint8_t c = 0;
	
	for (int i = 0; i < 8; i++) {
		c = c << 1;
		if (pixels[x + i + y * pitch] != 0xFFFFFFFF) {
			c = c | 0x01;
		}
	}
	
	return c;
}

void convert_png_to_font_data(const char *input, const char *output) {
	SDL_Surface *surface = IMG_Load(input);
	if (!surface) {
		fprintf(stderr, "IMG_Load() failed: %s\n", SDL_GetError());
		return;
	}

	uint32_t fmt = surface->format->format;
	if (fmt != SDL_PIXELFORMAT_ARGB8888 && fmt != SDL_PIXELFORMAT_ABGR8888) {
		fprintf(stderr, "Unsupported image surface format: 0x%08x\n", fmt);
		SDL_FreeSurface(surface);
		return;
	}
	
	// Open output file
	int total_written = 0;
	FILE *out = fopen(output, "wb");
	
	// Get parameters
	int columns = surface->w / 8;
	int rows = surface->h / 8;
	uint8_t b;
	
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < columns; col++) {
			int x = col * 8;
			for (int y = 0; y < 8; y++) {
				b = scan_byte_from_surface(surface, x, y + row * 8);
				total_written += write_uint8(out, b);
			}
		}
	}
	
	// Clean up
	fclose(out);
	SDL_FreeSurface(surface);
	
	fprintf(stdout, "Saved %d bytes of font data to %s\n", total_written, output);
}
