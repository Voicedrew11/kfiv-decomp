#include "common.h"

void func_001CFA70(int, int, int, int, int, int, ...);

void func_00123000(int sound_id) {
    if (sound_id >= 0x2F && sound_id < 0x3C) {
        func_001CFA70(0, 0, 0, 0, 0x2D, sound_id + 0x28);
    }
}
