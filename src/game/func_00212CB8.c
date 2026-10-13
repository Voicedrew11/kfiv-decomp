#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_script.h"

extern int D_00446770;

/* Starts the target map object on the two arguments of the command, then deactivates it. */
void func_00212CB8(ScriptCallCommand *command) {
    MapObject *object;

    object = KF_MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        KF_MapObject_PlayScriptedAnim(object, command->first.value, command->second.value);
        KF_MapObject_SetScriptedBit21(object, 1);
    }
    command->active = 0;
}
