// music.h
#pragma once

#include "libs.h"

/// Initialize the audio and mixer systems and play background music.
void music_init(void);

// Load in Music
void music_load(const char *filename);

/// Play background music.
void music_play(void);

/// Stop and cleanup music if needed (optional if you plan to expand).
void music_stop(void);
