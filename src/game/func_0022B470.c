#include "common.h"
#include <libdev_internal.h>

int func_0022B060(void);
void func_0022B090(u16, LibdevQuadword);

// libdev: sceDevVu0PutCtx in libdev_internal.h (name inferred). Writes VU1's registers, which VU0 sees in
// its data memory from quadword 0x400 on; fails while either VU is running.
int func_0022B470(LibdevVu1Registers *registers) {
    LibdevQuadword quadword;
    u32 i;

    if (func_0022B060()) {
        return FALSE;
    }
    for (i = 0; i < 32; i++) {
        *(LibdevUint128 *)&quadword = registers->vector[i];
        func_0022B090(i + 0x400, quadword);
    }
    *(LibdevUint128 *)&quadword = 0;
    quadword.dword[0] = registers->status;
    func_0022B090(0x430, quadword);
    quadword.dword[0] = registers->mac;
    func_0022B090(0x431, quadword);
    quadword.dword[0] = registers->clipping;
    func_0022B090(0x432, quadword);
    quadword.dword[0] = registers->random;
    func_0022B090(0x434, quadword);
    quadword.dword[0] = registers->imaginary;
    func_0022B090(0x435, quadword);
    quadword.dword[0] = registers->quotient;
    func_0022B090(0x436, quadword);
    quadword.dword[0] = registers->pressure;
    func_0022B090(0x437, quadword);
    for (i = 0; i < 16; i++) {
        quadword.dword[0] = registers->integer[i];
        func_0022B090(i + 0x420, quadword);
    }
    return TRUE;
}
