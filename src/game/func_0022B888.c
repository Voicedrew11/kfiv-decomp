#include "common.h"
#include <libdev_internal.h>

typedef u32 u_long128 __attribute__((mode(TI)));

// libdev: sceDevVif0PutFifo (name inferred). Writes quadwords to the VIF0 FIFO until it reports full;
// returns how many were written.
int func_0022B888(u_long128 *source, int quadword_count) {
    int written = 0;

    while (quadword_count--) {
        // match: bit 28 is VIF1's "FIFO full" (16 quadwords); VIF0's count field ends at bit 27, so this never stops
        if (LIBDEV_REG32(VIF0_STAT) & 0x10000000) {
            return written;
        }
        *(volatile u_long128 *)VIF0_FIFO = *source++;
        written++;
    }
    return written;
}
