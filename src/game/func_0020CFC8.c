#include "common.h"

extern f32 D_004323E0[16];

void KF_Vu0_LoadMatrix(void *);
void KF_Vu0_ProjectToScreen(int *, void *);

/* Projects a world-space point to the screen with the current world-to-screen matrix. */
void func_0020CFC8(void *point, int *screen) {
    KF_Vu0_LoadMatrix(D_004323E0);
    KF_Vu0_ProjectToScreen(screen, point);
}
