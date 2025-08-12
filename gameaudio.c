// music.c
#include "gameaudio.h"
#include <audio.h>
#include <mixer.h>

// Mixer channel allocation (More Channels, More Audio!!)
// Max channles 16
#define CHANNEL_MUSIC 0  // Load music in and out into to this channel
#define CHANNEL_SFX1 1   // Load sound effect for Fight (maybe others later)
#define CHANNEL_SFX2 2   // Additional channel for future sounds

// Music
static wav64_t music_wav; // Game Music

// Sound Effects
static wav64_t sfx1_wav;  // Voice Over

void audio_system_init(void) {
    // Initialize audio system
    audio_init(22050, 4);           // 22.05 kHz, 4 buffers
    mixer_init(16);                 // 16 mixer channels
    wav64_init_compression(1);      // compression level 1
    mixer_ch_set_limits(CHANNEL_MUSIC, 0, 48000, 0);
}

// ========== LOADING FUNCTIONS (prepare audio, don't play yet) ==========
void music_load(const char *filename) {
    wav64_open(&music_wav, filename);
    wav64_set_loop(&music_wav, true);
    //wav64_play(&music_wav, CHANNEL_MUSIC);
}

void sfx1_load(const char *filename) {
    wav64_open(&sfx1_wav, filename);
    wav64_set_loop(&sfx1_wav, false);  // Play once only
	//wav64_play(&sfx1_wav, CHANNEL_SFX1);
}

// ========== PLAYING FUNCTIONS (actually start the sound) ==========
void music_play(void) {
    wav64_play(&music_wav, CHANNEL_MUSIC);
}

void sfx1_play(void) {
    wav64_play(&sfx1_wav, CHANNEL_SFX1);
}


// ========== AUDIO PROCESSING (handles ALL audio - music + SFX) ==========
void audio_update(void) {

    mixer_try_play();
    /*  
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
      */
}

// ========== STOPPING FUNCTIONS ==========
void music_stop(void) {
    mixer_ch_stop(CHANNEL_MUSIC);
}

void sfx1_stop(void) {
    mixer_ch_stop(CHANNEL_SFX1);
}

// ========== CLEANUP FUNCTIONS ==========
void music_cleanup(void) {
    mixer_ch_stop(CHANNEL_MUSIC);
    wav64_close(&music_wav);
}

void sfx1_cleanup(void) {
    mixer_ch_stop(CHANNEL_SFX1);
    wav64_close(&sfx1_wav);
}

void audio_cleanup_all(void) {
    music_cleanup();
    sfx1_cleanup();
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