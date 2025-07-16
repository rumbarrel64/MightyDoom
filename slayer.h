// player.h
#pragma once

#include "libs.h"  // Provides access to T3D, libdragon, etc.
#include "zombie.h"

#define SLAYER_COLLISION_RADIUS 20.0f

typedef struct {
    T3DVec3 position;
    T3DVec3 move_dir;
    float rotation_y;
    float speed;
    float blend_factor;
    T3DMat4FP *model_matrix;

    T3DModel *model;
    T3DModel *model_shadow; 

    T3DSkeleton skel;
    T3DSkeleton skel_blend;
    T3DAnim anim_idle;
    T3DAnim anim_walk;
    // Optional: T3DAnim anim_attack;
} Player;

//void player_init(Player *player, T3DMat4FP *model_matrix);
void player_init(Player *player);
void player_draw(Player *player);
void draw_player_health_bar(const Player *player, T3DViewport *viewport);
void player_update(Player *player, float delta_time, joypad_inputs_t joypad, joypad_buttons_t buttons, Zombie *zombies, int zombie_count);
void player_cleanup(Player *player);
