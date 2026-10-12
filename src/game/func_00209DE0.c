#include "common.h"

int func_001CBAE0(int, ...);
int func_001CED78(int, char *, int);
extern char D_00441260[];

void func_00209DE0(void) {
    if (func_001CBAE0(1, 0x3090, 0) == 0) {
        func_001CED78(0, D_00441260, 0x10000);
        do {
        } while (func_001CBAE0(1, 0x30B0) == 0);
        func_001CBAE0(1, 0x3020, 0, 1);
    }
}
