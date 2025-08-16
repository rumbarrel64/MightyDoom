#include "bullets.h"
#include <math.h>

// === Static Globals ===
static const float BOX_SIZE = 140.0f;
static const float SPEED = 1.5f;
static const float ZOMBIE_HIT_RADIUS2 = 50.0f;

void bullet_init(Bullet *b) 
  {
    // Reserve Space for Matrix
    b->matrix = malloc_uncached(sizeof(T3DMat4FP));

    // Load in Bullet Model
    b->model = t3d_model_load("rom:/bullet.t3dm");

    // Set inital postition of bullet (outside of box)
    b->position = (T3DVec3){{150.0f, 0.0f, 150.0f}};

    // Set inital rotation of bullet (outside of box)
    b->rotation_y = 0;

    // Bullet starts inactive
    b->active = false;

  }

void bullet_update(Bullet *b, const T3DVec3 *playerPos, float rot_y, Zombie *zombies, int *zombie_count, int *enemy_count) {
    
    // Check if any zombies are alive
    bool any_zombies_alive = false;
    for (int i = 0; i < *zombie_count; i++) {
        if (zombies[i].alive) {
            any_zombies_alive = true;
            break;
        }
    }
    
    // AUTOMATIC FIRING MODE (current implementation)
    // Automatically fire when bullet is inactive and zombies are alive
    if (!b->active && any_zombies_alive) {
        // Fire the bullet from player position
        b->active = true;
        b->position = *playerPos;
        b->direction.v[0] = sinf(rot_y);
        b->direction.v[2] = cosf(rot_y);
        b->rotation_y = rot_y;
    }
    
    /* MANUAL FIRING MODE (uncomment to use button press instead)
    // To use this, also add joypad_buttons_t btn parameter to function signature
    if (btn.a && !b->active && any_zombies_alive) {
        // Fire the bullet from player position
        b->active = true;
        b->position = *playerPos;
        b->direction.v[0] = sinf(rot_y);
        b->direction.v[2] = cosf(rot_y);
        b->rotation_y = rot_y;
    }
    */
    
    // If no zombies are alive, deactivate bullet
    if (!any_zombies_alive && b->active) {
        b->active = false;
        b->position = (T3DVec3){{150.0f, 0.0f, 150.0f}};
    }
    
    // Only update bullet if it's active
    if (b->active) {
        // 1. Check if bullet is out of bounds
        if (b->position.v[0] < -BOX_SIZE || b->position.v[0] > BOX_SIZE ||
            b->position.v[2] < -BOX_SIZE || b->position.v[2] > BOX_SIZE) {
            // Deactivate bullet
            b->active = false;
            // Move it off-screen
            b->position = (T3DVec3){{150.0f, 0.0f, 150.0f}};
            return;
        }

        // 2. Check zombie collisions
        for (int j = 0; j < *zombie_count; j++) {
            if (!zombies[j].alive) continue;

            float dx = b->position.v[0] - zombies[j].position.v[0];
            float dz = b->position.v[2] - zombies[j].position.v[2];
            float dist2 = dx * dx + dz * dz;

            if (dist2 <= ZOMBIE_HIT_RADIUS2) {
                zombies[j].health--;
                if (zombies[j].health <= 0) {
                    zombies[j].alive = false;
                    zombies[j].blood_time = get_ticks_us() / 1000000.0;
                    (*enemy_count)--;
                }
                
                // Deactivate bullet after hitting a zombie
                b->active = false;
                // Move it off-screen
                b->position = (T3DVec3){{150.0f, 0.0f, 150.0f}};
                return;
            }
        }

        // 3. Update bullet position
        b->position.v[0] += b->direction.v[0] * SPEED;
        b->position.v[2] += b->direction.v[2] * SPEED;
    }

    // Always update bullet matrix (even when inactive, for when it needs to be drawn)
    t3d_mat4fp_from_srt_euler(
        b->matrix,
        (float[3]){0.035f, 0.035f, 0.035f},      // Scale
        (float[3]){0.0f, -b->rotation_y, 0.0f},  // Rotation
        (float[3]){b->position.v[0], 20.0f, b->position.v[2]}  // Position
    );
}

void bullet_draw(Bullet *b) 
  {
    // Only draw if bullet is active
    if (b->active) {
        t3d_matrix_push(b->matrix);
        rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
        t3d_model_draw(b->model);
        t3d_matrix_pop(1);
    }
  }

void bullet_cleanup(Bullet *b) 
  {
    // Free model matrix
    free_uncached(b->matrix);
    // Free model
    t3d_model_free(b->model);
  }