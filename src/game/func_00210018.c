#include "kf_collision.h"

void func_00210088(void *, void *, f32, void *, f32, f32);

/* func_00210088 for a ball of radius `reach` at `centre`: tested as a cylinder twice the radius high
   from the ball's top; a hit is reported at the ball's own height. */
void func_00210018(void *edge_start, void *edge_end, f32 height, void *centre, f32 reach) {
    f32 *point = centre;
    f32 top[4];

    top[0] = point[0];
    top[1] = point[1] + reach;
    top[2] = point[2];
    COLLISION_RESULT->hit = 0;
    func_00210088(edge_start, edge_end, height, top, reach + reach, reach);
    if (COLLISION_RESULT->hit != 0) {
        COLLISION_RESULT->point[1] = point[1];
    }
}
