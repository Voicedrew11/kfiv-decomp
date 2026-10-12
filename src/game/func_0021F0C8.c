#include "common.h"
#include "kf_gs.h"

int func_0021F098(int, s32, s64);

/* Adds a TEXFLUSH. */
int func_0021F0C8(int packet) {
    return func_0021F098(packet, GS_REG_TEXFLUSH, 0);
}
