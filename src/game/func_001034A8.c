#include "common.h"

extern u8 D_0034C918[];

int func_001034A8(int arg0) {
    if ((u32)arg0 < 0x80U) {
        return D_0034C918[arg0 / 8] >> (arg0 % 8) & 1;
    }
    return 0;
}
