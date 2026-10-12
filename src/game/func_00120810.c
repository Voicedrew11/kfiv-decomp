#include "kf_model.h"

int func_0011E3A0(void *);
s16 func_0011E340(char *, int);

/* Update handler of a switch turning on: runs the animation, then it is on. */
void func_00120810(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        func_0011E3A0((char *)object);
    } else {
        func_0011E340((char *)object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}
