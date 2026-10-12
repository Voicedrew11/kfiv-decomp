#include "kf_model.h"
#include "kf_script.h"

int func_0011E1F0(int, int);
extern int D_00446770;

/* Hides part `first` of the map object's model when `second` is 0 and shows it otherwise,
   then finishes. */
void func_00212E90(ScriptCallCommand *command) {
    MapObject *object;
    int part;

    object = (MapObject *)func_0011E1F0(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL) {
        part = command->first.value;
        if (part >= 0 && part < object->model->part_count) {
            MODEL_PART_FLAG_BITS(&object->model->parts[part])->bit40 = command->second.value == 0;
        }
    }
    command->active = 0;
}
