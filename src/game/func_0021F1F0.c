#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a PRIM. */
int func_0021F1F0(int packet, u32 prim, u32 iip, u32 tme, u32 fge, u32 abe, u32 aa1, u32 fst, u32 ctxt, u32 fix) {
    return func_0021F098(packet, 0, prim | ((u64)iip << 3) | ((u64)tme << 4) | ((u64)fge << 5) | ((u64)abe << 6) | ((u64)aa1 << 7) | ((u64)fst << 8) | ((u64)ctxt << 9) | ((u64)fix << 10));
}
