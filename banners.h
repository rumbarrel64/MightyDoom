#pragma once

#include "libs.h"

// For swtiching between banners
typedef enum {
    BANNER_SPAWN,
    BANNER_BLOOD,
    // add more here later
} BannerType;

typedef struct {
    T3DModel *model;
    T3DMat4FP *matrix;
    T3DVec3 position;
} Crack;



// Banner functions
void banners_init(void);
void banners_destroy(void);
void draw_floor_banner(const T3DMat4FP *matrix, BannerType type);

// Crack functions
void crack_init(Crack *crack, const char *model_path);
void crack_draw(Crack *crack);
void crack_cleanup(Crack *crack);
