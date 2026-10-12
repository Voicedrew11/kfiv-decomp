#include "kf_model.h"

extern int D_00447388;
extern int D_00447390;
extern ModelInstance *D_0044739C;

void func_0010B018(ModelInstance *, s32, s32);

void func_002095A0(void) {
    if (D_00447388 != 0 && D_00447390 > 0 && D_0044739C != NULL) {
        func_0010B018(D_0044739C, 0, 0);
    }
}
