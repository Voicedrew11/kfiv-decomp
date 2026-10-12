#include "kf_player.h"

extern PlayerState D_00413F80;
extern u8 D_00349682[];
extern int D_003EF658[];
extern char D_004470E1;
double sqrt(double);
float atan2f(float, float);
int rand(void);
float func_0019C640(float *, float *, float *);
void func_0019C598(float, float *, float *);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Plays a sound at a point with one of three volume levels by mode, panned toward it, if it is in range. */
int func_0019D490(u8 sound, u8 mode, float *position, float range) {
    float origin[4];
    float offset[4];
    float diff[4];
    float distance;
    float level;
    float angle;
    int volume;
    int detune;
    int pan;
    int slot;

    if (D_004470E1 == 0) {
        return -1;
    }
    distance = func_0019C640(diff, position, &D_00413F80.position.x);
    switch (mode) {
    case 0:
    case 6:
        volume = (range - distance) * 64.0f * (1.0f / range);
        break;
    case 1:
    case 7:
        level = range * range;
        level -= (range - distance) * (range - distance);
        volume = (int)((1.0 - sqrt(level) / range) * 64.0) * 1.5f;
        break;
    case 2:
    case 8:
        level = range * range;
        level -= distance * distance;
        volume = sqrt(level) / range * 64.0;
        break;
    }
    level = volume;
    if (level < 0.0f) {
        level = 0.0f;
    }
    volume = level * (float) D_00349682[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    angle = atan2f(position[0] - D_00413F80.position.x, position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    func_0019C598(angle, origin, offset);
    pan = (offset[0] + 1.0f) * 63.0f;
    if (mode < 3) {
        detune = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    } else {
        detune = 0;
    }
    slot = func_0019C688(3);
    if (volume != 0) {
        func_001CE690(slot, D_003EF658[0], 0, sound, 0x3C, volume, detune, (char) pan);
    }
    return 1;
}
