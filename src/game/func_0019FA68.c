#include "common.h"

extern u8 D_00349682[];
void func_0019C598(float, float *, float *);
int func_0019C688(int);
void func_001CE690(int, int, int, char, int, char, s16, int);
int rand(void);
double sqrt(double);

void func_0019FA68(u8 sound, int bank, float level, float max_level, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;
    int slot;

    if (!(level < max_level) && sound != 0xFF) {
        func_0019C598(angle, origin, offset);
        volume = (1.0 - sqrt(level * level - (level - max_level) * (level - max_level)) / level) * 64.0;
        volume *= 1.5f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = func_0019C688(2);
        func_001CE690(slot, bank, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}
