#include "common.h"

extern u8 spell_levels[6][5];
extern char D_00402DA8[];

int func_001F93A8(void) {
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] != 0) {
                if (D_00402DA8[(15 * i + 3 * j + spell_levels[i][j]) * 0x110 + 0x102] == 1) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
