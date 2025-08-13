#include "slayer_animation.h"

// Initialize animation to waiting state with specified heights
void slayer_animation_init(SlayerAnimation *anim, float start_height, float target_height) {
    anim->state = ANIM_STATE_WAITING;     // Start in waiting phase
    anim->state_start_time = 0.0f;        // No state transitions yet
    anim->start_y = start_height;         // Height to fall from (e.g., 125.0f)
    anim->target_y = target_height;       // Ground level to land on (e.g., 0.15f)
    anim->slayer_has_control = false;     // No control until landing
}

// Main animation update - handles state transitions and slayer positioning
void slayer_animation_update(SlayerAnimation *anim, Player *slayer, Crack *crack, float tutorial_time) {
    switch (anim->state) {
        case ANIM_STATE_WAITING:
            // Phase 1: Wait for 1.15 seconds before slayer appears
            if (tutorial_time >= 1.15f) {
                anim->state = ANIM_STATE_FALLING;
                anim->state_start_time = tutorial_time;
                // Slayer becomes visible and starts falling
            }
            break;
            
        case ANIM_STATE_FALLING:
            // Phase 2: Slayer falls from start_y to target_y over 0.85 seconds
            if (tutorial_time >= 2.0f) {
                // Fall complete - transition to landed state
                anim->state = ANIM_STATE_LANDED;
                anim->state_start_time = tutorial_time;
                anim->slayer_has_control = true;        // Give control back
                slayer->position.v[1] = anim->target_y; // Ensure exact landing position
            } else {
                // Calculate fall progress (0.0 to 1.0 over 0.85 seconds)
                float fall_progress = (tutorial_time - 1.15f) / 0.85f;
                // Interpolate Y position from start_y down to target_y
                slayer->position.v[1] = anim->start_y - (anim->start_y - anim->target_y) * fall_progress;
            }
            break;
            
        case ANIM_STATE_LANDED:
            // Phase 3: Slayer on ground with crack effect for 1 second
            if (tutorial_time >= anim->state_start_time + 1.0f) {
                // Crack effect duration over - move to final state
                anim->state = ANIM_STATE_COMPLETE;
            }
            // Keep slayer at ground level
            slayer->position.v[1] = anim->target_y;
            break;
            
        case ANIM_STATE_COMPLETE:
            // Phase 4: Animation done, normal gameplay continues
            slayer->position.v[1] = anim->target_y; // Maintain ground position
            break;
    }
}

// Slayer is visible during falling, landed, and complete states
bool slayer_animation_should_draw_slayer(SlayerAnimation *anim) {
    return anim->state >= ANIM_STATE_FALLING;
}

// Crack effect only visible during the 1-second landed phase
bool slayer_animation_should_draw_crack(SlayerAnimation *anim) {
    return anim->state == ANIM_STATE_LANDED;
}

// Slayer gains control only after landing (2.0+ seconds)
bool slayer_animation_has_control(SlayerAnimation *anim) {
    return anim->slayer_has_control;
}