#include "kf_script.h"

int func_00211CF8(ScriptArg *, int);
extern u8 D_0034C4E0[50];

/* Hands the script the counter of area `input` (-1 when there is no such area), then finishes. */
void func_00212040(ScriptQueryCommand *command) {
    u32 index;

    index = command->input.value;
    if (index < 50) {
        func_00211CF8(&command->result, D_0034C4E0[index]);
    } else {
        func_00211CF8(&command->result, -1);
    }
    command->active = 0;
}
