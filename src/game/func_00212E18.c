#include "kf_map_object.h"
#include "kf_script.h"

extern int D_00446770;

/* Sets the colour of a map object's model (ModelInstance.unk150, unk154, unk158 and alpha). */
void func_00212E18(ScriptObjectValuesCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL) {
        object->model->unk150 = command->values[0].value;
        object->model->unk154 = command->values[1].value;
        object->model->unk158 = command->values[2].value;
        object->model->alpha = command->values[3].value;
    }
    command->active = 0;
}
