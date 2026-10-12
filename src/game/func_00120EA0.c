#include "kf_model.h"

/* Update handler of the message kind, step 4: back to rest when the 45 frames are over. */
void func_00120EA0(MapObject *object) {
    s32 *work = object->work;

    if (--work[2] <= 0) {
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}
