// zombie.c
#include "zombie.h"
#include "collision.h"

#define ATTACK_ANIM_LENGHT 2.9f
#define ATTACK_ANIM_READY 0.666666f
#define ATTACK_ANIM_FINISHING 2.266666f

void zombie_init(Zombie *z, const T3DVec3 *start_pos) {
    
    // Zombie Positions
    z->position = *start_pos;
    z->rotation_y = 0.0f;
    z->speed = 0.3f;

    // Zombie Health
    z->health = 5;
    z->alive = true;

    // Zombie Death 
    z ->blood_time = 0;
    z ->blood_scale = 0.3f;

    z->model = t3d_model_load("rom:/zombie.t3dm"); 
    z->model_matrix = malloc_uncached(sizeof(T3DMat4FP));

    // Zombie Skeletons for model
    z->skel = t3d_skeleton_create(z->model);
    z->skelBlend = t3d_skeleton_clone(&z->skel, false); // optimized for blending, has no matrices
    
    // Zombie Animations
    // Attach walk animation
    z->anim_walk = t3d_anim_create(z->model, "walking"); 
    t3d_anim_attach(&z->anim_walk, &z->skel);  // main skeleton

    // Attach attack animation
    z->anim_attack = t3d_anim_create(z->model, "punching-left");
    t3d_anim_set_looping(&z->anim_attack, false); // don't loop this animation
    t3d_anim_set_playing(&z->anim_attack, false); // start in a paused state
    t3d_anim_attach(&z->anim_attack, &z->skelBlend);
    z->is_attacking = false;
    z->attack_timer = 0.0f;
    z->attack_blending_ratio = 0.0f;
}

void zombie_update(Zombie *z, const T3DVec3 *player_pos, float delta_time, Zombie *zombies, int count) {

    float dx = player_pos->v[0] - z->position.v[0];
    float dz = player_pos->v[2] - z->position.v[2];

    // Calculate distance between player and zombie (Pythagorean theorem)
    float dist = sqrtf(dx * dx + dz * dz);

    bool can_move = false;

    if (dist > 1.0f) {
        dx /= dist;
        dz /= dist;

        // Simulate new position
        T3DVec3 next_pos = z->position;
        next_pos.v[0] += dx * z->speed;
        next_pos.v[2] += dz * z->speed;

        // Check collision with player
        if (!check_overlap(&next_pos, player_pos, ZOMBIE_COLLISION_RADIUS)) {
            // Check collision with other zombies
            can_move = true;
            for (int i = 0; i < count; i++) {
                Zombie *other = &zombies[i];
                if (other == z || other->health <= 0) continue;
                if (check_overlap(&next_pos, &other->position, ZOMBIE_COLLISION_RADIUS)) {
                    can_move = false;
                    break;
                }
            }

            // Apply movement only if not blocked
            if (can_move) {
                z->position = next_pos;
                z->rotation_y = atan2f(dx, dz);
            }
        }
    }

    // Trigger attack animation if within range
    if (dist <= ZOMBIE_COLLISION_RADIUS + 10 && !z->is_attacking && !z->anim_attack.isPlaying) {
        t3d_anim_set_time(&z->anim_attack, 0.0f);
        t3d_anim_set_playing(&z->anim_attack, true);
        z->is_attacking = true;
        z->attack_timer = 0.0f;
        z->attack_blending_ratio = 0.0f;
    }

    // Update animations
    if (z->is_attacking) {

        if (z->anim_attack.time < ATTACK_ANIM_READY && z->attack_blending_ratio < 1.0f){
            
            z->attack_blending_ratio += delta_time / ATTACK_ANIM_READY; 
            if (z->attack_blending_ratio > 1.0f) z->attack_blending_ratio = 1.0f;
        }
        
        if (z->anim_attack.time > ATTACK_ANIM_FINISHING){
            
            z->attack_blending_ratio -= delta_time / (ATTACK_ANIM_LENGHT - ATTACK_ANIM_FINISHING); 
            if (z->attack_blending_ratio < 0.0f) z->attack_blending_ratio = 0.0f;
        }
        
        t3d_anim_update(&z->anim_walk, delta_time);
        t3d_anim_update(&z->anim_attack, delta_time);
        t3d_skeleton_blend(&z->skel, &z->skel, &z->skelBlend, z->attack_blending_ratio);
        
        if (!z->anim_attack.isPlaying) {
            
            z->is_attacking = false;
        }
    }

    else if (can_move) {
        // Only update walk animation if zombie is walking
        t3d_anim_update(&z->anim_walk, delta_time);
    }

    // Final matrix build
    t3d_skeleton_update(&z->skel);
    t3d_mat4fp_from_srt_euler(z->model_matrix,
        (float[3]){0.0030f, 0.0030f, 0.0030f},
        (float[3]){-4.5f, 0.0f, -z->rotation_y},
        z->position.v
    );

}

void draw_zombie_health_bar(const Zombie *z, T3DViewport *viewport) {
    if (!z->alive || z->health <= 0) return;

    // Offset above the zombie's head
    T3DVec3 world_pos = z->position;
    world_pos.v[1] += 60.0f; // raise it above the head (50 is good if camera is behind player)

    T3DVec3 screen_pos;
    t3d_viewport_calc_viewspace_pos(viewport, &screen_pos, &world_pos);

    const float width = 30.0f;
    const float height = 4.0f;
    float health_pct = (float)z->health / 5;//ZOMBIE_MAX_HEALTH;

    float x0 = screen_pos.v[0] - width / 2.0f;
    float y0 = screen_pos.v[1];

    // Background bar
    rdpq_set_mode_fill(RGBA32(50, 50, 50, 255)); // background
    rdpq_fill_rectangle(x0, y0, x0 + width, y0 + height);

    // Health Bar
    rdpq_set_mode_fill(RGBA32(255 * (1.0f - health_pct), 255 * health_pct, 0, 255));
    rdpq_fill_rectangle(x0, y0, x0 + width * health_pct, y0 + height);

    // Damage numbers (needs more work)
    //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, x0 + width / 2.0f - 4, y0 - 10, "%d", 1);
}

void zombie_draw(Zombie *z) {
    t3d_matrix_push(z->model_matrix);
    rdpq_set_prim_color(RGBA32(255, 0, 0, 255));
    t3d_model_draw_skinned(z->model, &z->skel); 
    t3d_matrix_pop(1);
}

void zombie_destroy(Zombie *z) {
    // Destroy animations first (they may reference skeletons)
    t3d_anim_destroy(&z->anim_walk); 
    // Then destroy skeletons
    t3d_skeleton_destroy(&z->skel);
    // Free model matrix
        free_uncached(z->model_matrix);
            // Free model
    t3d_model_free(z->model);
}

// ----- Multiple Zombie Helpers -----

void zombie_update_all(Zombie *zombies, int count, const T3DVec3 *player_pos, float delta_time) {
    for (int i = 0; i < count; i++) {
        if (!zombies[i].alive) continue;
        zombie_update(&zombies[i], player_pos, delta_time, zombies, count);
    }
}

void zombie_draw_all(Zombie *zombies, int count) {
    for (int i = 0; i < count; i++) {
        if (!zombies[i].alive) continue;
        zombie_draw(&zombies[i]);
    }
}

void zombie_destroy_all(Zombie *zombies, int count) {
    for (int i = 0; i < count; i++) {
        zombie_destroy(&zombies[i]);
    }
    free_uncached(zombies); 
}
