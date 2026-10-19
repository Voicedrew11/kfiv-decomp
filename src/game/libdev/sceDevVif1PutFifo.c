#include "common.h"
#include <libdev_internal.h>

typedef u32 u_long128 __attribute__((mode(TI)));

// libdev: Writes quadwords to the VIF1 FIFO until it reports full;
// returns how many were written.
int sceDevVif1PutFifo(u_long128 *source, int quadword_count) {
    int written = 0;

    while (quadword_count--) {
        if (LIBDEV_REG32(VIF1_STAT) & 0x10000000) {
            return written;
        }
        *(volatile u_long128 *)VIF1_FIFO = *source++;
        written++;
    }
    return written;
}
