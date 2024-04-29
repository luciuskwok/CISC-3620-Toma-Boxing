//
//  scene_gameplay.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef scene_gameplay_h
#define scene_gameplay_h

#include "array_list.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

// Scene parameters
typedef struct {
	uint32_t bg_color;
	array_list_t *shapes;
	array_list_t *meshes;
} gameplay_t;

void gameplay_init(void);
void gameplay_start(void);
bool gameplay_handle_keyboard(SDL_Event event);
void gameplay_update(double delta_time);
void gameplay_render(void);

#endif /* scene_gameplay_h */
