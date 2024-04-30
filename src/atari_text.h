// atari_text.h

#ifndef atari_text_h
#define atari_text_h

#include <stdbool.h>
#include <stdint.h>


typedef struct {
	uint8_t a[8];
} atari_char_data_t;

bool atari_text_init(void);
void atari_renderer_dispose(void);

void atari_draw_text(const char* s, int scale);
void atari_draw_centered_text(const char* s, int scale);
void atari_draw_right_justified_text(const char* s, int scale);
void atari_draw_shadowed_text(const char* s, int scale, uint32_t shadow_color);

void draw_key_text_line(const char *key, const char *text);
void set_key_text_color(uint32_t c);

void atari_draw_char(char c, int x, int y, int scale);
void atari_draw_test_text(void);

atari_char_data_t atari_get_char_data(char c);

#endif

