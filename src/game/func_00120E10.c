#include "kf_model.h"

int func_00208170(int);

/* Update handler of the message kind, step 2: back to rest once func_00208170
   accepts the message. */
void func_00120E10(MapObject *object) {
    if (func_00208170(object->work[1])) {
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}
