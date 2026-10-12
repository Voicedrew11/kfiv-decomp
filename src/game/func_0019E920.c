#include "common.h"

extern u8 D_00349680[];
extern char D_003EF710[];
int func_001CE9A0(int, u32, int);

typedef struct {
    char pad0[0x30];
    int v30;
    char pad1[6];
    char a3a;
    char a3b;
    char a3c;
    char pad2[3];
} Ent;

void func_0019E920(void) {
    Ent *p = (Ent *)D_003EF710;
    int n = 0x7F;
    int v;

    do {
        n--;
        if (p->a3a >= 0 && p->a3c == 0x63) {
            v = (int)((float)p->v30 * (float)D_00349680[2] * 0.015625f);
            func_001CE9A0(0, p->a3b, (v >= 0x80) ? 0x7F : v);
        }
        p++;
    } while (n >= 0);
}
