#include "kf_model.h"

s16 func_0011E340(char *, int);

/* Start handler: off (animation 4, step 1) or on (animation 5, step 3), with
   the wait that goes with it. */
void func_00121B38(MapObject *object) {
    if (!(object->unk10 & 0x20000)) {
        func_0011E340((char *)object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        object->work[0] = MAP_OBJECT_WORDS(object)->unk44;
    } else {
        func_0011E340((char *)object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
        object->work[0] = MAP_OBJECT_WORDS(object)->unk46;
    }
}
