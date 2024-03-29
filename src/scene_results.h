//
//  scene_results.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef scene_results_h
#define scene_results_h

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>


void results_init(void);
bool results_handle_keyboard(SDL_Event event);
void results_update(uint64_t frame_index);
void results_render(void);

#endif /* scene_results_h */
