#include "kf_script.h"

extern ScriptThread D_01DFB3B0[24];

/* Stops every script: frees all 24 threads and their commands, then finishes. */
void func_00211ED0(ScriptValueCommand *command) {
    int thread;
    int next;
    int index;

    for (thread = 0; thread < 24; thread = next) {
        next = thread + 1; // match: retail keeps thread + 1 in a register of its own across the inner loop
        D_01DFB3B0[thread].script_id = 0;
        for (index = 15; index >= 0; index--) {
            D_01DFB3B0[thread].commands[index].active = 0;
        }
    }
    command->active = 0;
}
