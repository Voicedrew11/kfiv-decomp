#include "common.h"

extern u8 D_00367980[6][5];
extern char D_00402DA8[];

int func_001BF6E8(void) {
    int i;
    int j;
    int k;
    char *p;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (D_00367980[i][j] != 0) {
                k = (15 * i + 3 * j + 1) * 0x110;
                p = D_00402DA8 + k;
                if (p[0x102] == 1) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
