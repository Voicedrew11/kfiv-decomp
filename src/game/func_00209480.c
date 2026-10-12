#include "kf_model.h"
#include "kf_splash.h"

extern int D_00447390;
extern ModelInstance *D_0044739C;
extern ItemUseEffectDef *D_004473A4;

void func_00108AC8(void *);

void func_00209480(void) {
    func_00108AC8(D_0044739C);
    if (MODEL_CHANNEL(D_0044739C)->param >= D_004473A4->unk00) {
        D_00447390++;
    }
}
