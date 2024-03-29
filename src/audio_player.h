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
void start_music(void);
void pause_music(bool state);
void stop_music(void);

bool is_music_playing(void);
bool is_music_paused(void);
void set_music_volume(float vol);


#endif /* audio_player_h */
