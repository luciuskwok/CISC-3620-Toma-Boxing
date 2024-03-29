//
//  scene_manager.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_manager.h"

// Globals
SCENE_INDEX scene_index;


SCENE_INDEX get_scene_index(void) {
	return scene_index;
}

void set_scene_index(SCENE_INDEX x) {
	scene_index = x;
}

