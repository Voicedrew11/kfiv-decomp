#include "common.h"

typedef struct {
    char pad[0xC0];
    float f0;
    float f4;
    float f8;
} Ctl;

typedef struct {
    char pad[0x44];
    s16 s44;
} Sub;

typedef struct {
    Ctl *p0;
    int pad4;
    Sub *p8;
    int pad0C;
    union { int flags; struct { char f0; u8 st; } b; } u;
} Obj;

extern s64 D_0043E780[];
s16 func_0011E340(void *, int);
void func_001C9340(int, s16, void *, void *, void *, void *);

void func_0011FCF8(Obj *arg0) {
    float sp[3];
    s64 sp10[2];
    Ctl *c;
    Sub *s;

    sp10[0] = D_0043E780[0];
    sp10[1] = D_0043E780[1];
    if (!(arg0->u.flags & 0x20000)) {
        func_0011E340(arg0, 4);
        arg0->u.b.st = 1;
    } else {
        func_0011E340(arg0, 5);
        s = arg0->p8;
        arg0->u.b.st = 3;
        if (s->s44 != 0) {
            if (!(arg0->u.flags & 0x40000)) {
                c = arg0->p0;
                sp[0] = c->f0;
                sp[1] = c->f4 - 25.0f;
                sp[2] = c->f8;
                func_001C9340((u8) arg0->u.flags & 1, s->s44, sp, (char *) c + 0xD0, sp10, arg0);
            }
        }
    }
}
