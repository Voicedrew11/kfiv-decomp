#include "common.h"
#include "kf_game.h"

extern char D_004471E1;
extern PendingSlot *D_004471E4;
extern PendingSlot *D_004471E8;

/* Makes D_004471E4's slot the current one and clears its unk04. */
void func_001CFC80(void) {
    PendingSlot *slot = D_004471E4;

    D_004471E1 = 0;
    D_004471E8 = slot;
    slot->unk04 = 0;
}
