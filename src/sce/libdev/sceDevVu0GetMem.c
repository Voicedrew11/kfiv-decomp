/* libdev.a: sceDevVu0GetMem (KFIV: not in ICO; the name is inferred from what the
   code does, not taken from SDK headers) */
#include <libdev_internal.h>

void *sceDevVu0GetMem(void *dst, unsigned int addr)
{
    LibdevQuadword quad;
    LibdevUint128 saved_vector;
    LibdevUint128 value;
    unsigned int saved_index;

    // match: the two saves are plain (not volatile) asm statements, so the scheduler hoists them above the frame setup
    __asm__("cfc2.ni %0, $vi1" : "=r"(saved_index));
    __asm__("qmfc2.ni %0, $vf1" : "=r"(saved_vector));
    LIBDEV_CTC2((addr & 0xFFFF), vi1);
    __asm__ __volatile__("vlqi.xyzw $vf1, ($vi1++)");
    __asm__("qmfc2.ni %0, $vf1" : "=r"(value));
    LIBDEV_CTC2(saved_index, vi1);
    __asm__ __volatile__("qmtc2.ni %0, $vf1" : : "r"(saved_vector));
    *(LibdevUint128 *)&quad = value;
    *(LibdevQuadword *)dst = quad;
    return dst;
}
