#include "kf_ent.h"

void *memset(void *, int, unsigned int);
extern int D_00446F14;
extern int D_00446F60;
extern int D_0044709C;
extern int D_004467E4;
extern int D_004467E8;
extern int D_004470AC;
extern int D_004470B4;
extern int D_004470B8;
extern int D_004470BC;
extern int D_004470C0;
extern char D_003BE530[];
extern int D_004470A0[2];

typedef struct {
    char a;
    unsigned short b;
    short c;
    char d;
    int e;
    int f;
} Entry;

extern Entry D_003D6DF0[];

void func_00123030(void) {
    Entry *s;
    int v;
    int i;

    memset((void *)D_00446F60, 0, D_00446F14);

    memset(D_003D6DF0, 0, 0x50);
    D_004470A8 = 0;
    D_004467E8 = -1;
    D_004470B0 = 0;
    D_004467E0 = 0;
    D_004470AC = 0;
    D_004470B4 = 0;
    D_004467E4 = 0;
    memset(D_004470A0, 0, 8);
    D_004470B8 = 0;
    D_004470BC = 0;
    D_004470C0 = 0;
    memset(D_003BE530, 0, 0x8C0);
    v = D_00446F60;
    s = D_003D6DF0; i = 4;
    do {
        s->f = v;
        i -= 1;
        s->a = 0;
        s->b = 0xFFFF;
        v += 0x80000;
        s->c = 0;
        s->d = 0;
        s->e = 0;
        s++;
    } while (i >= 0);
    D_0044709C = v;
}
