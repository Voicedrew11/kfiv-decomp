#include "common.h"

extern u8 D_00369830[];

/* Takes one from the count at the index, if there is any. */
void func_001BC2A0(u32 index) {
    if (index < 0x17C && D_00369830[index] != 0) {
        D_00369830[index]--;
    }
}
