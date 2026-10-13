#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_script.h"

extern int D_00446770;

/* Runs KF_MapObject_Signal on the map object with the first argument, then finishes. */
void func_00212F28(ScriptCallCommand *command) {
    MapObject *object;

    object = KF_MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        KF_MapObject_Signal(object, command->first.value);
    }
    command->active = 0;
}
