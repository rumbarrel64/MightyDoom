// gameState.h
#pragma once

typedef enum {
    STATE_MENU,
    STATE_TUTORIAL,
    STATE_GAME,
    STATE_EXIT
} GameState;

extern GameState state;