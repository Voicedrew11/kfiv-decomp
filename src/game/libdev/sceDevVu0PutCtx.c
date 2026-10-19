#include "common.h"
#include <libdev_internal.h>


// libdev: Writes VU1's registers, which VU0 sees in
// its data memory from quadword 0x400 on; fails while either VU is running.
int sceDevVu0PutCtx(LibdevVu1Registers *registers) {
    LibdevQuadword quadword;
    u32 i;

    if (sceDevVu0ChkIdle()) {
        return FALSE;
    }
    for (i = 0; i < 32; i++) {
        *(LibdevUint128 *)&quadword = registers->vector[i];
        sceDevVu0PutMem(i + 0x400, quadword);
    }
    *(LibdevUint128 *)&quadword = 0;
    quadword.dword[0] = registers->status;
    sceDevVu0PutMem(0x430, quadword);
    quadword.dword[0] = registers->mac;
    sceDevVu0PutMem(0x431, quadword);
    quadword.dword[0] = registers->clipping;
    sceDevVu0PutMem(0x432, quadword);
    quadword.dword[0] = registers->random;
    sceDevVu0PutMem(0x434, quadword);
    quadword.dword[0] = registers->imaginary;
    sceDevVu0PutMem(0x435, quadword);
    quadword.dword[0] = registers->quotient;
    sceDevVu0PutMem(0x436, quadword);
    quadword.dword[0] = registers->pressure;
    sceDevVu0PutMem(0x437, quadword);
    for (i = 0; i < 16; i++) {
        quadword.dword[0] = registers->integer[i];
        sceDevVu0PutMem(i + 0x420, quadword);
    }
    return TRUE;
}
