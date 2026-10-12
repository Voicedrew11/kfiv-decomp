#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a TRXREG: size of the rectangle to transfer. */
int func_0021F5C0(int packet, u32 width, u32 height) {
    return func_0021F098(packet, 0x52, ((u64)height << 32) | width);
}
