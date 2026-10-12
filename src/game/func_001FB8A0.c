#include "common.h"
#include "kf_game.h"

extern ScratchSlot *D_0044733C;
extern s32 D_0038CC84[];
extern s32 D_00413F80[];
extern PlayTime D_00446A28;

void SyncDCache(void *start, void *end);

void func_001FB8A0(void *unused_arg0, s32 index) {
    ScratchSlot *slot;

    if (D_0044733C == NULL) {
        return;
    }
    slot = D_0044733C + index;
    slot->flag0 = 1;
    slot->flag1 = 0;
    slot->words[0] = D_0038CC84[0];
    slot->words[1] = D_00413F80[20];
    slot->words[2] = D_00413F80[21];
    slot->words[3] = D_00413F80[45];
    slot->words[4] = D_00413F80[22];
    slot->words[5] = D_00413F80[46];
    slot->words[6] = D_00413F80[23];
    slot->words[7] = D_00413F80[10];
    slot->words[8] = D_00413F80[11];
    slot->words[9] = D_00413F80[19];
    slot->tail = D_00446A28;
    SyncDCache(D_0044733C, D_0044733C + 3);
}
