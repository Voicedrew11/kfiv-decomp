#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVu0PutMem (name inferred). Stores one quadword at a VU0 data memory address, through VF1 and
// VI1, which it saves and restores.
void func_0022B090(u16 address, LibdevQuadword quadword) {
    u32 saved_index;
    LibdevUint128 saved_vector;
    LibdevUint128 value;

    // match: the two saves are plain (not volatile) asm statements, so the scheduler moves them into the copy
    __asm__("cfc2.ni %0, $vi1" : "=r"(saved_index));
    __asm__("qmfc2.ni %0, $vf1" : "=r"(saved_vector));
    value = *(LibdevUint128 *)&quadword;
    __asm__ __volatile__("qmtc2.ni %0, $vf1" : : "r"(value));
    LIBDEV_CTC2(address, vi1);
    __asm__ __volatile__("vsqi.xyzw $vf1, ($vi1++)");
    LIBDEV_CTC2(saved_index, vi1);
    __asm__ __volatile__("qmtc2.ni %0, $vf1" : : "r"(saved_vector));
}
