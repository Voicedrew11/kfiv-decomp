#include "kf_player.h"

extern PlayerState D_00413F80;
extern int D_00447128;
extern f32 D_00447130;
extern int D_00447134;
extern Vector4 D_0057B350;
extern Vector4 D_0057B360;

void func_00216C48(void *, void *, void *);
void func_00216778(void *, void *);
int func_0019B908(float *, float, float);

void func_001BD778(void) {
    if (D_00447134 > 0) {
        D_00447134--;
        if (D_00447134 > 0) {
            func_00216C48(&D_00413F80.position, &D_00413F80.position, &D_0057B360);
        } else {
            func_00216778(&D_00413F80.position, &D_0057B350);
        }
    }
    if (func_0019B908(&D_00413F80.rotation.y, D_00447130, 0.029088821f) && D_00447134 <= 0) {
        D_00447128++;
    }
}
