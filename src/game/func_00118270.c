#include "kf_map.h"

extern MapLoadState D_0038D610;

/* Map load step: waits for the timer to run out. */
void func_00118270(void) {
    if (--D_0038D610.timer <= 0) {
        D_0038D610.step++;
    }
}
