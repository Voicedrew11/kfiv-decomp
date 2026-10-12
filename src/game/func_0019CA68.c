#include "kf_player.h"

extern PlayerState D_00413F80;
extern u8 D_00349682[];
extern int D_003EF628[];
extern s32 D_00502170[][2];
double sqrt(double);
float func_0019C640(float *, float *, float *);
void func_0019C598(float, float *, float *);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Starts a positional sound in a free D_00502170 slot, panned and scaled by its distance from the player. */
int func_0019CA68(int sound, int unused, float *position) {
    float diff[4];
    float direction[4];
    float origin[4];
    float offset[4];
    float distance;
    float falloff;
    int volume;
    int pan;
    int slot;
    int tries;

    if (sound == 0) {
        return -1;
    }
    if (sound >= 0x80) {
        return -1;
    }
    tries = 0;
    distance = func_0019C640(diff, position, &D_00413F80.position.x);
    func_0019C598(D_00413F80.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    falloff = 1.0f - distance * distance * 0.0004f * 0.0004f;
    volume = sqrt(falloff) * 64.0;
    if (volume < 0) {
        volume = 0;
    }
    volume = (float) volume * (float) D_00349682[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    pan = ((diff[0] * direction[0] + diff[1] * direction[1] + diff[2] * direction[2]) / distance + 1.0f) * 63.0f;
    for (;;) {
        slot = func_0019C688(5);
        tries++;
        if (D_00502170[slot - 100][0] == 0) {
            break;
        }
        if (tries >= 0xB) {
            return -1;
        }
    }
    func_001CE690(slot, D_003EF628[0], 0, sound, 0x3C, volume, 0, (char) pan);
    D_00502170[slot - 100][0] = TRUE;
    D_00502170[slot - 100][1] = volume;
    return slot;
}
