#include "common.h"

float sqrtf(float);
float atan2f(float, float);
char *func_0010CAB8(char *, u32);
int func_001A1C58(int, void *, void *, void *);

/* Spawns an effect at a model point, facing along the line to the next point. */
void func_00195EF8(char **model, int effect_id, int point) {
    f32 *from;
    f32 *tip;
    f32 angles[4];
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 yaw;
    f32 turns;

    from = (f32 *)func_0010CAB8(*model, point);
    tip = (f32 *)func_0010CAB8(*model, point + 1);
    if (from != NULL && tip != NULL) {
        delta_x = tip[0] - from[0];
        delta_y = tip[1] - from[1];
        delta_z = tip[2] - from[2];
        angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
        yaw = -atan2f(delta_z, delta_x) + 1.5707964f;
        turns = (int)(yaw / 6.2831855f);
        yaw -= turns * 6.2831855f;
        if (3.1415927f < yaw) {
            yaw -= 6.2831855f;
        } else if (yaw < -3.1415927f) {
            yaw += 6.2831855f;
        }
        angles[1] = yaw;
        angles[2] = angles[3] = 0.0f;
        func_001A1C58(effect_id, from, angles, model);
    }
}
