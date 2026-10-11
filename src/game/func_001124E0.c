#include "common.h"

u32 func_001124E0(int arg0, u32 arg1, u32 arg2) {
    if (arg0 == 0 && arg1 == 0xFF && arg2 < 8) {
        return arg2;
    }
    if (arg0 == 0xFF && arg1 == 0 && arg2 < 8) {
        return arg2;
    }
    if (arg0 == 0xFF && arg1 < 8 && arg2 == 0xFF) {
        return arg1;
    }
    if (arg0 == 0xFF && arg1 == 0xFF && arg2 >= 0xFE) {
        return 5;
    }
    return 6;
}
