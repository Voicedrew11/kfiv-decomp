#include "common.h"

extern char D_0038CC50[];
extern int D_0044705C;
extern int D_00447060;
extern int D_00447064;
extern int D_00447068;
extern int D_0044706C;
extern int D_00447070;
extern char D_00472080[];

void *memset(void *, int, u32);

void func_0011A598(void) {
    int *p = (int *)D_0038CC50;

    D_00447064 = 0;
    D_00447068 = 0;
    p[9] = p[9] & -0x101;
    D_0044705C = 0;
    D_00447060 = 0;
    D_0044706C = 0;
    D_00447070 = 0;
    memset(D_00472080, 0, 0x1C0);
}
