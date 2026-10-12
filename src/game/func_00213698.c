#include "common.h"
#include "kf_script.h"

extern u8 D_00369830[];

int func_00211CF8(char *, int);

void func_00213698(ScriptMotionCommand *command) {
    u32 index;

    index = command->target_id.value;
    if (index < 0x17C) {
        func_00211CF8((char *)&command->frames, D_00369830[index]);
    } else {
        func_00211CF8((char *)&command->frames, -1);
    }
    command->active = 0;
}
