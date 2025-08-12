// audio.h
#pragma once

#include "libs.h"

// Initialize the audio and mixer systems and play background music.
void audio_system_init(void);

// Loading functions (prepare audio files)
void music_load(const char *filename);
void sfx1_load(const char *filename);  // Fight sound
void sfx2_load(const char *filename);  // Hunt Begins sound

// Playing functions (start playing loaded audio)
void music_play(void); // Play background music
void sfx1_play(void);  // Fight
void sfx2_play(void);  // Hunt Begins

// Audio processing (call this every frame - handles ALL audio)
void audio_update(void);

// Stopping functions (stop playing but keep loaded)
void music_stop(void);
void sfx_cleanup(void);

// Cleanup functions (stop and unload from memory)
void music_cleanup(void);
void sfx1_cleanup(void);
void audio_cleanup_all(void);
