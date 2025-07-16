// collision.h
#pragma once

#include "libs.h"

// Returns true if the two positions are within a blocking radius
bool check_overlap(const T3DVec3 *a, const T3DVec3 *b, float min_distance);