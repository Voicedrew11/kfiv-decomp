#include "kf_player.h"

extern PlayerState D_00413F80;
extern u8 D_00349682[];
extern int D_003EF658[];
extern char D_004470E1;
double sqrt(double);
int rand(void);
float func_0019C640(float *, float *, float *);
void func_0019C598(float, float *, float *);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Plays a sound at a point, panned and scaled by its distance from the player and detuned at random. */
int func_0019CE30(u8 sound, float *position) {
    float origin[4];
    float offset[4];
    float direction[4];
    float diff[4];
    float distance;
    float falloff;
    float level;
    int volume;
    int pan;
    int detune;

    if (D_004470E1 == 0) {
        return -1;
    }
    distance = func_0019C640(diff, position, &D_00413F80.position.x);
    func_0019C598(D_00413F80.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    falloff = 1.0f - distance * distance * 0.0004f * 0.0004f;
    level = (int)(sqrt(falloff) * 64.0);
    if (level < 0.0f) {
        level = 0.0f;
    }
    volume = level * (float) D_00349682[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    pan = ((diff[0] * direction[0] + diff[1] * direction[1] + diff[2] * direction[2]) / distance + 1.0f) * 63.0f;
    detune = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    func_001CE690(func_0019C688(3), D_003EF658[0], 0, sound, 0x3C, volume, detune, (char) pan);
    return 1;
}
