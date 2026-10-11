#include "common.h"

void func_001A9E88(char *arg0, int arg1);

void func_001A9F40(char *arg0, int arg1) {
    char *s = arg0 + 0x10;
    int t;

    func_001A9E88(arg0, arg1);
    t = *(int *)0x70000018;
    if (t == 3 || t == 5) {
        *(u16 *)(s + 0x20) = *(u16 *)(*(char **)(arg0 + 0x54) + 0xF2);
    }
}
