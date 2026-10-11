#include "common.h"

typedef struct {
    int f0;
    int pad4;
    int f8;
} S;

extern u8 D_00349682[];
void func_0019C820(int);

void func_002121C0(S *arg0) {
    if (arg0->f8 >= 0 && arg0->f8 <= 0x40) {
        func_0019C820(D_00349682[0] * arg0->f8 / 0x40);
    }
    arg0->f0 = 0;
}
