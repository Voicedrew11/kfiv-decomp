#include "kf_map_object.h"
#include "kf_model.h"


/* Update handler, step 4: waits, then runs the animation back; at its end the
   object is off again. */
void func_00121CA0(MapObject *object) {
    if (object->work[0] != 0) {
        if (MAP_OBJECT_WORDS(object)->unk46 > 0) {
            object->work[0]--;
        }
    } else if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        object->work[0] = MAP_OBJECT_WORDS(object)->unk44;
    }
}
