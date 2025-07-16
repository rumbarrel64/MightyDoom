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







/*
void bullet_init(BulletSystem *bs, uint32_t max_bullets, float size_x, float size_y) {
    bs->max_count = max_bullets;
    bs->size_x = size_x;
    bs->size_y = size_y;
    bs->speed = 1.5f; // Bullet speed

    bs->bullets = malloc_uncached(sizeof(Bullet) * max_bullets);
    bs->matrices = malloc_uncached(sizeof(T3DMat4FP) * max_bullets);

    //bs->buffer = malloc_uncached(sizeof(TPXParticle) * max_bullets);
    bs->buffer = malloc_uncached_aligned(16, sizeof(TPXParticle) * max_bullets);

    for (uint32_t i = 0; i < max_bullets; i++) {
        bs->bullets[i].active = false;

        memset(&bs->buffer[i], 0, sizeof(TPXParticle));  // <-- important

        // Init particle visual
        int8_t *ptPos = bs->buffer[i].posA;
        uint8_t *ptColor = bs->buffer[i].colorA;
        bs->buffer[i].sizeA = 50;
        ptPos[0] = 1;
        ptPos[1] = 20;
        ptPos[2] = 0;
        ptColor[0] = 0;
        ptColor[1] = 0;
        ptColor[2] = 255;
        ptColor[3] = 255;
    }
}

void bullet_update(BulletSystem *bs, const T3DVec3 *origin, float rot_y, float box_limit, Zombie *zombies, int *zombie_count, int *enemy_count, joypad_buttons_t btn) {
    const float ZOMBIE_HIT_RADIUS2 = 50.0f;

    // Spawn bullet if B is pressed
    if (btn.b) {
        for (uint32_t i = 0; i < bs->max_count; i++) {
            if (!bs->bullets[i].active) {
                bs->bullets[i].position = *origin;
                bs->bullets[i].direction.v[0] = sinf(rot_y);
                bs->bullets[i].direction.v[1] = 0;
                bs->bullets[i].direction.v[2] = cosf(rot_y);
                bs->bullets[i].active = true;
                break; // fire one bullet per press
            }
        }
    }

    for (uint32_t i = 0; i < bs->max_count; i++) {
        if (!bs->bullets[i].active) continue;

        Bullet *b = &bs->bullets[i];

        // Move bullet
        b->position.v[0] += b->direction.v[0] * bs->speed;
        b->position.v[2] += b->direction.v[2] * bs->speed;

        // Out of bounds check
        if (b->position.v[0] < -box_limit || b->position.v[0] > box_limit ||
            b->position.v[2] < -box_limit || b->position.v[2] > box_limit) {
            b->active = false;
            continue;
        }

        // Check zombie collisions
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
                b->active = false;
                break;
            }
        }

        // Update matrix for this bullet
        T3DVec3 scale = {{0.8f, 0.8f, 0.8f}};
        T3DVec3 rotation = {{0.0f, 0.0f, 0.0f}};
        t3d_mat4fp_from_srt_euler(&bs->matrices[i], scale.v, rotation.v, b->position.v);
    }
}

void bullet_draw(BulletSystem *bs) {
    rdpq_set_env_color((color_t){0xFF, 0xFF, 0xFF, 0xFF});
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();
    rdpq_mode_zbuf(true, true);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_combiner(RDPQ_COMBINER1((PRIM, 0, ENV, 0), (0, 0, 0, 1)));

    tpx_state_from_t3d();
    tpx_state_set_scale(bs->size_x, bs->size_y);

    for (uint32_t i = 0; i < bs->max_count; i++) {
        if (!bs->bullets[i].active) continue;
        tpx_matrix_push(&bs->matrices[i]);
        tpx_particle_draw(&bs->buffer[i], 2); // 2 for two-particle mode
        tpx_matrix_pop(1);
    }
}

void bullet_cleanup(BulletSystem *bs) {
    free(bs->bullets);
    free(bs->buffer);
    free(bs->matrices);
}

*/
