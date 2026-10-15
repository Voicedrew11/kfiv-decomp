#include "kf_model.h"

int Sound_StopTagged(char);

/* Stops the sound a map object is playing, if any. */
void func_0011EA80(MapObject *object) {
    if (object->sound != 0) {
        Sound_StopTagged(object->sound);
        object->sound = 0;
    }
}
