#include "common.h"

typedef struct {
    char pad[0x24];
    u32 b0 : 1;
    u32 b1 : 1;
    u32 b2 : 1;
    u32 b3 : 1;
    u32 b4 : 1;
    u32 b5 : 1;
    u32 b6 : 1;
    u32 b7 : 1;
    u32 b8 : 1;
    u32 b9 : 5;
    u32 b14 : 1;
} A;

extern A D_0038CC50;

void func_001187B0(void);

void func_001020F8(void) {
    if (D_0038CC50.b2 || D_0038CC50.b3 || D_0038CC50.b4 || D_0038CC50.b5 ||
        D_0038CC50.b8 || D_0038CC50.b14) {
        D_0038CC50.b2 = 0;
        D_0038CC50.b3 = 0;
        D_0038CC50.b5 = 0;
        D_0038CC50.b4 = 0;
        D_0038CC50.b8 = 0;
        D_0038CC50.b14 = 0;
        func_001187B0();
    }
}
