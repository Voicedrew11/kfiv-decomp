#include "kf_state.h"

extern KindObject *(*const D_00440B70[])(int, int);

void func_001FBEB0(KindObject *);
void func_001FC0D8(KindObject *);
void func_001FCBD0(KindObject *);
KindObject *func_00207428(KindObject *, int);

/* Builds an object with the handler for entry_id % 1000, then dispatches it by kind. */
void func_00207478(int owner, int entry_id, int param, int variant) {
    KindObject *(*handler)(int, int) = D_00440B70[entry_id % 1000];
    KindObject *object = NULL;

    if (handler != NULL) {
        object = handler(owner, param);
    }
    if (object != NULL) {
        if (entry_id >= 1000) {
            object = func_00207428(object, variant);
        }
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
