#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_script.h"

extern int D_00446770;

/* Sets bit 21 of the map object if it is waiting, then finishes. */
void func_00213380(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        MapObject_SetScriptedBit21(object, 1);
    }
    command->active = 0;
}
