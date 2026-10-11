#include "common.h"

extern u16 D_00446DE8;
extern void **D_005CE840[];
void func_001D3DB0(int, void *);
void func_001D3BE8(int, void *);
s32 func_0019CDC0(s32);

int func_001DFFF8(void) {
    void *var_a1 = 0;

    if (D_005CE840[0] != 0) {
        var_a1 = *D_005CE840[0];
    }
    if (var_a1 != 0) {
        if (D_00446DE8 & 0x80) {
            void *temp_v0 = ((void **)var_a1)[1];
            if (temp_v0 != 0) {
                *D_005CE840[0] = temp_v0;
                func_001D3DB0(0, var_a1);
                func_0019CDC0(1);
                return 1;
            }
        }
        if (D_00446DE8 & 0x20) {
            void *temp_v1 = ((void **)var_a1)[2];
            if (temp_v1 != 0) {
                *D_005CE840[0] = temp_v1;
                func_001D3BE8(0, var_a1);
                func_0019CDC0(1);
                return 1;
            }
        }
    }
    return 0;
}
