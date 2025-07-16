// slayer.c
#include "slayer.h"
#include <float.h>
#include "collision.h"

// Initial conditions of the player
void player_init(Player *player) {
    
    // Slayer Positions
    player->move_dir = (T3DVec3){{0, 0, 0}};
    player->speed = 0.0f;
    player->blend_factor = 0.0f;
    player->model_matrix  = malloc_uncached(sizeof(T3DMat4FP));

    // Slayer Models
    player->model = t3d_model_load("rom:/slayer.t3dm");
    player->model_shadow = t3d_model_load("rom:/shadow.t3dm");

    // Slayer Skeletons for model
    player->skel = t3d_skeleton_create(player->model);
    player->skel_blend = t3d_skeleton_clone(&player->skel, false);

    // Slayer Animations
    player->anim_idle = t3d_anim_create(player->model, "breathing-idle");
    t3d_anim_attach(&player->anim_idle, &player->skel);

    player->anim_walk = t3d_anim_create(player->model, "walking-left");
    t3d_anim_attach(&player->anim_walk, &player->skel_blend);

    // Optional: setup attack animation
    // player->anim_attack = t3d_anim_create(model, "attack");
    // t3d_anim_set_looping(&player->anim_attack, false);
    // t3d_anim_set_playing(&player->anim_attack, false);
    // t3d_anim_attach(&player->anim_attack, &player->skel);
}


void player_draw(Player *player) {
    t3d_mat4fp_from_srt_euler(player->model_matrix,
        // Scale
        (float[3]){0.0035f, 0.0035f, 0.0035f},
        // Rotation
        (float[3]){
                    -4.5f,              // Rotate around the X axis
                    0,                  // Rotate around Z axis
                    -player->rotation_y // Rotate around Y axis
                  },
        // Position
        player->position.v
    );

    t3d_matrix_push(player->model_matrix);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw_skinned(player->model, &player->skel);

    rdpq_set_prim_color(RGBA32(0, 0, 0, 120));
    t3d_model_draw(player->model_shadow);
    t3d_matrix_pop(1);
}

/*
void player_face_nearest_enemy(Player *player, Zombie *zombies, int zombie_count, joypad_inputs_t joypad) {
    float min_dist2 = FLT_MAX;
    T3DVec3 *closest = NULL;

    for (int i = 0; i < zombie_count; i++) {
        float dx = zombies[i].position.v[0] - player->position.v[0];
        float dz = zombies[i].position.v[2] - player->position.v[2];
        float dist2 = dx * dx + dz * dz;

        if (dist2 < min_dist2 && dist2 <= 10000.0f) {  // 10 units squared = 100
            min_dist2 = dist2;
            closest = &zombies[i].position;
        }
    }

    if (closest) {
        float dx = closest->v[0] - player->position.v[0];
        float dz = closest->v[2] - player->position.v[2];
        player->rotation_y = atan2f(dx, dz);
    } else {
        // Fall back to joystick direction
        float stick_x = (float)joypad.stick_x;
        float stick_y = (float)joypad.stick_y;

        if (stick_x != 0 || stick_y != 0) {
            float joy_angle = atan2f(stick_x, -stick_y);
            player->rotation_y = joy_angle;
        }
    }
}
    */

void draw_player_health_bar(const Player *player, T3DViewport *viewport) {

    // Offset above the slayer's head
    T3DVec3 world_pos = player->position;
    world_pos.v[1] += 45.0f; // raise it above the head (50 is good if camera is behind player)

    T3DVec3 screen_pos;
    t3d_viewport_calc_viewspace_pos(viewport, &screen_pos, &world_pos);

    const float width = 30.0f;
    const float height = 4.0f;
    // No current player health
    //float health_pct = (float)z->health / 5;//ZOMBIE_MAX_HEALTH;

    float x0 = screen_pos.v[0] - width / 2.0f;
    float y0 = screen_pos.v[1];

    // Background bar
    rdpq_set_mode_fill(RGBA32(50, 50, 50, 255)); // background
    rdpq_fill_rectangle(x0, y0, x0 + width, y0 + height);

    // Health Bar
    //rdpq_set_mode_fill(RGBA32(255 * (1.0f - health_pct), 255 * health_pct, 0, 255));
    //rdpq_fill_rectangle(x0, y0, x0 + width * health_pct, y0 + height);

    // Damage numbers (needs more work)
    //rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, x0 + width / 2.0f - 4, y0 - 10, "%d", 1);
}

