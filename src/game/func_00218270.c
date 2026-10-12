#include "kf_mc.h"

/* Turns the result of a card check into the message code for the current port and slot. */
int func_00218270(int result) {
    int port = D_01E4F278.unk80;
    int slot = D_01E4F278.unk84;

    if (result >= 0) {
        if (MC_CARD_INFO->format[port][slot] == 0) {
            if (MC_CARD_INFO->type[port][slot] == 3) {
                return 1;
            }
            return 3;
        }
        return MC_CARD_INFO->type[port][slot] == 3;
    }
    if (result == -1) {
        if (MC_CARD_INFO->type[port][slot] == 3) {
            return 1;
        }
        return 2;
    }
    if (result == -2) {
        if (MC_CARD_INFO->type[port][slot] == 3) {
            return 1;
        }
        return 4;
    }
    if (result <= -11) {
        return 1;
    }
    return 0x64;
}
