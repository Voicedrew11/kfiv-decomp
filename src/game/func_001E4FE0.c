#include "common.h"

/* Tells whether value differs from the level the given step stands for. */
int func_001E4FE0(s32 value, u32 step) {
    switch (step) {
        case 0:
        case 1:
        case 2:
        case 11:
            return value != 0;
        case 3:
            return value != 30;
        case 4:
            return value != 55;
        case 5:
            return value != 80;
        case 6:
            return value != 105;
        case 7:
            return value != 130;
        case 8:
            return value != 140;
        case 9:
            return value != 150;
        default:
            return FALSE;
    }
}
