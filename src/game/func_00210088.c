#include "kf_collision.h"

float sqrtf(float);

/* Tests an upright cylinder (axis point `centre`, `radius` high below it) against the wall edge from
   `edge_start` to `edge_end`, which is `height` high below its points: a hit when the axis is on the
   front side of the edge, beside it, and no further than `reach` from it. */
void func_00210088(void *edge_start, void *edge_end, f32 height, void *centre, f32 radius, f32 reach) {
    f32 *start = edge_start;
    f32 *end = edge_end;
    f32 *point = centre;
    f32 edge_x;
    f32 edge_z;
    f32 to_x;
    f32 to_z;
    f32 side;
    f32 along;
    f32 near_x;
    f32 near_z;
    f32 away_x;
    f32 away_z;
    f32 distance;

    COLLISION_RESULT->hit = 0;
    if (point[1] < start[1] - height) {
        return;
    }
    if (start[1] < point[1] - radius) {
        return;
    }
    edge_x = end[0] - start[0];
    edge_z = end[2] - start[2];
    to_z = point[2] - start[2];
    to_x = point[0] - start[0];
    side = edge_x * to_z - edge_z * to_x;
    if (side < 0.0f) {
        return;
    }
    // match: retail tests the same thing a second time
    if (side < 0.0f) {
        return;
    }
    along = to_x * edge_x + to_z * edge_z;
    along /= edge_x * edge_x + edge_z * edge_z;
    if (along < 0.0f) {
        return;
    }
    if (along > 1.0f) {
        return;
    }
    near_x = start[0] + edge_x * along;
    near_z = start[2] + edge_z * along;
    away_x = point[0] - near_x;
    away_z = point[2] - near_z;
    distance = sqrtf(away_x * away_x + away_z * away_z);
    if (distance > reach) {
        return;
    }
    COLLISION_RESULT->point[0] = near_x;
    COLLISION_RESULT->point[1] = start[1];
    COLLISION_RESULT->point[2] = near_z;
    COLLISION_RESULT->normal[0] = away_x / distance;
    COLLISION_RESULT->normal[1] = 0.0f;
    COLLISION_RESULT->normal[2] = away_z / distance;
    COLLISION_RESULT->hit = 1;
}
