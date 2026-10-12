#include "common.h"

int func_001CDBC0(u32, s16);
extern s32 D_003EF5C8[];

/* Clamps the level to 0..0x7F and passes it on with the global as the first argument. */
void func_0019C7F0(s32 level) {
    if (level < 0) {
        level = 0;
    }
    if (level > 0x7F) {
        level = 0x7F;
    }
    func_001CDBC0(D_003EF5C8[0], level);
}
