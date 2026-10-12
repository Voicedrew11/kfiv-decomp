#include "kf_model.h"
#include "kf_splash.h"

extern int D_00447388;
extern int D_0044738C;
extern int D_00447390;
extern ModelInstance *D_0044739C;
extern int D_004473A0;
extern ItemUseEffectDef *D_004473A4;
extern int D_004473A8;

/* Clears the state of the item effect. */
void func_00208FF8(void) {
    D_00447388 = 0;
    D_0044738C = 0;
    D_00447390 = 0;
    D_0044739C = NULL;
    D_004473A0 = 0;
    D_004473A8 = 0;
    D_004473A4 = NULL;
}
