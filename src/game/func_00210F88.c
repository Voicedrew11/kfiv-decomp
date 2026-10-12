#include "kf_script.h"

typedef void (*ScriptCommandHandler)(ScriptThreadCommand *);

extern ScriptCommandHandler D_004331B0[];
extern ScriptCommandHandler D_00433440[];
extern ScriptCommandHandler D_00433650[];
extern int D_004473E4;

/* Steps the thread's commands that have bit 0x8000 set and counts every command in use. */
int func_00210F88(ScriptThread *thread) {
    int i;
    int number;
    int index;

    D_004473E4 = 0;
    for (i = 0; i < 16; i++) {
        number = thread->commands[i].active;
        if (number != 0) {
            if (number & 0x8000) {
                // match: the mask keeps bit 0x8000 and the tables are indexed with the unmasked number
                index = number & 0x7FFFF;
                if (index >= 0xC8) {
                    D_00433650[number - 0xC8](&thread->commands[i]);
                } else if (index >= 0x64) {
                    D_00433440[number - 0x64](&thread->commands[i]);
                } else {
                    D_004331B0[number](&thread->commands[i]);
                }
            }
            D_004473E4++;
        }
    }
    return D_004473E4;
}
