#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Sends driver command 0x2010 with bank bank_index's second word; -1 for a bad index. */
int func_001CE4D0(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5U) {
        return -1;
    }
    bank = &D_004471D4->banks[bank_index];
    return func_001CBAE0(1, 0x2010, bank->unk0C);
}
