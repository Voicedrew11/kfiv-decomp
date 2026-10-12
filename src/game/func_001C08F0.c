#include "common.h"

extern s32 D_0044717C;
extern s32 D_00447184;
extern s32 D_0057DF40[64];

/* ages every queued trail point and drops the expired ones from the front */
void func_001C08F0(void) {
    s32 i;
    s32 slot;
    s32 count = D_0044717C;

    for (i = 0; i < count; i++) {
        slot = (D_00447184 + i) % 64;
        D_0057DF40[slot]--;
        if (D_0057DF40[slot] <= 0) {
            D_0044717C--;
            D_00447184 = (D_00447184 + 1) % 64;
        }
    }
}
