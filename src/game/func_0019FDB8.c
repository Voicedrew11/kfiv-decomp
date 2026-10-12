#include "common.h"

extern u8 D_00349682[];
void func_0019C598(float, float *, float *);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

void func_0019FDB8(u8 sound, int bank, float level, float max_level, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int slot;

    if (!(level < max_level) && sound != 0xFF) {
        func_0019C598(angle, origin, offset);
        volume = (level - max_level) * 64.0f * (1.0f / level);
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        slot = func_0019C688(2);
        func_001CE690(slot, bank, 0, sound, 0x3C, volume, 0, (char) pan_pos);
    }
}
