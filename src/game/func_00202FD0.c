#include "kf_screen.h"

void func_002020A0(RenderSlot *, RenderSlotArg);

/* Runs func_002020A0 on slots 0-14 and 16-19 of the slot array. */
void func_00202FD0(RenderSlot *slots, RenderSlotArg slot_arg) {
    int i;

    for (i = 0; i < 15; i++) {
        func_002020A0(&slots[i], slot_arg);
    }
    for (i = 16; i < 20; i++) {
        func_002020A0(&slots[i], slot_arg);
    }
}
