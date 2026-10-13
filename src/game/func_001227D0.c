#include "kf_map_object.h"

/* Start handler: closed (animation 4, step 1) or, with bit 17 set, open (animation 5, step 3). */
void func_001227D0(MapObject *object) {
    if (!(object->unk10 & 0x20000)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}
