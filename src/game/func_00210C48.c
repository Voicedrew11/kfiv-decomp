#include "kf_script.h"

extern ScriptThread D_01DFB3B0[];

/* TRUE if some slot is running the script `script_id`. */
int func_00210C48(int script_id) {
    ScriptThread *thread;
    ScriptThread *end;

    thread = D_01DFB3B0;
    end = (ScriptThread *)((u8 *)thread + 0x12720);
    do {
        if (thread->script_id == script_id) {
            return TRUE;
        }
        thread++;
    } while ((s32)thread < (s32)end);
    return FALSE;
}