void player_update(Player *player, float delta_time, joypad_inputs_t joypad, joypad_buttons_t buttons, Zombie *zombies, int zombie_count) {
    T3DVec3 new_dir = {{
        (float)joypad.stick_x * 0.05f,
        0,
       -(float)joypad.stick_y * 0.05f
    }};

    float speed = sqrtf(t3d_vec3_len2(&new_dir));

    if (speed > 0.15f) {
        new_dir.v[0] /= speed;
        new_dir.v[2] /= speed;
        player->move_dir = new_dir;

        float new_angle = atan2f(player->move_dir.v[0], player->move_dir.v[2]);
        player->rotation_y = t3d_lerp_angle(player->rotation_y, new_angle, 0.25f);
        player->speed = t3d_lerp(player->speed, speed * 0.15f, 0.15f);
    } else {
        player->speed *= 0.8f;
    }

    player->blend_factor = player->speed / 0.51f;
    if (player->blend_factor > 1.0f) player->blend_factor = 1.0f;

    // Simulate the proposed new position
    T3DVec3 proposed_pos = player->position;
    proposed_pos.v[0] += player->move_dir.v[0] * player->speed;
    proposed_pos.v[2] += player->move_dir.v[2] * player->speed;

    // Collision Check: prevent overlap with zombies
    bool blocked = false;
    for (int i = 0; i < zombie_count; i++) {
        if (!zombies[i].alive) continue;
        if (check_overlap(&proposed_pos, &zombies[i].position, SLAYER_COLLISION_RADIUS)) {
            blocked = true;
            break;
        }
    }

    // Only move if not blocked (right not hard stop, maybe look into Sliding collision)
    if (!blocked) {
        player->position = proposed_pos;
    }

    // Limit player position inside the box
    const float BOX_SIZE = 140.0f;
    if (player->position.v[0] < -BOX_SIZE) player->position.v[0] = -BOX_SIZE;
    if (player->position.v[0] >  BOX_SIZE) player->position.v[0] =  BOX_SIZE;
    if (player->position.v[2] < -BOX_SIZE) player->position.v[2] = -BOX_SIZE;
    if (player->position.v[2] >  BOX_SIZE) player->position.v[2] =  BOX_SIZE;

    // Get nearast enemy (if within a diameter of 10)
    //player_face_nearest_enemy(player, zombies, zombie_count, joypad);

    // Update player matrix
    t3d_mat4fp_from_srt_euler(player->model_matrix,
    (float[3]){0.0035f, 0.0035f, 0.0035f},
    (float[3]){-4.5f, 0, -player->rotation_y},
    player->position.v);

    // Animation updates
    t3d_anim_update(&player->anim_idle, delta_time);
    t3d_anim_set_speed(&player->anim_walk, player->blend_factor + 0.15f);
    t3d_anim_update(&player->anim_walk, delta_time);

    t3d_skeleton_blend(&player->skel, &player->skel, &player->skel_blend, player->blend_factor);
    t3d_skeleton_update(&player->skel);
}

  // Cleanup Player
  void player_cleanup(Player *player) 
  {
    // Destroy animations first (they may reference skeletons)
    t3d_skeleton_destroy(&player->skel);
    t3d_skeleton_destroy(&player->skel_blend);
    // Then destroy skeletons
    t3d_anim_destroy(&player->anim_idle);
    t3d_anim_destroy(&player->anim_walk);
    // Free model matrix
    free_uncached(player->model_matrix);
    // Free models
    t3d_model_free(player->model);
    t3d_model_free(player->model_shadow);
    
  }
