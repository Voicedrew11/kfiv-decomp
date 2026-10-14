#include "common.h"

extern u32 D_0038CC74[];
extern char D_00441300[];
extern char D_00441320[];
extern int D_00446E90;
extern int D_00446F20;
extern int D_00446F50;

int Movie_Play(char *, u32, u32, int (*)(void), u32, s16, s16, s16);
int sceGsSyncV(int);
void func_0019BDA0(int, int);
void func_001CB9A0(void);
void func_0019C300(void);

void func_0020AE68(void) {
    char buf[0x40]; /* unused, but retail reserves the 0x40 bytes */

    D_00446E90 = 0;
    if ((D_0038CC74[0] >> 11) & 1) {
        Movie_Play(D_00441300, D_00446F50, D_00446F20, 0, 0x3FFF, 2, 0, 0);
    } else {
        Movie_Play(D_00441320, D_00446F50, D_00446F20, 0, 0x3FFF, 2, 0, 0);
    }
    sceGsSyncV(0);
    sceGsSyncV(0);
    func_0019BDA0(0, 0x4000);
    func_001CB9A0();
    func_0019C300();
}
