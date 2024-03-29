// atari_text.c

#include "atari_text.h"
#include "drawing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Globals
uint8_t *atari_font = NULL;
size_t atari_font_len = 0;


bool atari_text_init(void) {
	// Load Atari font from file
	FILE *file = fopen("assets/font.data", "rb");
	if (!file) {
		fprintf(stderr, "Unable to open font file!\n");
		return false;
	}

	fseek(file, 0L, SEEK_END);
	atari_font_len = ftell(file);
	fseek(file, 0L, SEEK_SET);
	
	atari_font = malloc(atari_font_len);
	if (!atari_font) {
		fprintf(stderr, "Unable to allocate font!\n");
		return false;
	}
	
	if (fread(atari_font, atari_font_len, 1, file) != 1) {
		fprintf(stderr, "Unable to load font!\n");
		return false;
	}

	return true;
}

void atari_renderer_dispose(void) {
	free(atari_font);
}

void atari_draw_text(const char* s, int scale) {
	// Draw text at cursor using the fill color
	vec2_t cursor = get_cursor();

	while (*s != 0) {
		atari_draw_char(*s, cursor.x, cursor.y, scale);
		cursor.x += 8 * scale;
		s++;
	}
	
	move_to(cursor);
}

void atari_draw_centered_text(const char* s, int scale) {
	size_t w = strnlen(s, 255);
	vec2_t p = get_cursor();
	p.x -= w * 4 * scale;
	move_to(p);
	atari_draw_text(s, scale);
}

void atari_draw_char(char c, int x, int y, int scale) {
	const int num_chars = (int)atari_font_len / 8;

	c = c % num_chars;
	for (int y1 = 0; y1 < 8; y1++) {
		uint8_t acc = atari_font[c * 8 + y1];
		for (int x1 = 7; x1 >= 0; x1--) {
			if (acc & 1) {
				fill_rect(x + x1 * scale, y + y1 * scale, scale, scale);
			}
			acc = acc >> 1;
		}
	}
}

void atari_draw_test_text(void) {
	int i = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 16; x++) {
			atari_draw_char(i, x * 8, y * 8, 1);
			i++;
		}
	}
}

