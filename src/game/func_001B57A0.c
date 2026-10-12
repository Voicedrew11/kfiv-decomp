#include "common.h"

extern s8 D_00349682[];
extern u8 D_00446949;
extern u8 D_0044694A;
extern u8 D_0044694B;
extern u8 D_0044694C;
extern u8 D_0044694D;

/* Requests a controller rumble (only when the option at D_00349682[4] is on). */
void func_001B57A0(u8 strength, u8 duration, u8 delay) {
    if (D_00349682[4] != 0) {
        D_0044694A = strength;
        D_00446949 = 1;
        D_0044694B = D_0044694C = duration;
        D_0044694D = delay;
    }
}
