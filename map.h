// map.h
#pragma once

#include "libs.h"

typedef struct {
    T3DModel *model;
    T3DMat4FP *matrix;
    T3DVec3 position;
} Map;

//void map_init(Map *map, T3DModel *model);
void map_init(Map *map, const char *model_path);
void map_draw(Map *map);
void map_destroy(Map *map);
