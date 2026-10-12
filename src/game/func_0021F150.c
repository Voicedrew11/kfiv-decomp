#include "common.h"
#include "kf_gs.h"

int func_0021F098(s64, s64, s64);

int func_0021F150(s64 ctx, u32 tbp0, u32 tbw, u32 psm, u32 tex_width_log2, u32 tex_height_log2, u32 tcc, u32 tfx, u32 cbp, u32 cpsm, u32 csm, u32 csa, u32 cld) {
    u64 value;

    value = tbp0 | ((u64)tbw << 14) | ((u64)psm << 20) | ((u64)tex_width_log2 << 26) | ((u64)tex_height_log2 << 30) | ((u64)tcc << 34) | ((u64)tfx << 35) | ((u64)cbp << 37) | ((u64)cpsm << 51) | ((u64)csm << 55) | ((u64)csa << 56) | ((u64)cld << 61);
    return func_0021F098(ctx, GS_REG_TEX0_1, value);
}
