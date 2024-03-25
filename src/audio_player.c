//
//  audio_player.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/25/24.
//

#include "audio_player.h"

#include <SDL2/SDL.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <stdio.h>

// Constants
#define DEFAULT_CHUNK_SIZE (512)
#define DEFAULT_CHANNEL_INDEX (0)

// Globals
//SDL_AudioSpec audio_spec;
Mix_Chunk *mix_chunk = NULL;
bool audio_paused = false;

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
	mix_chunk = Mix_LoadWAV("Toma - All Night Radio.wav");
	if (mix_chunk == NULL) {
		fprintf(stderr, "Mix_LoadWAV() failed: %s\n", SDL_GetError());
		return false;
	}
	
	return true;
}

void start_audio(void) {
	fprintf(stdout, "Start audio playback.\n");
	Mix_PlayChannel(DEFAULT_CHANNEL_INDEX, mix_chunk, false);
	audio_paused = false;
}

void pause_audio(bool state) {
	fprintf(stdout, "Pause audio playback.\n");
	if (state) {
		Mix_Pause(DEFAULT_CHANNEL_INDEX);
		audio_paused = true;
	} else {
		Mix_Resume(DEFAULT_CHANNEL_INDEX);
		audio_paused = false;
	}
}

void stop_audio(void) {
	fprintf(stdout, "Stop audio playback.\n");
	Mix_HaltChannel(DEFAULT_CHANNEL_INDEX);
}

bool is_audio_playing(void) {
	return Mix_Playing(DEFAULT_CHANNEL_INDEX) != 0;
}

bool is_audio_paused(void) {
	return audio_paused;
}
