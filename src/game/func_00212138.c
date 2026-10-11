#include "common.h"

typedef struct {
    int f0;
    int pad4;
    int f8;
} S;

extern u8 D_00349680[];
void func_0019C7F0(int);
void func_0019C820(int);

void func_00212138(S *arg0) {
    if (arg0->f8 >= 0 && arg0->f8 <= 0x40) {
        func_0019C7F0(D_00349680[1] * arg0->f8 / 0x40);
        func_0019C820(D_00349680[2] * arg0->f8 / 0x40);
    }
    arg0->f0 = 0;
}
