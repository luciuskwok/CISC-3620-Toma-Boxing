//
//  scene_manager.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef scene_manager_h
#define scene_manager_h

// Game Scenes
typedef enum {
	SCENE_TITLE,		/**< Title scene */
	SCENE_INSTRUCTIONS,	/**< Instructions scene */
	SCENE_GAMEPLAY,		/**< Gameplaly scene */
	SCENE_RESULTS		/**< Results scene */
} SCENE_INDEX;

SCENE_INDEX get_scene_index(void);
void set_scene_index(SCENE_INDEX x);


#endif /* scene_manager_h */
