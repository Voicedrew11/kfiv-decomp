#include "kf_script.h"

int func_00210C88(ScriptThread *);
void func_00211810(ScriptThread *, u32 *, ScriptArg *);
void func_002108F8(void *, int, int);

/* Starts a command: takes a free command of the thread, sets its number from the high half
   of the script word, reads as many arguments as the low half says and clears its work
   words. Returns where the script goes on. */
u32 *func_00211C38(ScriptThread *thread, u32 *cursor) {
    u32 header;
    u32 number;
    int arg_count;
    int index;
    int arg;

    header = *cursor++;
    arg_count = header & 0xFFFF;
    number = header >> 16;
    index = func_00210C88(thread);
    thread->commands[index].active = number;
    for (arg = 0; arg < arg_count; arg++) {
        func_00211810(thread, cursor, &thread->commands[index].args[arg]);
        cursor += 2;
    }
    func_002108F8(&thread->commands[index].args[16], 8, 0);
    return cursor;
}
