// levels.c
#include "levels.h"

const LevelData LEVEL_1 = {
    .player = {
        .position = {{0.0f, 0.15f, 104.0f}},
        .rotation_y = 3.1416f
    },
    .zombie_count = 3,
    .zombies = {
        { .position = {{  0.0f, 0.15f, -96.0f }}, .rotation_y = 0.0f },
        { .position = {{ 82.0f, 0.15f, -123.0f }}, .rotation_y = 0.0f },
        { .position = {{-87.0f, 0.15f, -123.0f }}, .rotation_y = 0.0f }
    }
};

const LevelData LEVEL_2 = {
    .player = {
        .position = {{0.0f, 0.15f, 104.0f}},
        .rotation_y = 3.1416f
    },
    .zombie_count = 1,
    .zombies = {
        { .position = {{  0.0f, 0.15f, 32.0f }}, .rotation_y = 0.0f }
    }
};

const LevelData* ALL_LEVELS[] = { &LEVEL_1, &LEVEL_2 };
const int TOTAL_LEVELS = sizeof(ALL_LEVELS) / sizeof(ALL_LEVELS[0]);