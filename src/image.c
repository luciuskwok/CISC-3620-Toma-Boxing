//
//  image.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/26/24.
//

#include "image.h"
#include "drawing.h"
#include "file_io.h"
#include "vector.h"

#include <SDL2/SDL.h>
#include <stdio.h>

// Constants
#define FILE_SIGNATURE (0x494E4458) /* Spells out 'INDX' */


image_t *load_indexed_image(const char *file) {
	FILE *in = fopen(file, "rb");
	
	// Header: sig, width, height, and number of palette entries
	uint32_t sig = read_int(in);
	uint32_t w = read_int(in);
	uint32_t h = read_int(in);
	uint32_t palette_len = read_int(in);
	uint32_t n = w * h;
	
	if (sig != FILE_SIGNATURE) {
		fprintf(stderr, "Invalid file!\n");
		return NULL;
	}
	
	// Read color palette
	uint32_t *palette = malloc(palette_len * sizeof(uint32_t));
	if (!palette) {
		fprintf(stderr, "Unable to allocate palette!\n");
		return NULL;
	}
	if (!read_buffer(in, palette, palette_len * sizeof(uint32_t))) {
		fprintf(stderr, "Unable to read palette!\n");
		return NULL;
	}
	
	// Read indexed pixels
	uint8_t *indexes = malloc(n * sizeof(uint8_t));
	if (!indexes) {
		fprintf(stderr, "Unable to allocate indexes!\n");
		return NULL;
	}
	if (!read_buffer(in, indexes, n * sizeof(uint8_t))) {
		fprintf(stderr, "Unable to read indexes!\n");
		return NULL;
	}
	
	// Create image_t
	image_t *img = (image_t *)malloc(sizeof(image_t));
	if (!img) {
		fprintf(stderr, "Unable to allocate image_t.\n");
		return NULL;
	}
	img->w = w;
	img->h = h;
	uint32_t *pixels = (uint32_t *)malloc(n * sizeof(uint32_t));
	if (!pixels) {
		fprintf(stderr, "Unable to allocate memory for pixels.\n");
		return NULL;
	}
	img->pixels = pixels;
	
	// Convert from indexed to ARGB
	for (int i = 0; i < n; i++) {
		int j = indexes[i];
		pixels[i] = palette[j];
	}

	// Clean up
	fclose(in);
	free(palette);
	free(indexes);
	
	return img;
}

void free_image(image_t *image) {
	free(image->pixels);
	free(image);
}

void draw_image(image_t *image) {
	vec2_t cursor = get_cursor();
	uint32_t *pixels = image->pixels;
	int w = image->w;
	int h = image->h;
	
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			uint32_t c = pixels[x + y * w];
			set_pixel(cursor.x + x, cursor.y + y, c);
		}
	}
}
