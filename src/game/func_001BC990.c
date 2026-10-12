#include "common.h"

extern u8 D_00367980[6][5];

/* Marks the first free one of the row's five slots. FALSE if all are taken. */
int func_001BC990(int row) {
    int i;

    for (i = 0; i < 5; i++) {
        if (D_00367980[row][i] == 0) {
            D_00367980[row][i] = 1;
            return TRUE;
        }
    }
    return FALSE;
}
