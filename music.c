// music.c
#include "music.h"
#include <audio.h>
#include <mixer.h>

#define CHANNEL_MUSIC 0  // You can change this to another pair if needed.

static wav64_t music_wav;

void music_init(void) {
    // Initialize audio system
    audio_init(22050, 4);           // 22.05 kHz, 4 buffers
    mixer_init(16);                 // 16 mixer channels
    wav64_init_compression(1);      // compression level 1
    mixer_ch_set_limits(CHANNEL_MUSIC, 0, 48000, 0);
}

void music_load(const char *filename) {
    wav64_open(&music_wav, filename);
    wav64_set_loop(&music_wav, true);
    wav64_play(&music_wav, CHANNEL_MUSIC);
}


void music_play(void) {

    	// Check if there are any free audio buffers
	  if (audio_can_write()) {
		  // Select an audio buffer that we can write to
		  short *buf = audio_write_begin();
		  // Write to the audio buffer from the mixer
		  mixer_poll(buf, audio_get_buffer_length());
		  // Tell the audio system that the buffer has
		  // been filled and is ready for playback
		  audio_write_end();
	  };
}


void music_stop(void) {
    mixer_ch_stop(CHANNEL_MUSIC);
    wav64_close(&music_wav);
}


/*
For . xm files

  audio_init(44100, 20);
	mixer_init(32);
	mixer_ch_set_limits(6, 0, 128000, 0);
	dfs_init(DFS_DEFAULT_LOCATION);

  xm64player_t xm;
	xm64player_open(&xm, "rom:/mallard_game_music.xm64");
	xm64player_play(&xm, 0);

*/