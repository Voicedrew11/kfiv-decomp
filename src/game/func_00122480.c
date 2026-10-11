#include "common.h"

typedef struct {
    void *p0;
    char pad4[0xC];
    char b10;
    char b11;
} Actor;

typedef struct {
    char pad[0xF8];
    int f;
} Ctl;

typedef struct {
    int p0[4];
    int a10;
    int a14;
    int pad18;
    char a1c;
} Sub;

extern int D_0038CC50[];
void func_00216C48(void *, void *, void *);
void func_00122600(void *, int);
s16 func_0011E340(void *, int);

void func_00122480(Actor *arg0) {
    Sub *s;
    int temp_a0;
    int temp_v0;

    s = (Sub *)((char *)arg0 + 0x40);
    temp_a0 = ((Ctl *)arg0->p0)->f + (s->a14 << 7);
    if (s->a1c > 0) {
        s->a1c = (u8)s->a1c - 1;
    }
    temp_a0 += 0x40;
    func_00216C48((void *)temp_a0, (void *)temp_a0, s);
    temp_v0 = s->a10 - 1;
    s->a10 = temp_v0;
    if (temp_v0 <= 0) {
        func_00122600(arg0, 0);
        func_0011E340(arg0, 4);
        arg0->b11 = 1;
        D_0038CC50[12] &= 0xFFFFFFFE;
    }
}
