#include "kf_model.h"

s16 func_0011E340(char *, int);

/* Start handler: animation 4, step 1. */
void func_00120668(MapObject *object) {
    func_0011E340((char *)object, 4);
    MAP_OBJECT_BYTES(object)->step = 1;
}
