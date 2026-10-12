#include "common.h"

int sceGsSyncPath(int, int);
int sceGsSyncV(int);
void func_0019BDA0(int, int);
void func_001A15C0(void);
void func_001A15B8(void);
void func_0019DC00(void);
int func_002179F8(int, int);
s32 func_001D72F0(s32);
void func_001B5460(void);
s16 func_00206C40(void);
void func_001FBDC8(void);

extern s8 D_00447318;
extern s8 D_00447313;
extern s8 D_00447314;
extern s8 D_00447315;
extern s8 D_00446C94;
extern s32 D_0044731C;
extern s32 D_00447320;
extern s32 D_00447328;
extern s32 D_00447308;
extern u16 D_00446DE8;
extern s32 D_00446CCC;
extern int (*const D_004404C0[])(s32);

/* Runs the screen loop of the handler D_00446CCC selects until it returns 0, watching the pad type for changes. */
void func_001EE730(void) {
    D_00447318 = 1;
    D_00447313 = 0;
    D_00447314 = 0;
    D_00447315 = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    func_0019BDA0(0x3D00, 0x3F00);
    func_001A15C0();
    func_0019DC00();
    D_00446C94 = 0;
    D_0044731C = D_00447320 = func_002179F8(0, 0);
    while (D_004404C0[D_00446CCC](0) != 0) {
        sceGsSyncPath(0, 0);
        D_00447328 = func_001D72F0(1);
        func_001B5460();
        D_0044731C = func_002179F8(0, 0);
        if (D_0044731C != D_00447320) {
            D_00447308 = 1;
        }
        D_00447320 = D_0044731C;
        func_00206C40();
        if (D_00447308 > 0) {
            D_00447308++;
            if (D_00447308 >= 21) {
                D_00447308 = 0;
            } else {
                D_00446DE8 = 0;
            }
        }
    }
    sceGsSyncPath(0, 0);
    while (sceGsSyncV(0) != 0) {
    }
    func_001A15B8();
    func_001FBDC8();
    D_00446CCC = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}
