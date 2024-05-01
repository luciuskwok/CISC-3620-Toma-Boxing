//
//  audio_player.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/25/24.
//

#include "audio_player.h"

#include <SDL2/SDL.h>
#include <stdio.h>

// Slightly different locations of header files
#ifdef __EMSCRIPTEN__
#include <SDL2/SDL_mixer.h>
#else
#include <SDL2_mixer/SDL_mixer.h>
#endif

// Constants
#define DEFAULT_CHUNK_SIZE (512)
#define SFX_CHANNEL (0)
#define MAX_MUSIC_VOLUME (9)

// Globals
//SDL_AudioSpec audio_spec;
Mix_Music *song = NULL;
bool music_paused = false;
int music_volume = MAX_MUSIC_VOLUME;
bool music_muted = false;

// Functions

bool init_audio(void) {
	// Init SDL for audio
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "SDL_Init(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
		return false;
	}
	
	// Open the audio device
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, DEFAULT_CHUNK_SIZE) < 0) {
		fprintf(stderr, "Mix_OpenAudio() failed: %s\n", SDL_GetError());
		return false;
	}
	
	// Load the audio file
	song = Mix_LoadMUS("assets/song_90s.ogg");
	if (song == NULL) {
		fprintf(stderr, "Mix_LoadMUS() failed: %s\n", SDL_GetError());
		return false;
	}
	
	// Set volume to -10 dB
	set_music_volume(7);
	
	return true;
}

void start_music(void) {
	// fprintf(stdout, "Start audio playback.\n");
	Mix_PlayMusic(song, 0);
	music_paused = false;
}

void pause_music(bool state) {
	// fprintf(stdout, "Pause audio playback.\n");
	if (state) {
		Mix_PauseMusic();
		music_paused = true;
	} else {
		Mix_ResumeMusic();
		music_paused = false;
	}
}

void stop_music(void) {
	// fprintf(stdout, "Stop audio playback.\n");
	Mix_HaltMusic();
}

bool is_music_playing(void) {
	return Mix_PlayingMusic() != 0;
}

bool is_music_paused(void) {
	return music_paused;
}

int get_music_volume(void) {
	return music_volume;
}

void set_music_volume(int vol) {
	// Limit vol to range 0-9
	vol = (vol <= 9)? vol : 9;
	vol = (vol >= 1)? vol : 1;

	music_volume = vol;
	music_muted = false;
	float db = (vol - MAX_MUSIC_VOLUME) * 3.0f;
	float scalar = powf(10.0f, db/10.0f);
	Mix_VolumeMusic((int)roundf(scalar * 128.0f));
	
	//fprintf(stdout, "Volume: %1.0f dB, %1.4f\n", db, scalar);
}

bool is_music_muted(void) {
	return music_muted;
}

void set_music_muted(bool state) {
	music_muted = state;
	if (music_muted) {
		Mix_VolumeMusic(0);
	} else {
		set_music_volume(music_volume);
	}
}

double get_music_duration(void) {
	return Mix_MusicDuration(song);
}

double get_music_position(void) {
	return Mix_GetMusicPosition(song);
}
