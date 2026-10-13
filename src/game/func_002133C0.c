#include "kf_map_object.h"
#include "kf_script.h"

extern int D_00446770;

/* Clear bit 21 of a map object of the current map. */
void func_002133C0(ScriptObjectCommand *command) {
    void *object;

    object = KF_MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        KF_MapObject_SetScriptedBit21(object, 0);
    }
    command->active = 0;
}
