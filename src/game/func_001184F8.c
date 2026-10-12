#include "kf_map.h"
#include "kf_file.h"

extern MapLoadState D_0038D610;

/* Map load step: once the read in flight is done, goes back to read the next piece. */
void func_001184F8(void) {
    if (func_00102790(D_0038D610.file_handle, 1) == 0) {
        D_0038D610.step--;
    }
}
