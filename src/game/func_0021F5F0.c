#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a TRXDIR: starts a transfer. */
int func_0021F5F0(int packet, u32 direction) {
    return func_0021F098(packet, 0x53, (u64)direction);
}
