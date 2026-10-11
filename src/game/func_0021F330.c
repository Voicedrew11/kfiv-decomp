#include "common.h"

int func_0021F098(s64, s64, s64);

int func_0021F330(s64 arg0, u32 arg1, u32 arg2, u32 arg3, u64 arg4) {
    u64 lo = arg1 | (arg4 << 32);
    u64 hi = ((u64)arg3 << 24) | ((u64)arg2 << 16);
    return func_0021F098(arg0, 0x4C, lo | hi);
}
