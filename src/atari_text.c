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
	const int num_chars = atari_font_data_len / 8;

	c = c % num_chars;
	for (int y1 = 0; y1 < 8; y1++) {
		uint8_t acc = atari_font_data[c * 8 + y1];
		for (int x1 = 7; x1 >= 0; x1--) {
			if (acc & 1) {
				fill_rect(x + x1 * scale, y + y1 * scale, scale, scale);
			}
			acc = acc >> 1;
		}
	}
}