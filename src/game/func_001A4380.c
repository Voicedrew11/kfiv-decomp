#include "common.h"

void func_001AF298(char *, char *);

void func_001A4380(char *arg0) {
    char tmp[0x10];
    int n;

    n = *(int *)(arg0 + 0x10) - 1;
    *(int *)(arg0 + 0x10) = n;
    if (*(s8 *)(arg0 + 0x14) == 0 && n <= 0) {
        func_001AF298(arg0, tmp);
    }
    *(u16 *)(arg0 + 0x4C) += 1;
}
