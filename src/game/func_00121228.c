#include "common.h"

extern char D_0038CC50[];

s16 func_0011E340(char *, int);
void func_0011E3A0(char *);

void func_00121228(char *arg0) {
    char *temp_a2 = arg0 + 0x40;
    char *g;
    int w;

    if (*(int *)(temp_a2 + 0x1C) > 0) {
        *(int *)(temp_a2 + 0x1C) = *(int *)(temp_a2 + 0x1C) - 1;
    }
    if (*(s16 *)(*(char **)(*(char **)arg0 + 0xFC) + 0x82) != 0) {
        func_0011E3A0(arg0);
        return;
    }
    func_0011E340(arg0, 5);
    arg0[0x11] = 4;
    g = D_0038CC50;
    w = *(u32 *)(g + 0x30);
    *(u32 *)(g + 0x30) = w & 0xFFFFFFFE;
}
