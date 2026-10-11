#include "common.h"

extern u8 D_00367980[6][5];

int func_001BF5F8(void) {
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (D_00367980[i][j] != 0) {
                return 1;
            }
        }
    }
    return 0;
}
