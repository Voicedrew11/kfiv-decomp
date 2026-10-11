#include "common.h"

int func_001CBD20(void);
void func_001CEFA0(u32);
extern s32 D_00446868;
extern u8 D_00446895;
extern char D_004470E1;
extern char D_004470E2;

int func_001A0F60(void) {
    if (func_001CBD20() == 0) {
        return 0;
    }
    if (D_00446895 < 4U) {
        func_001CEFA0((char) D_00446895);
        D_004470E2 = TRUE;
    }
    D_004470E1 = TRUE;
    D_00446868 += 1;
    return 1;
}
