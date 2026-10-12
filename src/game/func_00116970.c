#include "kf_gs.h"

void func_00109F80(s64, s64, s64);

/* Points both drawing contexts at the frame buffer with a 256x256 scissor and no offset bias. */
void func_00116970(void) {
    func_00109F80(0xA04015E, 0xFF000000FF0000L, 0x780000007800L);
}
