#include "kf_model.h"

/* Start handler of the message kind: takes the message (the definition gives the
   default) and a flag from its source. */
void func_00120D90(MapObject *object) {
    s32 *work = object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);

    work[1] = source->unk44;
    if (work[1] == 0) {
        work[1] = (s16)object->unk04->unk68;
    }
    object->work[0] = source->unk46;
    MAP_OBJECT_BYTES(object)->step = 1;
}
