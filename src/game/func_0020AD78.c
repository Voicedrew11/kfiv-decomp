#include "common.h"

extern int D_00446E90;
extern int D_00446F20;
extern int D_00446F50;
extern int D_00446F78;
extern int D_00446F7C;
extern int D_004412E0[];
void D_0020AD30(void);
extern s64 D_00432E20[];
extern s64 D_00433050[];

int func_0021B540(char *, u32, u32, int (*)(void), u32, s16, s16, s16);
void func_001095E0(void);
void func_0010B000(void);
void func_001095D0(void);
void func_0010A008(void);
void func_0010A4E8(s64, s64, s64, s64, s64);
void func_001095D8(void);
void func_001096E8(void);
void func_00109800(void);
void func_0020B780(int);
void func_001CB9A0(void);
void func_0019C300(void);

void func_0020AD78(void) {
    s64 temp;
    char buf[0x40];

    D_00446E90 = 0;
    func_0021B540(D_004412E0, D_00446F50, D_00446F20, D_0020AD30, 0x3FFF, 2, 0, 0);
    temp = (s64)0xC000 << 17;
    D_00432E20[0] = temp;
    D_00432E20[30] = temp;
    D_00433050[0] = temp;
    D_00433050[30] = temp;
    func_001095E0();
    func_001095E0();
    func_0010B000();
    func_001095D0();
    D_00446F78 = 0;
    D_00446F7C = 0;
    func_0010A008();
    func_0010A4E8(0, 0, 0, 0x80, 0x100000044L);
    func_001095D8();
    D_00446F78 = 1;
    D_00446F7C = 1;
    func_001096E8();
    func_00109800();
    func_0020B780(0);
    func_001CB9A0();
    func_0019C300();
}
