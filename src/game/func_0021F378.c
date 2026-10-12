#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a TEST_1: alpha, destination alpha and depth tests. */
int func_0021F378(int packet, u32 ate, u32 atst, u32 aref, u32 afail, u32 date, u32 datm, u32 zte, u32 ztst) {
    return func_0021F098(packet, GS_REG_TEST_1, ate | ((u64)atst << 1) | ((u64)aref << 4) | ((u64)afail << 12) | ((u64)date << 14) | ((u64)datm << 15) | ((u64)zte << 16) | ((u64)ztst << 17));
}
