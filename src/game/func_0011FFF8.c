#include "kf_model.h"

void func_0011E3A0(char *);
s16 func_0011E340(char *, int);
void func_00120058(MapObject *);

/* Update handler: runs the opening animation; at its end the object is open
   (step 3) for 90 frames. Then the open-state update. */
void func_0011FFF8(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        func_0011E3A0((char *)object);
    } else {
        func_0011E340((char *)object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
        object->work[0] = 0x5A;
    }
    func_00120058(object);
}
