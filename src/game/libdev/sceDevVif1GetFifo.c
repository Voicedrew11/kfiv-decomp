#include "common.h"
#include <libdev_internal.h>

typedef u32 u_long128 __attribute__((mode(TI)));

// libdev: Reads quadwords from the VIF1 FIFO while it holds any;
// returns how many were read.
int sceDevVif1GetFifo(u_long128 *destination, int quadword_count) {
    int read_count = 0;

    while (quadword_count--) {
        if (!(LIBDEV_REG32(VIF1_STAT) & VIF_STAT_FIFO_COUNT_MASK)) {
            return read_count;
        }
        *destination++ = *(volatile u_long128 *)VIF1_FIFO;
        read_count++;
    }
    return read_count;
}
