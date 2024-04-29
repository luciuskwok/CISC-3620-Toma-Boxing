//
//  sequencer.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/29/24.
//

#ifndef sequencer_h
#define sequencer_h

#include <stdio.h>
#include "scene_gameplay.h"

void sequencer_start(gameplay_t *scene);
void sequencer_update(gameplay_t *scene, double previous_time, double current_time);


#endif /* sequencer_h */
