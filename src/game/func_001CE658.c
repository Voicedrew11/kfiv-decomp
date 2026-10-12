#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Sends driver command 0x2080 with bank bank_index's second word and an unsigned 16-bit value. */
int func_001CE658(int bank_index, u16 value) {
    return func_001CBAE0(1, 0x2080, D_004471D4->banks[bank_index].unk0C, value);
}
