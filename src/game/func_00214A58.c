#include "common.h"
#include "kf_script.h"

extern int D_0038CC50[];

void func_00214A58(ScriptObjectCommand *command) {
    if (command->object_id.value != 0) {
        if (!(((u32)D_0038CC50[9] >> 12) & 1)) {
            D_0038CC50[9] = (D_0038CC50[9] | 0x1000) & ~0x2000;
        }
    } else {
        D_0038CC50[9] &= ~0x1000;
        D_0038CC50[9] &= ~0x2000;
    }
    command->active = 0;
}
