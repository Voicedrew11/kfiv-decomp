#include "common.h"
#include "kf_script.h"

extern char D_0038CC53[];
extern float D_004325A0[];

int func_0019B9F0(float, float *, int);

/* Eases three of the command's floats toward the shared target each frame until the count runs out. */
void func_00213BA8(ScriptTripleCommand *command) {
    if (D_0038CC53[0] == 0 ||
        (command->x.value == D_004325A0[4] && command->y.value == D_004325A0[5] && command->z.value == D_004325A0[6])) {
        command->active = 0;
    } else {
        func_0019B9F0(command->x.value, &D_004325A0[4], command->frames.value);
        func_0019B9F0(command->y.value, &D_004325A0[5], command->frames.value);
        func_0019B9F0(command->z.value, &D_004325A0[6], command->frames.value);
        if (--command->frames.value == 0) {
            command->active = 0;
        }
    }
}
