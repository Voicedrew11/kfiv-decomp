#include "common.h"

int func_0021F098(s64, s64, s64);

int func_0021F2A8(s64 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u64 arg5) {
    u64 lo = ((u64)arg3 << 16) | ((u64)arg2 << 8);
    u64 hi = arg1 | (arg5 << 32);
    return func_0021F098(arg0, 1, hi | lo | ((u64)arg4 << 24));
}
