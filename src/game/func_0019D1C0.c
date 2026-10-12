#include "kf_player.h"

extern PlayerState D_00413F80;
extern u8 D_00349682[];
extern int D_003EF5F8[];
float atan2f(float, float);
float func_0019C640(float *, float *, float *);
void func_0019C598(float, float *, float *);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Plays a sound panned and scaled by where the point lies relative to the player, if it is within range. */
int func_0019D1C0(float range, int sound, float *position) {
    float origin[4];
    float offset[4];
    float diff[4];
    float distance;
    float angle;
    int volume;
    int pan_pos;
    int slot;

    distance = func_0019C640(diff, position, &D_00413F80.position.x);
    if (!(range < distance)) {
        angle = atan2f(position[0] - D_00413F80.position.x, position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
        angle -= (int)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        func_0019C598(angle, origin, offset);
        volume = (range - distance) * 64.0f * (1.0f / range);
        if (!((float) volume < 0.0f)) {
            pan_pos = (offset[0] + 1.0f) * 63.0f;
            volume = (float) volume * (float) D_00349682[0] * 0.015625f;
            if (volume >= 128) {
                volume = 127;
            }
            slot = func_0019C688(3);
            func_001CE690(slot, D_003EF5F8[0], 0, sound, 0x3C, volume, 0, (char) pan_pos);
            return slot;
        }
    }
}
