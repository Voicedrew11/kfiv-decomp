#include "kf_model.h"

/* Restarts a waiting map object (model flag 1, bit 20) on another animation. */
void func_0011E7B0(void *arg, int anim, int bit22) {
    MapObject *object = arg;

    if ((object->model->unk100 & 1) && (object->unk10 & 0x100000)) {
        MAP_OBJECT_BITS(object)->bit21 = FALSE;
        MAP_OBJECT_BITS(object)->bit22 = bit22;
        MAP_OBJECT_BITS(object)->anim = anim;
        MAP_OBJECT_BITS(object)->step = 0;
    }
}
