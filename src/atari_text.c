// atari_text.c

#include "atari_text.h"
#include "drawing.h"
#include "color.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Globals
uint8_t *atari_font = NULL;
size_t atari_font_len = 0;
uint32_t key_text_color = COLOR_ABGR_BLACK;


bool atari_text_init(void) {
	// Load Atari font from file
	FILE *file = fopen("assets/custom_font", "rb");
	if (!file) {
		fprintf(stderr, "Unable to open font file!\n");
		return false;
	}

	fseek(file, 0L, SEEK_END);
	atari_font_len = (size_t)ftell(file);
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
		atari_draw_char(*s, (int)cursor.x, (int)cursor.y, scale);
		cursor.x += 8.0f * scale;
		s++;
	}
	
	move_to(cursor);
}

void atari_draw_centered_text(const char* s, int scale) {
	int w = (int)strnlen(s, 255);
	vec2_t p = get_cursor();
	p.x -= w * 4 * scale;
	move_to(p);
	atari_draw_text(s, scale);
}

void atari_draw_right_justified_text(const char* s, int scale) {
	int w = (int)strnlen(s, 255);
	vec2_t p = get_cursor();
	p.x -= w * 8 * scale;
	move_to(p);
	atari_draw_text(s, scale);
}

void atari_draw_shadowed_text(const char* s, int scale, uint32_t shadow_color) {
	// Save old parameters
	uint32_t fill = get_fill_color();
	vec2_t cursor = get_cursor();
	
	// Draw shadow below
	vec2_t shadow_position = { cursor.x + 1, cursor.y + 1 };
	move_to(shadow_position);
	set_fill_color_abgr(shadow_color);
	atari_draw_text(s, scale);
	
	// Draw normal text on top
	move_to(cursor);
	set_fill_color_abgr(fill);
	atari_draw_text(s, scale);
}

void draw_key_text_line(const char *key, const char *text) {
	const int line_height = 10;
	const uint32_t text_color = get_fill_color();
	
	vec2_t p = get_cursor();
	if (key) {
		set_fill_color_abgr(key_text_color);
		atari_draw_text(key, 1);
	}
	if (key && text) {
		// Half space
		vec2_t q = get_cursor();
		q.x += 4;
		move_to(q);
	}
	if (text) {
		set_fill_color_abgr(text_color);
		atari_draw_text(text, 1);
	}
	
	p.y += line_height;
	move_to(p);
}

void set_key_text_color(uint32_t c) {
	key_text_color = c;
}

#pragma mark -

void atari_draw_char(char c, int x, int y, int scale) {
	const int num_chars = (int)atari_font_len / 8;

	c = c % num_chars;
	for (int y1 = 0; y1 < 8; y1++) {
		uint8_t acc = atari_font[c * 8 + y1];
		for (int x1 = 7; x1 >= 0; x1--) {
			if (acc & 1) {
				rectangle_t r = { x + x1 * scale, y + y1 * scale, scale, scale };
				fill_rect(r);
			}
			acc = acc >> 1;
		}
	}
}

void atari_draw_test_text(void) {
	char i = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 16; x++) {
			atari_draw_char(i, x * 8, y * 8, 1);
			i++;
		}
	}
}

