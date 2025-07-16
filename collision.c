// collision.c
#include "collision.h"
#include <math.h>

bool check_overlap(const T3DVec3 *a, const T3DVec3 *b, float min_distance) {
    float dx = a->v[0] - b->v[0];
    float dz = a->v[2] - b->v[2];
    float dist2 = dx * dx + dz * dz;
    float min_dist2 = min_distance * min_distance;

    return dist2 < min_dist2;
}