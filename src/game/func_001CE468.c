#include "common.h"

int func_001CBAE0();
extern char *D_004471D4;

int func_001CE468(u32 arg0, int arg1) {
    char *entry;

    if (*(u16 *)(D_004471D4 + 0x4D6) == 0) {
        return -1;
    }
    if (arg0 >= 5) {
        return -1;
    }
    entry = D_004471D4 + (arg0 * 0x10 + 0x200);
    if (*(int *)entry == 0) {
        return -1;
    }
    return func_001CBAE0(1, 0x2030, *(int *)(entry + 0xC), arg1) == 0 ? 0 : -1;
}
