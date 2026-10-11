#include "kf_gs.h"

typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    int v[4];
} Color;

extern int D_00446F78;
extern unsigned int D_00446744;
extern int D_0038A178[8];
extern Color D_0043E450;

unsigned int *func_00109990(int);
void func_00109650(unsigned int *, unsigned int *);
void func_00109A50(void);
void func_00109A80(int, long);
void func_00109AD0(void);

void func_00106520(void) {
    Color col = D_0043E450;
    int base;
    int th;
    unsigned int i;
    unsigned long c;
    unsigned int *p0;
    unsigned int *p;

    base = 0x1180;
    if (D_00446F78 != 0) {
        base = 0;
    }
    if (D_00446744 == 0) {
        return;
    }
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x80000000A8L);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109A80(8, 5);
    func_00109AD0();

    for (i = 0; i < D_00446744; i++) {
        p0 = p = func_00109990(0xD);
        *p++ = 0x1000000B;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x6C09C000;
        *(unsigned long *)p = 0x80AF400000008001L;
        p += 2;
        *(unsigned long *)p = 0x513513EEL;
        p += 2;
        th = 9;
        c = base | 0x28028000L;
        *(unsigned long *)p = c | ((unsigned long)th << 30);
        p += 2;
        *p++ = 6;
        *p++ = 0;
        *(unsigned long *)p = 0x60;
        p += 2;
        *p++ = 0x14;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *(u_long128 *)p = *(u_long128 *)&col;
        p += 4;
        *p++ = 0x6BF8;
        *p++ = 0x7200;
        *p++ = D_0038A178[i];
        *p++ = 0;
        *p++ = 0x2800;
        *p++ = 0x1C00;
        *p++ = 0;
        *p++ = 0;
        *(u_long128 *)p = *(u_long128 *)&col;
        p += 4;
        *p++ = 0x93F8;
        *p++ = 0x8E00;
        *p++ = D_0038A178[i];
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x15000276;
        func_00109650(p0, p);
    }

    func_00109A50();
    func_00109A80(8, 0);
    func_00109AD0();
}
