#include "kf_map_object.h"
#include "kf_script.h"

int func_00211CF8(ScriptArg *, int);
extern int D_00446770;

/* Stores bit 17 of a map object of the current map in `first`, 0 when there is no such object. */
void func_00212C18(ScriptCallCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        func_00211CF8(&command->first, MAP_OBJECT_BITS(object)->bit17);
    } else {
        func_00211CF8(&command->first, 0);
    }
    command->active = 0;
}
