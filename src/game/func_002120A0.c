#include "kf_script.h"

extern u8 D_0034C4E0[50];

/* Sets the counter of area `first` to `second` (0 to 3), then finishes. */
void func_002120A0(ScriptPairCommand *command) {
    u32 index;

    index = command->first.value;
    if (index < 50 && command->second.value >= 0 && command->second.value < 4) {
        D_0034C4E0[index] = command->second.value;
    }
    command->active = 0;
}
