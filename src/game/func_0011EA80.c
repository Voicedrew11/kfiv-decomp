#include "kf_model.h"

int func_0019E9E0(char);

/* Stops the sound a map object is playing, if any. */
void func_0011EA80(MapObject *object) {
    if (object->sound != 0) {
        func_0019E9E0(object->sound);
        object->sound = 0;
    }
}
