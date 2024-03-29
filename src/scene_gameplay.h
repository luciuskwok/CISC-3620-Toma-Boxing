//
//  scene_gameplay.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef scene_gameplay_h
#define scene_gameplay_h

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

void gameplay_init(void);
bool gameplay_handle_keyboard(SDL_Event event);
void gameplay_update(uint64_t frame_index);
void gameplay_render(void);

#endif /* scene_gameplay_h */
