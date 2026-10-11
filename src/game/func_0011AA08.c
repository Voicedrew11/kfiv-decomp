#include "common.h"

extern int D_00447064;
extern int D_00447068;
extern char D_0038D6B0[];
extern u16 D_00413DA8[];

void func_0020B780(int);
void func_001096E8(void);
void func_001095E0(void);
void func_0010B000(void);
void func_001095D0(void);
void func_0011AAE8(void);
void func_001095D8(void);
void func_001B5460(void);
u8 func_00109800(void);

void func_0011AA08(void) {
    int temp_v1;
    char *temp_s1;

    temp_s1 = (D_00447064 * 0xC) + D_0038D6B0;
    do {
        func_0020B780(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_001095D0();
        func_0011AAE8();
        func_001095D8();
        func_001B5460();
        if ((D_00413DA8[1] & 0x10) && (D_00447068 > 0)) {
            D_00447068 -= 1;
        } else if (D_00413DA8[1] & 0x40) {
            temp_v1 = D_00447068 + 1;
            if (temp_v1 < *(int *)(temp_s1 + 8)) {
                D_00447068 = temp_v1;
            }
        }
        func_00109800();
    } while ((D_00413DA8[1] & 0x2000) == 0);
}
