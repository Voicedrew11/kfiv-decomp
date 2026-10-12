#include "kf_script.h"
#include "kf_map.h"

extern MapLoadState D_0038D610;
extern MapLoadRequest D_0038D640;
extern int D_00446794;

/* Set D_00446794 when a background map load is running for a pending request. */
void func_00213D58(ScriptValueCommand *command) {
    if (D_0038D610.step != 0 && D_0038D640.map != 0xFFFF) {
        D_00446794 = 1;
    }
    command->active = 0;
}
