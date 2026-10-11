#include "common.h"

int func_001CBAE0();
extern char *D_004471D4;

int func_001CD5E0(int arg0, char arg1, u32 arg2) {
    char *temp_s0;
    int temp_v0;
    u32 temp_v1;

    temp_s0 = D_004471D4 + (arg0 << 5);
    temp_s0[0x1A] = arg1;
    if (arg2 < 0x5010) {
        return -1;
    }
    if ((u32) (arg2 + *(int *) (temp_s0 + 0x14)) > 0x1E7FBFU) {
        return -1;
    }
    *(u32 *) (temp_s0 + 0xC) = arg2;
    if (func_001CBAE0(1, 0x1010, *(s16 *) (temp_s0 + 0x18), arg2, *(int *) temp_s0, *(char *) (temp_s0 + 0x1B)) != 0) {
        return -1;
    }
    temp_v0 = func_001CBAE0(1, 0x90, *(char *) (temp_s0 + 0x1A), 0, *(int *) (temp_s0 + 0x8), *(int *) (temp_s0 + 0xC), *(int *) (temp_s0 + 0x14));
    if (temp_v0 != *(int *) (temp_s0 + 0x14)) {
        return -1;
    }
    temp_v1 = arg2 + temp_v0;
    if (*(u32 *) (D_004471D4 + 0x4C0) < temp_v1) {
        *(u32 *) (D_004471D4 + 0x4C0) = temp_v1;
    }
    return 0;
}
