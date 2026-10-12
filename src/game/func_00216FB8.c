#include "kf_game.h"

extern PadSystem D_01E0DB00;
int scePadPortOpen(int, int, void *);

/* Opens a controller slot and marks it for func_00217008 to poll. */
int func_00216FB8(int port, int slot) {
    D_01E0DB00.open_slots[port] |= 1 << slot;
    return scePadPortOpen(port, slot, D_01E0DB00.slots[port * 4 + slot].dma_buffer);
}
