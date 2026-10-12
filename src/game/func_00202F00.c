#include "kf_screen.h"

void func_002020A0(RenderSlot *, RenderSlotArg);

/* Runs func_002020A0 on the first 20 slots of the slot array. */
void func_00202F00(RenderSlot *slots, RenderSlotArg slot_arg) {
    int i;

    for (i = 0; i < 20; i++) {
        func_002020A0(&slots[i], slot_arg);
    }
}
