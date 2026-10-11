#include "common.h"

typedef struct { int p0; char pad[8]; u8 *tbl; } Obj;
typedef struct { u8 idx; u8 cnt; s16 s; } Ent;

int func_00192100(int, int, u8, s16);

int func_0019A538(Obj *arg0, Ent *arg1, int arg2) {
    u8 *p = arg0->tbl + (arg1->idx & 0x1F);
    u8 v;

    func_00192100(arg0->p0, arg2, p[0x60], arg1->s);
    arg1->cnt = v = arg1->cnt - 1;
    return v;
}
