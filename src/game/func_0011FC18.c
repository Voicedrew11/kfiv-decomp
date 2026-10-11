#include "common.h"

typedef struct Ctl Ctl;
struct Ctl {
    char pad[0xD0];
    char sD0[0x1B4 - 0xD0];
    void (*f1B4)(Ctl *);
    void *(*f1B8)(Ctl *, int);
};

typedef struct {
    char pad[0x44];
    s16 s44;
} Sub;

typedef struct Obj Obj;
struct Obj {
    Ctl *p0;
    u8 *p4;
    Sub *p8;
    int pad0C;
    union { int flags; struct { char f0; u8 st; } b; } u;
};

extern s64 D_0043E780[];
extern void (*D_003BE3B0[])(Obj *);
void func_001C9340(int, s16, void *, void *, void *, void *);

void func_0011FC18(Obj *arg0) {
    s64 sp10[2];

    sp10[0] = D_0043E780[0];
    sp10[1] = D_0043E780[1];
    if (arg0->p8->s44 != 0) {
        if (!(arg0->u.flags & 0x40000)) {
            D_003BE3B0[*arg0->p4](arg0);
            arg0->p0->f1B4(arg0->p0);
            func_001C9340((u8) arg0->u.flags & 1, arg0->p8->s44,
                          arg0->p0->f1B8(arg0->p0, 0), arg0->p0->sD0, sp10, arg0);
        }
    }
}
