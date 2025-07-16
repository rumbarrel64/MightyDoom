// zombie.h
#pragma once

#include "libs.h"

#define ZOMBIE_COLLISION_RADIUS 20.0f

typedef struct {
    T3DVec3 position;
    float rotation_y;

    T3DModel *model;

    // Skeletons
    T3DSkeleton skel;
    T3DSkeleton skelBlend;

    // Animations
    T3DAnim anim_walk;
    T3DAnim anim_attack;
    bool playing_attack; 
    bool is_attacking; 
    float attack_timer;
    float attack_blending_ratio;

    T3DMat4FP *model_matrix;
    float speed;

    int health;
    bool alive;
    
    // Tracks time after death
    float blood_time;
    float blood_scale;
    
} Zombie;

// Initialize one zombie with a starting position
void zombie_init(Zombie *z, const T3DVec3 *start_pos);

// Update one zombie's movement and animation
void zombie_update(Zombie *z, const T3DVec3 *player_pos, float delta_time, Zombie *zombies, int count);

// New
void draw_zombie_health_bar(const Zombie *z, T3DViewport *viewport);

// Draw one zombie
void zombie_draw(Zombie *z);

// Clean up a single zombie's resources
void zombie_destroy(Zombie *z);

// Bulk helpers
void zombie_update_all(Zombie *zombies, int count, const T3DVec3 *player_pos, float delta_time);
void zombie_draw_all(Zombie *zombies, int count);
void zombie_destroy_all(Zombie *zombies, int count);
