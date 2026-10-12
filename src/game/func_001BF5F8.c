#include "common.h"

extern u8 spell_levels[6][5];

int func_001BF5F8(void) {
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] != 0) {
                return 1;
            }
        }
    }
    return 0;
}
