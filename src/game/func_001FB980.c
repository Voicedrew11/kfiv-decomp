#include "kf_game.h"

extern int D_0044733C;

char func_001FB980(int index) {
    ScratchSlot *slot = (ScratchSlot *)(D_0044733C + index * 0x180);

    if (slot->flag1 == 0) {
        return slot->flag0;
    }
    return -1;
}
