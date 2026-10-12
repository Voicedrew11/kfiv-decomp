#include "kf_mc.h"

/* Picks the message for the last check of a card. Returns what the check answered, negated (0 for an
   unchanged card), or a negative number when the card cannot be used. */
int func_0021B208(int port, int slot) {
    int type = MC_CARD_INFO->type[port][slot];
    int result;

    if (type == 1) {
        D_01E4F278.timeout = 1;
        return -1;
    }
    result = MC_CARD_INFO->result[port][slot];
    if (result < -2) {
        D_01E4F278.timeout = 1;
        return -2;
    }
    if (type != 2 && type != 3) {
        D_01E4F278.timeout = 1;
        return -5;
    }
    result = -result;
    if (type == 3) {
        switch (result) {
        case 3:
            D_01E4F278.timeout = 0x14;
            break;
        case 4:
            D_01E4F278.timeout = 0x16;
            break;
        case 5:
            D_01E4F278.timeout = 0x15;
            break;
        default:
            D_01E4F278.timeout = 1;
            break;
        }
    } else {
        switch (result) {
        case 0:
            D_01E4F278.timeout = 0;
            break;
        case 1:
            D_01E4F278.timeout = 2;
            break;
        case 2:
            D_01E4F278.timeout = 3;
            break;
        default:
            D_01E4F278.timeout = 1;
            break;
        }
    }
    if (D_01E4F278.timeout == 1) {
        return -6;
    }
    return result;
}
