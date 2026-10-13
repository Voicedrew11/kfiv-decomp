#include "kf_map_object.h"
#include "kf_model.h"


/* Update handler of a switch turning off: runs the animation, then it is off. */
void func_001208B0(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        KF_MapObject_StepAnim(object);
    } else {
        KF_MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}
