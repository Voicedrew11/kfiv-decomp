#include "kf_model.h"

/* Sets bit 21 of a waiting map object (model flag 1, bit 20). */
void func_0011E810(void *arg, int bit21) {
    MapObject *object = arg;

    if ((object->model->unk100 & 1) && (object->unk10 & 0x100000)) {
        MAP_OBJECT_BITS(object)->bit21 = bit21;
    }
}
