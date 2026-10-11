#include "common.h"

typedef struct {
    char pad0[0x10];
    u32 b0 : 8;
    u32 b8 : 8;
    u32 b16 : 4;
    u32 f20 : 1;
    u32 f21 : 1;
    u32 f22 : 1;
    u32 b23 : 1;
    u32 b24 : 8;
} Rec;

void func_0011E758(Rec *arg0, int arg1) {
    if (arg0->f20 != arg1) {
        arg0->f20 = arg1;
        arg0->f21 = 0;
        arg0->f22 = 0;
        arg0->b24 = 0;
        arg0->b8 = 0;
    }
}
