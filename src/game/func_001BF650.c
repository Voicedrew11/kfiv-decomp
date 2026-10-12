#include "kf_effect.h"

extern u8 D_00367980[6][5];
extern FxTableEntry D_00402DA8[];

int func_001BF650(void) {
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (D_00367980[i][j] != 0) {
                if (D_00402DA8[15 * i + 3 * j + 1].unk102 == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
