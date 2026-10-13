#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_script.h"

extern int D_00446770;

/* Runs KF_MapObject_Activate on the map object, then finishes. */
void func_00212970(ScriptObjectCommand *command) {
    MapObject *object;

    object = KF_MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        KF_MapObject_Activate(object);
    }
    command->active = 0;
}
