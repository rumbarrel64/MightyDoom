// map.c
#include "map.h"

void map_init(Map *map, const char *model_path) {
    // Load model
    map->model = t3d_model_load(model_path);

    // Default position
    map->position = (T3DVec3){{0, 0, -10}};

    // Allocate matrix
    map->matrix = malloc_uncached(sizeof(T3DMat4FP));

    // Default scale and rotation
    float scale[3] = {0.3f, 0.3f, 0.3f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};

    // Create transform matrix
    t3d_mat4fp_from_srt_euler(map->matrix, scale, rotation, map->position.v);
}

void map_draw(Map *map) {
    t3d_matrix_push(map->matrix);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(map->model);
    t3d_matrix_pop(1);
}

/*
void map_destroy(Map *map) {
    t3d_model_free(map->model);
    // Optionally free map->matrix if you want to release memory
}
    */

void map_destroy(Map *map) {
    // Free model matrix
    free_uncached(map->matrix);
    // Free model
    t3d_model_free(map->model);
}
