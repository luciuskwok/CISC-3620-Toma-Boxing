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
int get_music_volume(void);
void set_music_volume(int vol);
bool is_music_muted(void);
void set_music_muted(bool state);


#endif /* audio_player_h */
