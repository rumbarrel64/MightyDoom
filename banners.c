#include "banners.h"

static T3DModel *spawn_in_model;
static T3DModel *blood_model;

void banners_init(void) {
    // Load in Models
    spawn_in_model = t3d_model_load("rom:/enemyFloorIntro.t3dm"); // Enemy Spawn In
    // Enemy Alive
    // Enemy Dead
    // Shrinking Blood
    blood_model = t3d_model_load("rom:/bloodSplatter.t3dm"); // Enemy Spawn In
    // Player 
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

void banners_destroy(void) {
        // Free Models
        t3d_model_free(spawn_in_model);
        t3d_model_free(blood_model);
}
