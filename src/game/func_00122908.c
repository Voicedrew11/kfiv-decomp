#include "kf_model.h"

void func_0011E3A0(char *);
s16 func_0011E340(char *, int);

/* Update handler of a switch turning off: runs the animation, then it is off. */
void func_00122908(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        func_0011E3A0((char *)object);
    } else {
        func_0011E340((char *)object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}
