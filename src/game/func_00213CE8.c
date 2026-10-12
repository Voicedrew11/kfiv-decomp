#include "kf_script.h"
#include "kf_map.h"

extern MapLoadState D_0038D610;
extern MapLoadRequest D_0038D640;

/* Ask for map `value` to be loaded in the background: the command waits while a load is running
   or a request is pending, then fills the request (no offset between the maps) and starts the
   load after two frames. */
void func_00213CE8(ScriptValueCommand *command) {
    if ((u32)command->value.value < 0x80) {
        if (D_0038D610.step == 0 && D_0038D640.map == 0xFFFF) {
            D_0038D640.map = command->value.value;
            D_0038D640.x = 0.0f;
            D_0038D640.y = 0.0f;
            D_0038D640.z = 0.0f;
            D_0038D610.step = 0;
            D_0038D610.request = &D_0038D640;
            D_0038D610.next_request = NULL;
            D_0038D610.timer = 2;
            command->active = 0;
        }
        return;
    }
    command->active = 0;
}
