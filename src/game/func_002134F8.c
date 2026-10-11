#include "kf_script.h"

extern char D_0038CC50[];
extern float D_00413F80[];
extern float D_00413F90[];

int func_0019B9F0(float, float *, int);

/* Eases two of the command's floats toward the shared target each frame until the count runs out. */
void func_002134F8(ScriptBlendCommand *command) {
    if (D_0038CC50[0] != 0 || (command->target_a.value == D_00413F80[4] && command->target_b.value == D_00413F80[5])) {
        command->active = 0;
    } else {
        func_0019B9F0(command->target_a.value, D_00413F90, command->frames.value);
        func_0019B9F0(command->target_b.value, &D_00413F90[1], command->frames.value);
        if (--command->frames.value == 0) {
            command->active = 0;
        }
    }
}
