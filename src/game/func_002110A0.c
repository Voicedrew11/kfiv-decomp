#include "kf_script.h"

int func_00210C88(ScriptThread *);
int func_002111F0(ScriptThread *, u32 *);
u32 *func_002114D8(ScriptThread *, u32 *);
u32 *func_00211C38(ScriptThread *, u32 *);
int func_00211CF0(void);

/* Reads on in the thread's script until a word makes it wait: control words (0x190 to 0x19D) are
   handled here, anything else starts a command. TRUE if a command was started. */
int func_002110A0(ScriptThread *thread) {
    int started;
    int no_wait;
    int running;
    u32 *cursor;
    u32 word;
    int operand;
    int skip;

    started = FALSE;
    no_wait = FALSE;
    running = TRUE;
    cursor = thread->cursor;
    do {
        word = *cursor;
        operand = word & 0xFFFF;
        switch (word >> 16) {
        case 0x191:
            running = FALSE;
            thread->script_id = 0;
            break;
        case 0x194:
            no_wait = TRUE;
        case 0x190:
        next_word:
            cursor++;
            break;
        case 0x195:
            running = FALSE;
            no_wait = FALSE;
            goto next_word; // match: retail jumps back to the shared step; a second cursor++ here merges the other way
            break;
        case 0x192:
            skip = func_002111F0(thread, cursor);
            cursor = thread->cursor;
            if (skip != 0) {
                cursor += skip;
            }
            break;
        case 0x197:
            cursor++;
            cursor += (s16)operand;
            break;
        case 0x196:
            cursor = func_002114D8(thread, cursor);
            break;
        case 0x19D:
            // match: func_00211CF0 is a stub that takes nothing and gives 0, so the cursor becomes NULL
            cursor = (u32 *)((int (*)(u32 *))func_00211CF0)(cursor);
            running = no_wait;
            started = TRUE;
            thread->commands[func_00210C88(thread)].active = 4;
            break;
        default:
            cursor = func_00211C38(thread, cursor);
            running = no_wait;
            started = TRUE;
            break;
        }
    } while (running);
    thread->cursor = cursor;
    return started;
}
