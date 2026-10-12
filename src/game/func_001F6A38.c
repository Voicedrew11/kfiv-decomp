#include "common.h"

extern int (*const D_00440628[])(int);
extern int D_00446D00;
extern int D_00446CFC;
extern u8 D_00446C94;
extern u16 D_00446DE8;
extern int D_0044730C;
extern int D_0044731C;
extern int D_00447320;
extern int D_00447328;

int sceGsSyncPath(int, int);
int sceGsSyncV(int);
void func_0019BDA0(int, int);
void func_001A15C0(void);
void func_0019DC00(void);
void func_001A15B8(void);
void func_001B5460(void);
int func_001D72F0(int);
int func_002179F8(int, int);
s16 func_00206C40(void);
void func_001FBDC8(void);

/* Runs the current mode handler every frame until it reports done, then tears the mode down. */
void func_001F6A38(void) {
    int field;

    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    func_0019BDA0(0x3D00, 0x3F00);
    func_001A15C0();
    func_0019DC00();
    D_00446C94 = 0;
    D_0044731C = D_00447320 = func_002179F8(0, 0);
    while (D_00440628[D_00446D00](0) != 0) {
        sceGsSyncPath(0, 0);
        D_00447328 = func_001D72F0(1);
        func_001B5460();
        field = func_002179F8(0, 0);
        D_0044731C = field;
        if (field != D_00447320) {
            D_0044730C = 1;
        }
        D_00447320 = field;
        func_00206C40();
        if (D_0044730C > 0) {
            if (++D_0044730C >= 0x15) {
                D_0044730C = 0;
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
    D_00446CFC = 0;
    D_00446D00 = 0;
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
}
