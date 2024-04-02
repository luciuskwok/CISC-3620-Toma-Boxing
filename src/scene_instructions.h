//
//  scene_instructions.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef scene_instructions_h
#define scene_instructions_h

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

void instructions_init(void);
void instructions_start(void);
bool instructions_handle_keyboard(SDL_Event event);
void instructions_update(double delta_time);
void instructions_render(void);

#endif /* scene_instructions_h */
