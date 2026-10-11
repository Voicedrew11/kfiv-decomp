#include "common.h"

int func_001CBAE0();
extern char *D_004471D4;

int func_001CE510(u32 arg0) {
    char *entry;

    if (arg0 >= 5) {
        return -1;
    }
    entry = D_004471D4 + (arg0 * 0x10 + 0x200);
    if (*(int *)entry == 0) {
        return -1;
    }
    return func_001CBAE0(1, 0x2040, *(int *)(entry + 0xC)) == 0 ? 0 : -1;
}
