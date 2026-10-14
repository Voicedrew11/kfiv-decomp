#include "kf_map_object.h"
#include "kf_model.h"

void MapObject_UpdateOpenChest(MapObject *);

/* Update handler: runs the opening animation; at its end the object is open
   (step 3) for 90 frames. Then the open-state update. */
void func_0011FFF8(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
        object->work[0] = 0x5A;
    }
    MapObject_UpdateOpenChest(object);
}
