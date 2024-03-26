//
//  image.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/26/24.
//

#include "image.h"
#include "drawing.h"
#include "vector.h"

#include <SDL2/SDL.h>
#include <stdio.h>

// Slightly different locations of header files
#ifdef __EMSCRIPTEN__
#include <SDL2/SDL_image.h>
#else
#include <SDL2_image/SDL_image.h>
#endif

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
