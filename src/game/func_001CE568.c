#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Sends driver command 0x2050 with bank bank_index's second word if the bank holds IOP memory; -1 if not. */
int func_001CE568(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5U) {
        return -1;
    }
    bank = &D_004471D4->banks[bank_index];
    if (bank->iop_heap == 0) {
        return -1;
    }
    return (func_001CBAE0(1, 0x2050, bank->unk0C) == 0) ? 0 : -1;
}
