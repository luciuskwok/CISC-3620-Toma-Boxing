//
//  scene_title.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef scene_title_h
#define scene_title_h

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

void title_init(void);
bool title_handle_keyboard(SDL_Event event);
void title_update(double delta_time);
void title_render(void);

#endif /* scene_title_h */
