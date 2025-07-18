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

  }

void bullet_update(Bullet *b, const T3DVec3 *playerPos, float rot_y, Zombie *zombies, int *zombie_count, int *enemy_count) {
    
    
    // 1. Reset bullet if out of bounds
    if (b->position.v[0] < -BOX_SIZE || b->position.v[0] > BOX_SIZE ||
        b->position.v[2] < -BOX_SIZE  || b->position.v[2] > BOX_SIZE ) {
            // Reset the bullet back to player
            b->position = *playerPos;
            b->direction.v[0] = sinf(rot_y);
            b->direction.v[2] = cosf(rot_y);
            // Update the bullet rotation
            b->rotation_y = rot_y;
    }

    // 2. Check zombie collisions
    //    2a. Reduce health if collision occurs
    //    2b. Reset the bullet back to player
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
        
        // Reset the bullet back to player
        b->position = *playerPos;
        b->direction.v[0] = sinf(rot_y);
        b->direction.v[2] = cosf(rot_y);
        // Update the bullet rotation
        b->rotation_y = rot_y;
        
        }
    }

    // 3. Update bullet position
    b->position.v[0] += b->direction.v[0] * SPEED;
    b->position.v[2] += b->direction.v[2] * SPEED;

    // Update bullet matrix
    t3d_mat4fp_from_srt_euler(
        b->matrix,
        (float[3]){0.035f, 0.035f, 0.035f},      // Scale
        (float[3]){0.0f, -b->rotation_y, 0.0f},      // Rotation Bullet needs to be fixed on diagonals (0 Right good, -1,5713 up good, -3.1306 left good, 1,5713 down good)
        // Position
        (float[3]){b->position.v[0], 20.0f, b->position.v[2]}
    );
}

void bullet_draw(Bullet *b) 
  {
    t3d_matrix_push(b->matrix);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(b->model);
    t3d_matrix_pop(1);
  }

void bullet_cleanup(Bullet *b) 
  {
    // Free model matrix
    free(b->matrix);
    // Free model
    t3d_model_free(b->model);
  }