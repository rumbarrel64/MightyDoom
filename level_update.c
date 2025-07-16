// level_update.c
#include "level_update.h"


// Goal box constants
#define GOAL_MIN_X   -30.0f
#define GOAL_MAX_X   30.0f
#define GOAL_MIN_Y   -140.0f
#define GOAL_MAX_Y   -130.0f
#define RESET_FRAMES 15

// Add all levels here
static const LevelData *levels[] = {
    &LEVEL_1,
    &LEVEL_2
};
#define TOTAL_LEVELS (sizeof(levels) / sizeof(LevelData*))

static int current_level_index = 0;

bool level_update(Player *player, Zombie *zombies, int zombie_count, int *enemy_count, const LevelData **level_out) {
    static bool reset_pending = false;
    static int reset_timer = 0;

    // Check win condition
    if (!reset_pending && *enemy_count == 0 &&
        player->position.v[0] >= GOAL_MIN_X && player->position.v[0] <= GOAL_MAX_X &&
        player->position.v[2] >= GOAL_MIN_Y && player->position.v[2] <= GOAL_MAX_Y) {
        reset_pending = true;
        reset_timer = RESET_FRAMES;
    }

    if (reset_pending) {
        if (reset_timer > 0) {
            rdpq_attach(display_get(), display_get_zbuf());
            rdpq_set_mode_fill(RGBA32(0, 0, 0, 255));
            rdpq_fill_rectangle(0, 0, 320, 240);
            rdpq_detach_show();
            reset_timer--;
            return true; // Skip game frame while black screen
        } else {
            // Progress to next level (loop back to 0 after last)
            current_level_index = (current_level_index + 1) % TOTAL_LEVELS;
            const LevelData *level = levels[current_level_index];
            *level_out = level;

            // Reset player
            player->position = level->player.position;
            player->rotation_y = level->player.rotation_y;
            player->speed = 0.0f;
            player->move_dir = (T3DVec3){{0,0,0}};

            // Reset enemies
            *enemy_count = level->zombie_count;
            for (int i = 0; i < level->zombie_count; i++) {
                zombies[i].position = level->zombies[i].position;
                zombies[i].rotation_y = level->zombies[i].rotation_y;
                zombies[i].health = 5;
                zombies[i].alive = true;
                zombies[i].blood_scale = 0.3f;
            }

            reset_pending = false;
        }
    }

    return false; // Continue regular game frame
}