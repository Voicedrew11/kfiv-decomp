#include "common.h"
#include "kf_sound.h"

extern SoundChannel D_003EF710[];
int func_001CE9A0(int, u32, int);

void func_0019E8B0(void) {
    SoundChannel *entry = D_003EF710;
    int n = 0x7F;

    do {
        n--;
        if (entry->unk3A >= 0 && entry->unk3C == 0x63) {
            func_001CE9A0(0, entry->unk3B, 0);
        }
        entry++;
    } while (n >= 0);
}
