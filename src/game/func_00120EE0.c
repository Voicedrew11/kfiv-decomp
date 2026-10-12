#include "kf_model.h"

s16 func_0011E340(AnimObject *, int);

void func_00120EE0(AnimObject *object) {
    AnimObjectBody *body = &object->body;

    body->unk14 = 2;
    body->unk18 = 0;
    if (!(ANIM_OBJECT_FLAGS(object) & 0x20000)) {
        func_0011E340(object, 4);
        object->mode = 1;
    } else {
        func_0011E340(object, 5);
        object->mode = 4;
    }
}
