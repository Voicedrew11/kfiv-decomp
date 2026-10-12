#include "common.h"

extern u8 spell_levels[6][5];

/* Marks the first free one of the row's five slots. FALSE if all are taken. */
int func_001BC990(int row) {
    int i;

    for (i = 0; i < 5; i++) {
        if (spell_levels[row][i] == 0) {
            spell_levels[row][i] = 1;
            return TRUE;
        }
    }
    return FALSE;
}
