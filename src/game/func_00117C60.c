#include "kf_map.h"

extern MapLoadState D_0038D610;
extern MapLoadRequest D_0038D640;
extern int D_00446794;

void *memset(void *, int, u32);

/* Resets the background map load and the pending request. */
void func_00117C60(void) {
    memset(&D_0038D610, 0, sizeof(MapLoadState));
    D_0038D610.file_handle = -1;
    D_0038D610.step = 0;
    D_0038D610.request = NULL;
    D_00446794 = 0;
    D_0038D640.x = 0.0f;
    D_0038D640.y = 0.0f;
    D_0038D640.z = 0.0f;
    D_0038D640.map = 0xFFFF;
}
