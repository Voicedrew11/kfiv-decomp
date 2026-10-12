#include "common.h"

extern int D_0038CC50[];
extern char D_0038CD20[];
extern int D_00446770;
extern int D_00447064;
extern int D_0044706C;
extern int D_00447070;

int func_0011C678(u8);
void func_001CFA70(int, int, int, int, int, int, ...);

void func_0011A438(int arg0) {
    u32 temp_s0;
    char *row;

    if (!(((u32)D_0038CC50[9] >> 8) & 1) && ((temp_s0 = arg0 - 1) < 5U)) {
        if (arg0 == 5) {
            row = D_0038CD20 + D_00446770 * 0x60;
            if (func_0011C678(row[1]) == 0) {
                func_001CFA70(0, 0, 0, 1, 0x2D, 0xB);
                return;
            }
        }
        D_00447064 = temp_s0;
        D_0044706C = 0;
        D_00447070 = 0;
        D_0038CC50[9] |= 0x100;
    }
}
