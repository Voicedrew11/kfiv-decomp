#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a BITBLTBUF: destination buffer of a transfer. */
int func_0021F560(int packet, u32 dest_base, u32 dest_width, u32 dest_format) {
    return func_0021F098(packet, 0x50, ((u64)dest_format << 56) | ((u64)dest_width << 48) | ((u64)dest_base << 32));
}
