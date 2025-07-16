// camera.c
#include "camera.h"

void camera_init(Camera *cam) {
    cam->viewport = t3d_viewport_create();
    cam->position = (T3DVec3){{0, 125, 135}};
    cam->target   = (T3DVec3){{0, 0, 0}};
    cam->mode     = CAMERA_TOP_DOWN;

    t3d_viewport_set_projection(&cam->viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
}

void camera_update(Camera *cam, const T3DVec3 *player_pos, float rotation_y) {
    cam->target = *player_pos;

    if (cam->mode == CAMERA_TOP_DOWN) {
        cam->position.v[0] = cam->target.v[0];
        cam->position.v[1] = cam->target.v[1] + 180;
        cam->position.v[2] = cam->target.v[2];
        t3d_viewport_look_at(&cam->viewport, &cam->position, &cam->target, &(T3DVec3){{0, 0, -1}});
    } else {
        cam->target.v[2] -= 20;
        cam->position.v[0] = cam->target.v[0];
        cam->position.v[1] = cam->target.v[1] + 45;
        cam->position.v[2] = cam->target.v[2] + 65;
        t3d_viewport_look_at(&cam->viewport, &cam->position, &cam->target, &(T3DVec3){{0, 1, 0}});
    }
}

T3DViewport *camera_get_viewport(Camera *cam) {
    return &cam->viewport;
}

void camera_toggle_mode(Camera *cam) {
    cam->mode = (cam->mode == CAMERA_TOP_DOWN) ? CAMERA_BEHIND_PLAYER : CAMERA_TOP_DOWN;
}
