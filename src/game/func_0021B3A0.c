#include "kf_mc.h"

/* The result of the last card check of this port and slot. */
int func_0021B3A0(int port, int slot) {
    return MC_CARD_INFO->result[port][slot];
}
