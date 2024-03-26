// atari_text.c

#include "atari_text.h"
#include "atari_font_data.h"
#include "drawing.h"

#include <string.h>


bool atari_renderer_init(void) {
	// Allocate memory

	return true;
}

void atari_renderer_dispose(void) {
}

void atari_draw_text(const char* s) {
	// Draw text at cursor using the fill color
	vec2_t cursor = get_cursor();
	int color = get_fill_color();

	while (*s != 0) {
		atari_draw_char(*s, cursor.x, cursor.y, color);
		cursor.x += 8;
		s++;
	}
	
	move_to(cursor);
}

void atari_draw_centered_text(const char* s, int x, int y) {
	size_t w = strnlen(s, 255);
	x -= w * 4;
	vec2_t p = { x, y };
	move_to(p);
	atari_draw_text(s);
}

void atari_draw_char(char c, int x, int y, uint32_t color) {
	const int num_chars = atari_font_data_len / 8;

	c = c % num_chars;
	for (int y1 = 0; y1 < 8; y1++) {
		uint8_t acc = atari_font_data[c * 8 + y1];
		for (int x1 = 7; x1 >= 0; x1--) {
			if (acc & 1) {
				set_pixel(x + x1, y + y1, color);
			}
			acc = acc >> 1;
		}
	}
}
