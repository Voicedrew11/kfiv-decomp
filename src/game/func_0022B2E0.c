#include "common.h"
#include <libdev_internal.h>

int func_0022B060(void);
// match: sceDevVu0GetMem returns the quadword by value; libdev_internal.h spells out the hidden pointer
#define GET_MEM(address) ((LibdevQuadword (*)(u16))sceDevVu0GetMem)(address)

// libdev: sceDevVu0GetCtx in libdev_internal.h (name inferred). Reads VU1's registers, which VU0 sees in
// its data memory from quadword 0x400 on; fails while either VU is running.
int func_0022B2E0(LibdevVu1Registers *registers) {
    LibdevQuadword status;
    LibdevQuadword mac;
    LibdevQuadword clipping;
    LibdevQuadword random;
    LibdevQuadword imaginary;
    LibdevQuadword quotient;
    LibdevQuadword pressure;
    LibdevQuadword integer;
    u32 i;

    if (func_0022B060()) {
        return FALSE;
    }
    for (i = 0; i < 32; i++) {
        status = GET_MEM(i + 0x400);
        registers->vector[i] = *(LibdevUint128 *)&status;
    }
    status = GET_MEM(0x430);
    registers->status = status.word[0];
    mac = GET_MEM(0x431);
    registers->mac = mac.word[0];
    clipping = GET_MEM(0x432);
    registers->clipping = clipping.word[0];
    random = GET_MEM(0x434);
    registers->random = random.word[0];
    imaginary = GET_MEM(0x435);
    registers->imaginary = imaginary.word[0];
    quotient = GET_MEM(0x436);
    registers->quotient = quotient.word[0];
    pressure = GET_MEM(0x437);
    registers->pressure = pressure.word[0];
    for (i = 0; i < 16; i++) {
        integer = GET_MEM(i + 0x420);
        registers->integer[i] = integer.half[0];
    }
    return TRUE;
}
