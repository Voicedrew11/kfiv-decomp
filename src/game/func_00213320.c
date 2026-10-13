#include "kf_map_object.h"
#include "kf_script.h"

extern int D_00446770;

/* Plays animation `first` (bit 22 from `second`) on a scripted map object and clears its bit 21. */
void func_00213320(ScriptCallCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        MapObject_PlayScriptedAnim(object, command->first.value, command->second.value);
        MapObject_SetScriptedBit21(object, 0);
    }
    command->active = 0;
}
