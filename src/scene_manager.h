//
//  scene_manager.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef scene_manager_h
#define scene_manager_h

#include "scene_title.h"
#include "scene_instructions.h"
#include "scene_gameplay.h"
#include "scene_results.h"

// Game Scenes
typedef enum {
	SCENE_STARTUP,
	SCENE_TITLE,		/**< Title scene */
	SCENE_INSTRUCTIONS,	/**< Instructions scene */
	SCENE_GAMEPLAY,		/**< Gameplaly scene */
	SCENE_RESULTS		/**< Results scene */
} SCENE_INDEX;

void init_scene_manager(void);

SCENE_INDEX get_scene_index(void);
void set_scene_index(SCENE_INDEX x);

void update_scene(double delta_time);
void draw_scene(void);
void draw_meshes(void);
void draw_shapes(void);

double get_scene_lifetime(void);

#endif /* scene_manager_h */
