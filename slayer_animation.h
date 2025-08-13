#pragma once
#include "libs.h"
#include "slayer.h"
#include "banners.h"

// Animation states for the slayer's dramatic entrance sequence
typedef enum {
    ANIM_STATE_WAITING,      // 0.0s - 1.15s: waiting to fall (slayer not visible)
    ANIM_STATE_FALLING,      // 1.15s - 2.0s: slayer falling from above 
    ANIM_STATE_LANDED,       // 2.0s - 3.0s: landed with crack effect visible
    ANIM_STATE_COMPLETE      // 3.0s+: normal gameplay, animation done
} AnimationState;

// Contains all data needed for the slayer entrance animation
typedef struct {
    AnimationState state;           // Current animation phase
    float state_start_time;         // When current state began
    float start_y;                  // Starting height for fall (e.g., 125.0f)
    float target_y;                 // Landing height (ground level, e.g., 0.15f)
    bool slayer_has_control;        // Whether slayer can move/camera toggle
} SlayerAnimation;

// Initialize the animation system with starting and ending heights
void slayer_animation_init(SlayerAnimation *anim, float start_height, float target_height);

// Update animation state and slayer position each frame
void slayer_animation_update(SlayerAnimation *anim, Player *slayer, Crack *crack, float tutorial_time);

// Check if slayer should be drawn (visible during falling/landed/complete states)
bool slayer_animation_should_draw_slayer(SlayerAnimation *anim);

// Check if crack effect should be drawn (only during landed state)
bool slayer_animation_should_draw_crack(SlayerAnimation *anim);

// Check if slayer has control (movement/camera - only after landing)
bool slayer_animation_has_control(SlayerAnimation *anim);