#include "kf_script.h"

int func_00210C48(int);
u32 *func_00210CD0(u32 *);
extern char D_00432920[];
extern u32 *D_004473E8;
extern ScriptThread D_01DFB3B0[];

/* Starts the script `script_id` in a free slot unless it is running already: looks its label (word 0x190)
   up from the top of the script data. Gives the slot's index, 0 when nothing was started. */
int func_002109C8(int script_id, int argument) {
    ScriptThread *thread;
    u32 slot_index;
    u32 word;
    int operand;
    u32 command_index;
    char unused[0x50]; // match: retail reserves 0x50 bytes it never uses

    if (func_00210C48(script_id)) {
        return 0;
    }
    thread = NULL;
    for (slot_index = 0; slot_index < 24; slot_index++) {
        if (D_01DFB3B0[slot_index].script_id == 0) {
            thread = &D_01DFB3B0[slot_index];
            break;
        }
    }
    if (thread == NULL) {
        D_00432920[0] = 1;
        return 0;
    }
    *(void **)&thread->argument = (void *)argument; // match: stored as a pointer, so the word read below does not wait for it
    thread->script_id = script_id;
    thread->cursor = D_004473E8;
    while (TRUE) {
        word = *thread->cursor;
        operand = word & 0xFFFF;
        word >>= 16;
        if (word == 0x190) {
            if (operand == script_id) {
                break;
            }
            if (operand == 0x7FFE) {
                return 0; // match: the end label; the slot stays claimed and 0 is also slot 0's index
            }
        }
        thread->cursor = func_00210CD0(thread->cursor);
    }
    for (command_index = 0; command_index < 16; command_index++) {
        thread->commands[command_index].active = 0;
    }
    return slot_index;
}
