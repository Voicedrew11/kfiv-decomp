#include "kf_map_object.h"

/* Step handler: waits out a delay (counted only while unk44 is positive), then runs the
   animation and, once it ends, starts animation 5 at step 3 with the delay reloaded from unk46. */
void func_00121BE8(MapObject *object) {
    int delay = object->work[0];
    char reserved[0x10]; // match: unused, but retail reserves the 0x10 bytes

    if (delay != 0) {
        if (MAP_OBJECT_WORDS(object)->unk44 > 0) {
            object->work[0] = delay - 1;
        }
    } else {
        if (MODEL_CHANNEL(object->model)->param != 0) {
            KF_MapObject_StepAnim(object);
        } else {
            KF_MapObject_StartAnim(object, 5);
            MAP_OBJECT_BYTES(object)->step = 3;
            object->work[0] = MAP_OBJECT_WORDS(object)->unk46;
        }
    }
}
