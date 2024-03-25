//
//  audio_player.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/25/24.
//

#ifndef audio_player_h
#define audio_player_h

#include <stdbool.h>

bool init_audio(void);
void start_audio(void);
void pause_audio(bool state);
void stop_audio(void);

bool is_audio_playing(void);
bool is_audio_paused(void);
void set_audio_volume(float vol);


#endif /* audio_player_h */
