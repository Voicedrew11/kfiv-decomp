#include "common.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern float D_00420D10[];

void KF_Vector_Scale(float *, float *, float);
void func_00216C48(void *dst, void *a, void *b);

int func_001B7D80(void) {
    float vec[4];
    int result;

    KF_Vector_Scale(vec, D_00420D10, 100.0f);
    func_00216C48(vec, vec, &D_00413F80);
    vec[1] -= 186.0f;
    result = 0;
    if (vec[1] <= D_00413F80.unk1DC) {
        result = 1;
    }
    return result;
}
