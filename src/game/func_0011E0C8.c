#include "kf_model.h"

/* Puts a map object in use: not started, fully opaque. */
void func_0011E0C8(MapObject *object) {
    ModelInstance *model;

    if (object != NULL && !(object->unk10 & 0x10000)) {
        object->unk10 |= 0x10000;
        MAP_OBJECT_BITS(object)->step = 0;
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
        MAP_OBJECT_BITS(object)->anim = 4;
        model = object->model;
        model->unk145 = 0;
        model->unk150 = model->unk154 = model->unk158 = model->alpha = 128.0f;
        object->model->unk146 = 0x80;
    }
}
