//
//  image_convert.c
//  toma-boxing-tools
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "image_convert.h"
#include "file_io.h"

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

// Constants
#define FILE_SIGNATURE (0x494E4458) /* Spells out 'INDX' */
#define MAX_PALETTE_LEN (256)


// Convert PNG to indexed image that is printed to stdout

uint32_t swap_red_blue(uint32_t x) {
	uint32_t r = (x & 0x00FF0000) >> 16;
	uint32_t b = (x & 0x000000FF);
	return (x & 0xFF00FF00) | (b << 16) | (r);
}

int color_exists_in_palette(uint32_t color, uint32_t *palette, int palette_len) {
	for (int i = 0; i < palette_len; i++) {
		if (palette[i] == color) return i;
	}
	return -1;
}

int convert_rgb_to_indexed(SDL_Surface *surface, uint8_t *indexes, uint32_t *palette) {
	// Returns number of palette entries
	int palette_len = 0;
	int w = surface->w;
	int h = surface->h;
	int pitch = surface->pitch / sizeof(uint32_t);
	uint32_t *pixels = surface->pixels;
	uint32_t fmt = surface->format->format;
	uint32_t c;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			// Get color
			c = pixels[x + y * pitch];
			if (fmt == SDL_PIXELFORMAT_ARGB8888) {
				c = swap_red_blue(c);
			}
			
			// Find matching index
			int color_index = color_exists_in_palette(c, palette, palette_len);
			if (color_index < 0) {
				if (palette_len >= MAX_PALETTE_LEN) {
					fprintf(stderr, "Out of palette slots!\n");
					return -1;
				}
				palette[palette_len] = c;
				color_index = palette_len;
				palette_len++;
			}
			indexes[x + y * w] = color_index;
		}
	}

	return palette_len;
}

void convert_png(const char *input, const char *output) {
	SDL_Surface *surface = IMG_Load(input);
	if (!surface) {
		fprintf(stderr, "IMG_Load() failed: %s\n", SDL_GetError());
		return;
	}

	// Get parameters
	int w = surface->w;
	int h = surface->h;
	uint32_t fmt = surface->format->format;
	
	// Allocate indexes and palette
	uint8_t *indexes = malloc(w * h * sizeof(uint8_t));
	if (!indexes) {
		fprintf(stderr, "Unable to allocate indexes!\n");
		return;
	}
	uint32_t *palette = malloc(MAX_PALETTE_LEN * sizeof(uint32_t));
	if (!palette) {
		fprintf(stderr, "Unable to allocate palette!\n");
		return;
	}
	int palette_len = 0;

	if (fmt == SDL_PIXELFORMAT_ARGB8888 || fmt == SDL_PIXELFORMAT_ABGR8888) {
		// RGB format
		palette_len = convert_rgb_to_indexed(surface, indexes, palette);
	} else if (fmt == SDL_PIXELTYPE_INDEX8) {
		// Indexed format that Aseprite produces
		fprintf(stderr, "Unsupported image surface format: 0x%08x\n", fmt);
		SDL_FreeSurface(surface);
		return;
	} else {
		fprintf(stderr, "Unsupported image surface format: 0x%08x\n", fmt);
		SDL_FreeSurface(surface);
		return;
	}

	SDL_FreeSurface(surface);
	
	// Write out file
	int total_written = 0;
	FILE *out = fopen(output, "wb");
	
	// Header: sig, width, height, and number of palette entries
	total_written += write_uint32(out, FILE_SIGNATURE);
	total_written += write_uint32(out, w);
	total_written += write_uint32(out, h);
	total_written += write_uint32(out, palette_len);
	
	// Color Palette
	total_written += write_buffer(out, palette, palette_len * sizeof(uint32_t));

	// Image data
	total_written += write_buffer(out, indexes, w * h * sizeof(uint8_t));
	
	// Clean up
	fclose(out);
	free(indexes);
	
	printf("File written with %d bytes\n", total_written);
}

