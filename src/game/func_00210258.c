#include "kf_collision.h"

float sqrtf(float);

/* func_00210088 with the edge made longer by `reach` at both ends: tests an upright cylinder (axis point
   `centre`, `radius` high below it) against the wall edge from `start` to `end`, `height` high below it. */
void func_00210258(Vector4 *start, Vector4 *end, f32 height, Vector4 *centre, f32 radius, f32 reach) {
    f32 edge_x;
    f32 edge_z;
    f32 to_x;
    f32 to_z;
    f32 side;
    f32 along;
    f32 margin;
    f32 upper;
    f32 near_x;
    f32 near_z;
    f32 away_x;
    f32 away_z;
    f32 distance;

    COLLISION_RESULT->hit = 0;
    if (centre->y < start->y - height) {
        return;
    }
    if (start->y < centre->y - radius) {
        return;
    }
    edge_x = end->x - start->x;
    edge_z = end->z - start->z;
    to_z = centre->z - start->z;
    to_x = centre->x - start->x;
    side = edge_x * to_z - edge_z * to_x;
    if (side < 0.0f) {
        return;
    }
    along = to_x * edge_x + to_z * edge_z;
    along /= edge_x * edge_x + edge_z * edge_z;
    margin = -reach / sqrtf(edge_x * edge_x + edge_z * edge_z);
    upper = 1.0 - margin;
    if (along < margin) {
        return;
    }
    if (along > upper) {
        return;
    }
    near_x = start->x + edge_x * along;
    near_z = start->z + edge_z * along;
    away_x = centre->x - near_x;
    away_z = centre->z - near_z;
    distance = sqrtf(away_x * away_x + away_z * away_z);
    if (distance > reach) {
        return;
    }
    COLLISION_RESULT->point[0] = near_x;
    COLLISION_RESULT->point[1] = start->y;
    COLLISION_RESULT->point[2] = near_z;
    COLLISION_RESULT->normal[0] = away_x / distance;
    COLLISION_RESULT->normal[1] = 0.0f;
    COLLISION_RESULT->normal[2] = away_z / distance;
    COLLISION_RESULT->hit = 1;
}
