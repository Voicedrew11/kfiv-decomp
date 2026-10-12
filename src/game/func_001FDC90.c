#include "common.h"
#include "kf_gs.h"

extern GsPackedWord D_00446D68;

/* Resets D_00446D68 to 0x140, 0xB0, 0, 0. */
int func_001FDC90(void) {
    D_00446D68.f.lo16 = 0x140;
    D_00446D68.f.mid16 = 0xB0;
    D_00446D68.f.mid24 = 0;
    D_00446D68.f.hi8 = 0;
    return 1;
}
