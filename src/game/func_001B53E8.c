#include "common.h"

extern s16 D_0044692C;
extern s16 D_0044692E;
extern s16 D_00446930;
extern s16 D_00446932;
extern s16 D_00446934;
extern s16 D_00446936;
extern s16 D_00446938;
extern s16 D_0044693A;
extern u16 D_0044693C;
extern s16 D_0044693E;
extern u16 D_00446940;
extern s16 D_00446942;
extern s16 D_00446944;
extern s16 D_00446946;

/* Store a fixed set of 14 bit-mask constants into consecutive globals. */
void func_001B53E8(void) {
    D_0044692C = 0x10;
    D_0044692E = 0x40;
    D_00446930 = 0x20;
    D_00446932 = 0x80;
    D_00446934 = 0x800;
    D_00446936 = 0x400;
    D_00446938 = 0x100;
    D_0044693A = 0x200;
    D_0044693C = 0x8000;
    D_0044693E = 0x1000;
    D_00446940 = 0x4000;
    D_00446942 = 0x2000;
    D_00446944 = 1;
    D_00446946 = 8;
}
