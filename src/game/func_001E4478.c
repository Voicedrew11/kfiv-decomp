#include "kf_pad.h"

extern int D_004472D0;
extern void *D_00446B20;
void func_001D7C30(void);
s32 func_0019CDC0(s32);

void func_001E4478(void) {
    if (KF_Pad_GetReport(0, 0)->pressed & 0x2000) {
        if (D_004472D0 != 0xFF) {
            D_00446B20 = (void *)func_001D7C30;
            func_0019CDC0(2);
        }
    }
}
