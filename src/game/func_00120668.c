#include "kf_map_object.h"
#include "kf_model.h"


/* Start handler: animation 4, step 1. */
void func_00120668(MapObject *object) {
    MapObject_StartAnim(object, 4);
    MAP_OBJECT_BYTES(object)->step = 1;
}
