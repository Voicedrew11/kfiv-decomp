#include "common.h"

int func_0021F098(s64, s64, s64);

int func_0021F0E8(s64 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6, u64 arg7) {
    u64 b = ((u64) arg1) | (arg7 << 32);
    u64 a = (((u64) arg2) << 2) | (((u64) arg5) << 9);
    u64 c = ((((u64) arg4) << 6) | (((u64) arg3) << 5)) | (((u64) arg6) << 19);

    return func_0021F098(arg0, 0x14, (b | a) | c);
}
