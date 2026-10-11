#include "common.h"

int func_001CBAC0(void);
int func_001CBD20(void);
void func_001CEFA0(u32);
extern int D_00446858;
extern u8 D_00446896;
extern char D_004470E2;

int func_0019F220(void *actor, int mode) {
    if (mode != 0x12) {
        if (func_001CBAC0() != 0) {
            return -1;
        }
        if (func_001CBD20() == 0) {
            return -1;
        }
        if (D_00446896 < 4U) {
            func_001CEFA0((char) D_00446896);
            D_004470E2 = TRUE;
        }
    }
    D_00446858 += 1;
    return 0;
}
