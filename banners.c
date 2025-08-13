#include "banners.h"

static T3DModel *spawn_in_model;
static T3DModel *blood_model;

void banners_init(void) {
    // Load in Models
    spawn_in_model = t3d_model_load("rom:/enemyFloorIntro.t3dm"); // Enemy Spawn In
    // Enemy Alive
    // Enemy Dead
    blood_model = t3d_model_load("rom:/bloodSplatter.t3dm"); // Shrinking Blood
    
}

void crack_init(Crack *crack, const char *model_path) {
    // Load model
    crack->model = t3d_model_load(model_path);

    // Default position
    crack->position = (T3DVec3){{0.0f, 0.15f, 104.0f}}; // This must match the slayers postition from levels.c

    // Allocate matrix
    crack->matrix = malloc_uncached(sizeof(T3DMat4FP));

    // Default scale and rotation
    float scale[3] = {0.5f, 0.5f, 0.5f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};

    // Create transform matrix
    t3d_mat4fp_from_srt_euler(crack->matrix, scale, rotation, crack->position.v);
}

void draw_floor_banner(const T3DMat4FP *matrix, BannerType type) {
    t3d_matrix_push(matrix);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));

    switch (type) {
        case BANNER_SPAWN:
            t3d_model_draw(spawn_in_model);
            break;
        case BANNER_BLOOD:
            t3d_model_draw(blood_model); // 2kb per call
            break;
        default:
            break;
    }

    t3d_matrix_pop(1);
}

void crack_draw(Crack *crack) 
  {
    t3d_matrix_push(crack->matrix);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(crack->model);
    t3d_matrix_pop(1);
  }

void crack_cleanup(Crack *crack) 
  {
    // Free model matrix
    free_uncached(crack->matrix);
    // Free model
    t3d_model_free(crack->model);
  }

void banners_destroy(void) {
        // Free Models
        t3d_model_free(spawn_in_model);
        t3d_model_free(blood_model);
}
