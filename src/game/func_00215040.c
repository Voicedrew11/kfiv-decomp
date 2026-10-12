#include "kf_script.h"

int rand(void);
int func_00211CF8(ScriptArg *, int);

/* Script command: store a random number below `input` in the result. */
void func_00215040(ScriptLookupCommand *command) {
    func_00211CF8(&command->result, rand() % command->input.value);
    command->active = 0;
}
