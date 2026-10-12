#include "kf_mc.h"

/* The type of the card in this port and slot. */
int func_0021B438(int port, int slot) {
    return MC_CARD_INFO->type[port][slot];
}
