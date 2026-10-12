#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVu0GetVi in libdev_internal.h (name inferred). Copies VU0's status, MAC, clipping, R, I and Q
// registers and its sixteen integer registers into the register block; fails while VU0 is running.
int func_0022AE10(LibdevVu0Registers *registers) {
    u32 value;

    if (sceDevVu0IsRunning()) {
        return FALSE;
    }
    // match: plain (not volatile) asm statements; the scheduler moves the return value's load past the first
    __asm__("cfc2.ni %0, $vi16" : "=r"(value));
    registers->status = value;
    __asm__("cfc2.ni %0, $vi17" : "=r"(value));
    registers->mac = value;
    __asm__("cfc2.ni %0, $vi18" : "=r"(value));
    registers->clipping = value;
    __asm__("cfc2.ni %0, $vi20" : "=r"(value));
    registers->random = value;
    __asm__("cfc2.ni %0, $vi21" : "=r"(value));
    registers->imaginary = value;
    __asm__("cfc2.ni %0, $vi22" : "=r"(value));
    registers->quotient = value;
    __asm__("cfc2.ni %0, $vi0" : "=r"(value));
    registers->integer[0] = value;
    __asm__("cfc2.ni %0, $vi1" : "=r"(value));
    registers->integer[1] = value;
    __asm__("cfc2.ni %0, $vi2" : "=r"(value));
    registers->integer[2] = value;
    __asm__("cfc2.ni %0, $vi3" : "=r"(value));
    registers->integer[3] = value;
    __asm__("cfc2.ni %0, $vi4" : "=r"(value));
    registers->integer[4] = value;
    __asm__("cfc2.ni %0, $vi5" : "=r"(value));
    registers->integer[5] = value;
    __asm__("cfc2.ni %0, $vi6" : "=r"(value));
    registers->integer[6] = value;
    __asm__("cfc2.ni %0, $vi7" : "=r"(value));
    registers->integer[7] = value;
    __asm__("cfc2.ni %0, $vi8" : "=r"(value));
    registers->integer[8] = value;
    __asm__("cfc2.ni %0, $vi9" : "=r"(value));
    registers->integer[9] = value;
    __asm__("cfc2.ni %0, $vi10" : "=r"(value));
    registers->integer[10] = value;
    __asm__("cfc2.ni %0, $vi11" : "=r"(value));
    registers->integer[11] = value;
    __asm__("cfc2.ni %0, $vi12" : "=r"(value));
    registers->integer[12] = value;
    __asm__("cfc2.ni %0, $vi13" : "=r"(value));
    registers->integer[13] = value;
    __asm__("cfc2.ni %0, $vi14" : "=r"(value));
    registers->integer[14] = value;
    __asm__("cfc2.ni %0, $vi15" : "=r"(value));
    registers->integer[15] = value;
    return TRUE;
}
