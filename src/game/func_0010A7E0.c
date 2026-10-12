#include "common.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

/* Queue a packet carrying one 4x4 matrix. */
void func_0010A7E0(void *matrix) {
    u32 *packet_start;
    u32 *packet;

    packet_start = packet = func_00109990(7);
    *packet++ = 0x10000005;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C040000;
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "lqc2 $vf17, 0x10(%0)\n"
        "lqc2 $vf18, 0x20(%0)\n"
        "lqc2 $vf19, 0x30(%0)\n"
        "sqc2 $vf16, 0x0(%1)\n"
        "sqc2 $vf17, 0x10(%1)\n"
        "sqc2 $vf18, 0x20(%1)\n"
        "sqc2 $vf19, 0x30(%1)\n"
        : : "r"(matrix), "r"(packet));
    packet += 0x10;
    func_00109650(packet_start, packet);
}
