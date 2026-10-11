#include "common.h"

void *memset(void *, int, u32);
typedef struct {
    char a0, a1, pad[5], a7;
} TblA;
typedef struct {
    char pad00[0x60];
    s64 a60;
    int a68, a6c, a70, a74, a78;
    int pad7c;
    s64 a80;
    int a88;
} Blk;
extern TblA D_0038CC50[];
extern Blk D_0038A1B0[];
extern float D_004325A0[];

void func_001127F8(void) {
    memset(D_0038CC50, 0, 0x48);
    D_0038CC50->a7 = 1;
    D_0038CC50->a0 = 1;
    D_0038CC50->a1 = 1;
    memset(D_0038A1B0, 0, 0x2AA0);
    D_0038A1B0->a60 |= (s64)0x8000 << 31;
    D_0038A1B0->a60 |= (s64)0x8000 << 27;
    D_0038A1B0->a60 |= (s64)0x8000 << 30;
    D_0038A1B0->a88 = 0x40;
    D_0038A1B0->a68 = 1;
    D_0038A1B0->a6c = 0;
    D_0038A1B0->a70 = 0;
    D_0038A1B0->a74 = 0;
    D_0038A1B0->a78 = 0;
    D_0038A1B0->a80 = 0x44;
    memset(D_004325A0, 0, 0x80);
    D_004325A0[18] = 512.0f;
}
