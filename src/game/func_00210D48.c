#include "kf_script.h"

int func_00210E88(ScriptThread *);
int func_002110A0(ScriptThread *);
extern ScriptThread *D_00446FD0;
extern int D_004473E4;
extern ScriptThread D_01DFB3B0[];

/* Runs the 24 script slots for a frame: steps a live one's commands and, when none is left, reads on in its script. */
int func_00210D48(void) {
    ScriptThread *thread;
    int i;
    char unused[0x30]; // match: retail reserves 0x30 bytes it never uses

    D_004473E4 = 0;
    for (i = 0; i < 24; i++) {
        thread = &D_01DFB3B0[i];
        if (thread->script_id != 0) {
            D_00446FD0 = thread;
            while (!func_00210E88(thread)) {
                if (!func_002110A0(thread)) {
                    break;
                }
            }
        }
    }
    return D_004473E4;
}
