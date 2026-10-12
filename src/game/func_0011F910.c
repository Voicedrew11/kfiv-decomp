#include "kf_model.h"

/* Opens a closed door-like object (step 1 to 2) or closes an open one (3 to 4). */
void func_0011F910(MapObject *object, int open) {
    if ((object->unk10 & 0x2FF00) == 0x100 && open) {
        object->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 2;
    } else if ((object->unk10 & 0x2FF00) == 0x20300 && !open) {
        object->unk10 &= ~0x20000;
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}
