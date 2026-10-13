#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_script.h"

extern int D_00446770;

/* Runs MapObject_Activate on the map object, then finishes. */
void func_00212970(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        MapObject_Activate(object);
    }
    command->active = 0;
}
