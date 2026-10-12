#include "kf_model.h"

int func_0011E3A0(void *);
s16 func_0011E340(char *, int);

/* Update handler, step 4: waits, then runs the animation back; at its end the
   object is off again. */
void func_00121CA0(MapObject *object) {
    if (object->work[0] != 0) {
        if (MAP_OBJECT_WORDS(object)->unk46 > 0) {
            object->work[0]--;
        }
    } else if (MODEL_CHANNEL(object->model)->param != 0) {
        func_0011E3A0((char *)object);
    } else {
        func_0011E340((char *)object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        object->work[0] = MAP_OBJECT_WORDS(object)->unk44;
    }
}
