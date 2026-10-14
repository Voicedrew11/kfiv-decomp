#include "kf_script.h"

extern u8 recollection_pages[50];

/* Script command: sets which description Recollection page `first` shows to `second` (0 to 3), then finishes. */
void func_002120A0(ScriptPairCommand *command) {
    u32 index;

    index = command->first.value;
    if (index < 50 && command->second.value >= 0 && command->second.value < 4) {
        recollection_pages[index] = command->second.value;
    }
    command->active = 0;
}
