// level_update.h
#ifndef LEVEL_UPDATE_H
#define LEVEL_UPDATE_H

#include <stdbool.h>
#include "slayer.h"
#include "zombie.h"
#include "levels.h"
#include "banners.h"

bool level_update(Player *player, Zombie *zombies, int zombie_count, int *enemy_count, const LevelData **level_out);

#endif