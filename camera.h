// camera.h
#pragma once

#include "libs.h"

typedef enum {
    CAMERA_TOP_DOWN,
    CAMERA_BEHIND_PLAYER
} CameraMode;

typedef struct {
    T3DVec3 position;
    T3DVec3 target;
    CameraMode mode;
    T3DViewport viewport;
} Camera;

void camera_init(Camera *cam);
void camera_update(Camera *cam, const T3DVec3 *player_pos, float rotation_y);
T3DViewport *camera_get_viewport(Camera *cam);
void camera_toggle_mode(Camera *cam);