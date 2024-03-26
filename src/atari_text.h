// atari_text.h

#ifndef atari_text_h
#define atari_text_h

#include <stdbool.h>
#include <stdint.h>

bool atari_renderer_init(void);
void atari_renderer_dispose(void);

void atari_draw_text(const char* s, int scale);
void atari_draw_centered_text(const char* s, int scale);
void atari_draw_char(char c, int x, int y, int scale);

#endif
