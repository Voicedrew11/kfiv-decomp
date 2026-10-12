#include "kf_model.h"

/* Update handler, step 3: turns off again (unless its second wait is negative). */
void func_00121C68(MapObject *object) {
    if (MAP_OBJECT_WORDS(object)->unk46 >= 0) {
        object->unk10 &= ~0x20000;
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}
