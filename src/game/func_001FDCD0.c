#include "kf_gs.h"

extern u8 D_00446D70[4];

s32 func_001FDCD0(ColorBytes bytes) {
    D_00446D70[0] = bytes.red;
    D_00446D70[1] = bytes.green;
    D_00446D70[2] = bytes.blue;
    D_00446D70[3] = bytes.alpha;
    return 1;
}
