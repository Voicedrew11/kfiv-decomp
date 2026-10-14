#include "kf_script.h"

int func_00210AF8(int, int);
int func_00210C28(int);

/* Starts a script once and keeps request->active set until the script has finished (with unk10 1, clears it
   at once). */
void func_00211E58(ScriptRequest *request) {
    if (request->started == 0) {
        request->started = 1;
        request->slot = func_00210AF8(request->script_id, 0);
        if (request->unk10 == 1) {
            request->active = 0;
        }
    } else if (func_00210C28(request->slot) == 0) {
        request->active = 0;
    }
}
