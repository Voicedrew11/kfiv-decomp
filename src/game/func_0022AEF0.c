#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVu0GetVf in libdev_internal.h (name inferred). Copies VU0's 32 vector registers into the
// register block, then loads VU0's status, MAC, clipping, R, I, Q and integer registers FROM the block.
int func_0022AEF0(LibdevVu0Registers *registers) {
    LibdevUint128 *vectors = registers->vector;
    u32 value;

    if (sceDevVu0IsRunning()) {
        return FALSE;
    }
    // match: sqc2 stores the vector registers; a function that restores the others was surely meant to load them
    __asm__ __volatile__("sqc2 $vf0, 0x0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf1, 0x10(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf2, 0x20(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf3, 0x30(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf4, 0x40(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf5, 0x50(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf6, 0x60(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf7, 0x70(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf8, 0x80(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf9, 0x90(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf10, 0xA0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf11, 0xB0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf12, 0xC0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf13, 0xD0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf14, 0xE0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf15, 0xF0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf16, 0x100(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf17, 0x110(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf18, 0x120(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf19, 0x130(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf20, 0x140(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf21, 0x150(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf22, 0x160(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf23, 0x170(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf24, 0x180(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf25, 0x190(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf26, 0x1A0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf27, 0x1B0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf28, 0x1C0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf29, 0x1D0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf30, 0x1E0(%0)" : : "r"(vectors));
    __asm__ __volatile__("sqc2 $vf31, 0x1F0(%0)" : : "r"(vectors));
    value = registers->status;
    LIBDEV_CTC2(value, vi16);
    value = registers->mac;
    LIBDEV_CTC2(value, vi17);
    value = registers->clipping;
    LIBDEV_CTC2(value, vi18);
    value = registers->random;
    LIBDEV_CTC2(value, vi20);
    value = registers->imaginary;
    LIBDEV_CTC2(value, vi21);
    value = registers->quotient;
    LIBDEV_CTC2(value, vi22);
    value = registers->integer[0];
    LIBDEV_CTC2(value, vi0);
    value = registers->integer[1];
    LIBDEV_CTC2(value, vi1);
    value = registers->integer[2];
    LIBDEV_CTC2(value, vi2);
    value = registers->integer[3];
    LIBDEV_CTC2(value, vi3);
    value = registers->integer[4];
    LIBDEV_CTC2(value, vi4);
    value = registers->integer[5];
    LIBDEV_CTC2(value, vi5);
    value = registers->integer[6];
    LIBDEV_CTC2(value, vi6);
    value = registers->integer[7];
    LIBDEV_CTC2(value, vi7);
    value = registers->integer[8];
    LIBDEV_CTC2(value, vi8);
    value = registers->integer[9];
    LIBDEV_CTC2(value, vi9);
    value = registers->integer[10];
    LIBDEV_CTC2(value, vi10);
    value = registers->integer[11];
    LIBDEV_CTC2(value, vi11);
    value = registers->integer[12];
    LIBDEV_CTC2(value, vi12);
    value = registers->integer[13];
    LIBDEV_CTC2(value, vi13);
    value = registers->integer[14];
    LIBDEV_CTC2(value, vi14);
    value = registers->integer[15];
    LIBDEV_CTC2(value, vi15);
    return TRUE;
}
