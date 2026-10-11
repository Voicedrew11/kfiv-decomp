#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Copy entry index (0-4) of the table into the current record, then pass it to func_001CBAE0. */
void func_001CF008(u32 index) {
    if (index < 5) {
        D_004471D4->effect = D_004471D4->effects[index];
        func_001CBAE0(0, 0x40070, &D_004471D4->effect);
    }
}
