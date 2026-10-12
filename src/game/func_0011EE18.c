#include "kf_model.h"

s16 func_0011E340(char *, int);
void func_0011E3A0(char *);

/* Start handler shared by several kinds: restarts the animation it is on. */
void func_0011EE18(MapObject *object) {
    if (object->unk04->kind == 0) {
        object->work[0] = 0;
    }
    if (object->model->unk100 & 1) {
        func_0011E340((char *)object, MAP_OBJECT_BYTES(object)->anim);
        func_0011E3A0((char *)object);
    }
    MAP_OBJECT_BYTES(object)->step = 1;
}
