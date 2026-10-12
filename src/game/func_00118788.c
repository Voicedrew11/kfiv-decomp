#include "kf_map.h"

extern MapLoadState D_0038D610;

void func_00118788(void) {
    if (!(D_0038D610.flags.raw & 6)) {
        D_0038D610.flags.raw |= 4;
    }
}
