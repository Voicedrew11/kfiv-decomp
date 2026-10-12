#include "kf_gs.h"

extern int D_00446F78;

void func_00109F80(s64, s64, s64);

void func_0010A008(void) {
    u32 frame_base = 0x1180;

    if (D_00446F78 != 0) {
        frame_base = 0;
    }
    func_00109F80((frame_base >> 5) | 0xA0000, 0x1BF0000027F0000L, 0x720000006C00L);
}
