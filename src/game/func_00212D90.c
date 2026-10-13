#include "kf_map_object.h"
#include "kf_script.h"

int func_00211CF8(ScriptArg *, int);
extern int D_00446770;

/* Stores in `first` whether a map object's animation has ended; -1 when the object
   has no motion or bit 22 is set. */
void func_00212D90(ScriptCallCommand *command) {
    MapObject *object = MapObject_GetById(D_00446770, command->target_id.value);

    if (object != NULL && (object->model->unk100 & 1) && !(object->unk10 & 0x400000)) {
        func_00211CF8(&command->first, MODEL_CHANNEL(object->model)->param < 1);
    } else {
        func_00211CF8(&command->first, -1);
    }
    command->active = 0;
}
