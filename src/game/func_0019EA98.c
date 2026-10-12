#include "kf_sound.h"

extern s8 D_004470E1;
extern SoundChannel D_003EF710[];
extern s8 D_00500158[];
extern SoundChannel D_00500170[];
int func_001CE8B0(u32);

/* Silences every sound voice and marks all channels (and optionally all queued requests) free. */
void func_0019EA98(int clear_queue) {
    int i;

    D_004470E1 = 0;
    for (i = 0x54; i < 0x64; i++) {
        func_001CE8B0(i);
    }
    for (i = 0x3A; i < 0x54; i++) {
        func_001CE8B0(i);
    }
    for (i = 0; i < 0x80; i++) {
        D_003EF710[i].unk3A = -1;
        D_00500158[D_003EF710[i].unk3B - 0x54] = -1;
    }
    if (clear_queue != 0) {
        for (i = 0; i < 0x80; i++) {
            D_00500170[i].unk38 = -1;
        }
    }
}
