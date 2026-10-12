#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a TRXPOS: destination corner of a transfer. */
int func_0021F590(int packet, u32 direction, u32 dest_x, u32 dest_y) {
    return func_0021F098(packet, 0x51, ((u64)direction << 59) | ((u64)dest_y << 48) | ((u64)dest_x << 32));
}
