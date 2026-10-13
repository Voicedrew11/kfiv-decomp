#include "kf_map_object.h"
#include "kf_model.h"


/* Start handler shared by several kinds: restarts the animation it is on. */
void func_0011EE18(MapObject *object) {
    if (object->unk04->kind == 0) {
        object->work[0] = 0;
    }
    if (object->model->unk100 & 1) {
        KF_MapObject_StartAnim(object, MAP_OBJECT_BYTES(object)->anim);
        KF_MapObject_StepAnim(object);
    }
    MAP_OBJECT_BYTES(object)->step = 1;
}
