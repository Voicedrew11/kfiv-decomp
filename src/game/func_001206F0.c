#include "kf_map_object.h"

/* Step handler: runs the animation; when it ends, starts animation 5, or with the
   channel index set animation 4 at step 1. */
void func_001206F0(MapObject *object) {
    ModelChannel *channel = MODEL_CHANNEL(object->model);

    if (channel->index == 0) {
        if (channel->param != 0) {
            MapObject_StepAnim(object);
        } else {
            MapObject_StartAnim(object, 5);
        }
    } else {
        if (channel->param == 0) {
            MapObject_StartAnim(object, 4);
            MAP_OBJECT_BYTES(object)->step = 1;
        } else {
            MapObject_StepAnim(object);
        }
    }
}
