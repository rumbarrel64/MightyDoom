// levels.h
#pragma once

#include "libs.h"

#define MAX_ZOMBIES 16

typedef struct {
    T3DVec3 position;
    float rotation_y;
} SpawnData;

typedef struct {
    SpawnData player;
    int zombie_count;
    SpawnData zombies[MAX_ZOMBIES];
} LevelData;

extern const LevelData LEVEL_1;
extern const LevelData LEVEL_2;

extern const LevelData* ALL_LEVELS[];
extern const int TOTAL_LEVELS;
