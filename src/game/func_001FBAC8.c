#include "kf_game.h"

extern int D_0044733C;

void *memset(void *, int, u32);

void func_001FBAC8(void) {
    ScratchSlot *slot;
    int i;

    for (i = 0; i < 3; i++) {
        slot = (ScratchSlot *)(D_0044733C + i * 0x180);
        slot->words[9] = slot->words[8] = slot->words[7] = slot->words[6] = slot->words[5] = slot->words[4] = slot->words[3] = slot->words[2] = slot->words[1] = slot->words[0] = slot->flag1 = slot->flag0 = 0;
        memset(&slot->tail, 0, sizeof(slot->tail));
    }
}
