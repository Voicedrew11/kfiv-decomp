#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a PRMODECONT. */
int func_0021F460(int packet, u32 use_prim) {
    return func_0021F098(packet, 0x1A, (u64)use_prim);
}
