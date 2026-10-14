#include "kf_script.h"

int func_00211CF8(ScriptArg *, int);
extern u8 recollection_pages[50];

/* Script command: hands the script the description Recollection page `input` shows (-1 when there is no such
   page), then finishes. */
void func_00212040(ScriptQueryCommand *command) {
    u32 index;

    index = command->input.value;
    if (index < 50) {
        func_00211CF8(&command->result, recollection_pages[index]);
    } else {
        func_00211CF8(&command->result, -1);
    }
    command->active = 0;
}
