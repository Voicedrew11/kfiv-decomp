#include "kf_model.h"
#include "kf_script.h"

int func_0011E1F0(int, int);
extern int D_00446770;

/* Waits until the map object's motion has run out; finishes at once when there is no such
   object, its model has no flag 1 or its bit 22 is set. */
void func_00212D18(ScriptObjectCommand *command) {
    MapObject *object;

    object = (MapObject *)func_0011E1F0(D_00446770, command->object_id.value);
    if (object == NULL || !(object->model->unk100 & 1) || (object->unk10 & 0x400000) ||
        MODEL_CHANNEL(object->model)->param == 0) {
        command->active = 0;
    }
}
