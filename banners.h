#pragma once

#include "libs.h"

// For swtiching between banners
typedef enum {
    BANNER_SPAWN,
    BANNER_BLOOD,
    // add more here later
} BannerType;

/// Load a shared shadow model once and reuse it for all shadows
void banners_init(void);

/// Free the shared shadow model
void banners_destroy(void);

/// Draw a shadow under a character or object
void draw_floor_banner(const T3DMat4FP *matrix, BannerType type);