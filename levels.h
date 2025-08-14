// levels.h
#pragma once

#include "libs.h"

#define MAX_ZOMBIES 16

typedef struct {
    T3DVec3 position;
    float rotation_y;
} SpawnData;

typedef struct {
    T3DVec3 position;
    float rotation_y;
    bool spawn_delay;  // If true, wait for slayer Y <= -10
} EnemySpawnData;

typedef struct {
    SpawnData player;
    int zombie_count;
    EnemySpawnData zombies[MAX_ZOMBIES];
} LevelData;

extern const LevelData LEVEL_1;
extern const LevelData LEVEL_2;
extern const LevelData LEVEL_END; 

extern const LevelData* ALL_LEVELS[];
extern const int TOTAL_LEVELS;
