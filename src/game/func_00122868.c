#include "kf_map_object.h"
#include "kf_model.h"


/* Update handler of a switch turning on: runs the animation, then it is on. */
void func_00122868(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}
