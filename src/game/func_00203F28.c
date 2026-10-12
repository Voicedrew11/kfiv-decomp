#include "kf_state.h"

extern KindObject *(*const D_00440B70[])(int, int);

void func_001FBEB0(KindObject *);
void func_001FC0D8(KindObject *);
void func_001FCBD0(KindObject *);

/* Builds an object with the handler in slot index, then dispatches it by kind. */
void func_00203F28(int owner, int index, int param) {
    KindObject *(*handler)(int, int) = D_00440B70[index];
    KindObject *object = NULL;

    if (handler != NULL) {
        object = handler(owner, param);
    }
    if (object != NULL) {
        switch (object->kind) {
        case 0:
            func_001FBEB0(object);
            break;
        case 1:
            func_001FC0D8(object);
            break;
        case 2:
            func_001FCBD0(object);
            break;
        }
    }
}
