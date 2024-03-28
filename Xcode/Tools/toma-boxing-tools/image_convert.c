//
//  image_convert.c
//  toma-boxing-tools
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "image_convert.h"

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

// Constants
#define FILE_SIGNATURE (0x494E4458) /* Spells out 'INDX' */

// Image type
typedef struct {
	uint32_t *pixels;
	uint32_t w;
	uint32_t h;
} image_t;

// Convert PNG to indexed image that is printed to stdout

uint32_t swap_red_blue(uint32_t x) {
	uint32_t r = (x & 0x00FF0000) >> 16;
	uint32_t b = (x & 0x000000FF);
	return (x & 0xFF00FF00) | (b << 16) | (r);
}

image_t *load_image(const char *file) {
	SDL_Surface *surface = IMG_Load(file);
	if (!surface) {
		fprintf(stderr, "IMG_Load() failed: %s\n", SDL_GetError());
		return NULL;
	}

	uint32_t fmt = surface->format->format;
	if (fmt != SDL_PIXELFORMAT_ARGB8888 && fmt != SDL_PIXELFORMAT_ABGR8888) {
		fprintf(stderr, "Unsupported image surface format: 0x%08x\n", fmt);
		SDL_FreeSurface(surface);
		return NULL;
	}

	// Allocate
	image_t *img = (image_t *)malloc(sizeof(image_t));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory for img.\n");
		SDL_FreeSurface(surface);
		return NULL;
	}
	img->w = surface->w;
	img->h = surface->h;
	uint32_t *pixels = (uint32_t *)malloc(img->w * img->h * sizeof(uint32_t));
	if (!pixels) {
		fprintf(stderr, "Unable to allocate memory for pixels.\n");
		SDL_FreeSurface(surface);
		return NULL;
	}
	img->pixels = pixels;
	
	// Copy the data in the image into our own image struct
	uint32_t *src = (uint32_t *)(surface->pixels);
	for (int i = 0; i < img->w * img->h; i++) {
		if (fmt == SDL_PIXELFORMAT_ABGR8888) {
			pixels[i] = src[i];
		} else {
			pixels[i] = swap_red_blue(src[i]);
		}
	}

	SDL_FreeSurface(surface);
	
	return img;
}

int color_exists_in_palette(uint32_t color, uint32_t *palette, int palette_len) {
	for (int i = 0; i < palette_len; i++) {
		if (palette[i] == color) return i;
	}
	return -1;
}

size_t write_int32(FILE *file, uint32_t x) {
	size_t num_members_written = fwrite(&x, sizeof(x), 1, file);
	return sizeof(x) * num_members_written;
}

size_t write_buffer(FILE *file, void *buf, size_t buf_len) {
	size_t num_members_written = fwrite(buf, buf_len, 1, file);
	return buf_len * num_members_written;
}

void convert_png(const char *input, const char *output) {
	image_t *img = load_image(input);
	if (!img) {
		fprintf(stderr, "load_image() returned NULL.\n");
		return;
	}
	
	// Construct color palette
	uint32_t palette[256];
	int palette_len = 0;
	
	// Store indexes in array of bytes, 8 bits per pixel
	int n = img->w * img->h;
	uint8_t *indexes = malloc(n);
	if (!indexes) {
		fprintf(stderr, "Unable to allocate memory for indexes.\n");
		return;
	}
	
	for (int i = 0; i < n; i++) {
		uint32_t pixel = img->pixels[i];
		int color_index = color_exists_in_palette(pixel, palette, palette_len);
		if (color_index < 0) {
			palette[palette_len] = pixel;
			color_index = palette_len;
			palette_len++;
		}
		indexes[i] = color_index;
	}
	
	// Print out color palette and indexes in C code
	const int palette_entries_per_line = 4;
	printf("uint32_t palette[] = {\n\t");
	for (int j = 0; j < palette_len; j++) {
		printf("0x%08x", palette[j]);
		if (j < palette_len - 1) printf(", ");
		if (j % palette_entries_per_line == palette_entries_per_line - 1) printf("\n\t");
	}
	printf("\n};\n");
	printf("int palette_len = %d;\n\n", palette_len);
	
	const int indexes_entries_per_line = 8;
	printf("uint8_t indexes[] = {\n\t");
	for (int i = 0; i < n; i++) {
		printf("0x%02x", indexes[i]);
		if (i < n - 1) printf(", ");
		if (i % indexes_entries_per_line == indexes_entries_per_line - 1) printf("\n\t");
	}
	printf("\n};\n");
	printf("int indexes_len = %d;\n\n", n);
	
	// Write out file
	int total_written = 0;
	FILE *out = fopen(output, "wb");
	
	// Header: sig, width, height, and number of palette entries
	total_written += write_int32(out, FILE_SIGNATURE);
	total_written += write_int32(out, img->w);
	total_written += write_int32(out, img->h);
	total_written += write_int32(out, palette_len);
	
	// Color Palette
	total_written += write_buffer(out, palette, palette_len * sizeof(uint32_t));

	// Image data
	total_written += write_buffer(out, indexes, n * sizeof(uint8_t));
	
	// Clean up
	fclose(out);
	free(indexes);
	
	printf("File written with %d bytes\n", total_written);
}

