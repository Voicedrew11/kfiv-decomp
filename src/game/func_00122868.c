#include "kf_model.h"

void func_0011E3A0(char *);
s16 func_0011E340(char *, int);

/* Update handler of a switch turning on: runs the animation, then it is on. */
void func_00122868(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        func_0011E3A0((char *)object);
    } else {
        func_0011E340((char *)object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}
