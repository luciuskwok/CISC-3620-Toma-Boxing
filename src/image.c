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


image_t *create_abgr_image_from_indexed_bitmap(uint8_t *bitmap, int w, int h, bool flipped, uint32_t *palette, int palette_len) {
	// Create image_t
	image_t *img = (image_t *)malloc(sizeof(image_t));
	if (!img) {
		fprintf(stderr, "Unable to allocate image_t.\n");
		return NULL;
	}
	img->w = w;
	img->h = h;
	int n = w * h;
	uint32_t *pixels = (uint32_t *)malloc(n * sizeof(uint32_t));
	if (!pixels) {
		fprintf(stderr, "Unable to allocate memory for pixels.\n");
		return NULL;
	}
	img->pixels = pixels;
	
	// Convert from indexed to ARGB
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int i = x + y * w;
			int j = x + ( flipped? (h - y - 1) : y) * w;
			pixels[j] = palette[bitmap[i]];
		}
	}

	return img;
}

image_t *load_indexed_image(const char *file) {
	FILE *in = fopen(file, "rb");
	if (!in) {
		fprintf(stderr, "File could not be opened!\n");
		return NULL;
	}
	
	// Header: sig, width, height, and number of palette entries
	uint32_t sig = read_uint32(in);
	uint32_t w = read_uint32(in);
	uint32_t h = read_uint32(in);
	uint32_t palette_len = read_uint32(in);
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
	
	// Read bitmap
	uint8_t *bitmap = malloc(n);
	if (!bitmap) {
		fprintf(stderr, "Unable to allocate bitmap!\n");
		return NULL;
	}
	if (!read_buffer(in, bitmap, n)) {
		fprintf(stderr, "Unable to read bitmap!\n");
		return NULL;
	}
	
	// Create image_t
	image_t *img = create_abgr_image_from_indexed_bitmap(bitmap, w, h, false, palette, palette_len);

	// Clean up
	fclose(in);
	free(palette);
	free(bitmap);
	
	return img;
}

void convert_bmp_palette_to_abgr(uint32_t *palette, int palette_len) {
	uint8_t *p = (uint8_t *)palette;
	uint8_t r, g, b;
	for (int i = 0; i < palette_len; i++) {
		b = p[i * 4 + 0];
		g = p[i * 4 + 1];
		r = p[i * 4 + 2];
		
		p[i * 4 + 0] = r;
		p[i * 4 + 1] = g;
		p[i * 4 + 2] = b;
		p[i * 4 + 3] = 0xFF;
	}
}

image_t *load_bmp_image(const char *file) {
	// Windows BMP format. Only BMP Version 3 (Windows 3.x) format is supported.
	FILE *in = fopen(file, "rb");
	if (!in) {
		fprintf(stderr, "File could not be opened!\n");
		return NULL;
	}

	// Header: sig, width, height, and number of palette entries
	uint16_t FileType = read_uint16(in);
	if (FileType != 0x4D42) {
		fprintf(stderr, "Header of file does not match BMP header.\n");
		return NULL;
	}
	fseek(in, 8, SEEK_CUR); // Skip FileSize, Reserved1, Reserved2
	uint32_t BitmapOffset = read_uint32(in);
	uint32_t BitmapHeader_Size = read_uint32(in);
	if (BitmapHeader_Size != 40) { // Should be 40 for BMP Version 3
		fprintf(stderr, "Unexpected BitmapHeader_Size for BMP Version 3 format.\n");
		return NULL;
	}
	int32_t BitmapHeader_Width = read_int32(in);
	int32_t BitmapHeader_Height = read_int32(in);
	uint16_t BitmapHeader_Planes = read_uint16(in);
	uint16_t BitmapHeader_BitsPerPixel = read_uint16(in);
	uint32_t BitmapHeader_Compression = read_uint32(in);
	uint32_t BitmapHeader_SizeOfBitmap = read_uint32(in);
	fseek(in, 16, SEEK_CUR); // Skip HorzResolution, VertResolution, BitmapHeader_ColorsUsed, ColorsImportant
	
	// Debugging: write out header data
	/*
	printf("Windows BMP file header:\n");
	printf("  BitmapOffset = %d\n", BitmapOffset);
	printf("  Width = %d\n", BitmapHeader_Width);
	printf("  Height = %d\n", BitmapHeader_Height);
	printf("  Planes = %d\n", BitmapHeader_Planes);
	printf("  BitsPerPixel = %d\n", BitmapHeader_BitsPerPixel);
	printf("  Compression = %d\n", BitmapHeader_Compression);
	printf("  SizeOfBitmap = %d\n", BitmapHeader_SizeOfBitmap);
	printf("  ColorsUsed = %d\n", BitmapHeader_ColorsUsed); */

	if (BitmapHeader_Planes != 1 || BitmapHeader_BitsPerPixel != 8 || BitmapHeader_Compression != 0) {
		fprintf(stderr, "Unsupported BMP bitmap format.\n");
		return NULL;
	}
	
	// Flipped coordinates
	bool flipped = true;
	if (BitmapHeader_Height < 0) {
		flipped = false;
		BitmapHeader_Height = -BitmapHeader_Height;
	}
	
	// Palette
	int palette_len = (BitmapOffset - 54) / 4;
	// printf("  palette_len = %d\n", palette_len);
	uint32_t *palette = malloc(palette_len * sizeof(uint32_t));
	if (!palette) {
		fprintf(stderr, "Unable to allocate palette!\n");
		return NULL;
	}
	if (!read_buffer(in, palette, palette_len * sizeof(uint32_t))) {
		fprintf(stderr, "Unable to read palette!\n");
		return NULL;
	}
	convert_bmp_palette_to_abgr(palette, palette_len);
	
	// Read bitmap data
	uint8_t *bitmap = malloc(BitmapHeader_SizeOfBitmap);
	if (!bitmap) {
		fprintf(stderr, "Unable to allocate pixels!\n");
		return NULL;
	}
	if (!read_buffer(in, bitmap, BitmapHeader_SizeOfBitmap)) {
		fprintf(stderr, "Unable to read pixels!\n");
		return NULL;
	}
	
	// Create image_t
	image_t *img = create_abgr_image_from_indexed_bitmap(bitmap, BitmapHeader_Width, BitmapHeader_Height, flipped, palette, palette_len);

	// Clean up
	fclose(in);
	free(palette);
	free(bitmap);
	
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
