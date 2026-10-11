#include "common.h"

typedef struct {
    char *ctx;
    char pad4[8];
    u8 *c;
    char pad10[0x18];
    u16 s28;
    char pad2a[0x9C];
    u16 sc6;
} Act;

typedef struct {
    char pad0[0x40];
    float f40;
    float f44;
    float f48;
} Sub;

typedef struct {
    char pad0[0x130];
    Sub *sub;
} Ctx;

typedef struct {
    u32 idx : 5;
    u32 flag : 1;
    u32 pad : 2;
    u32 b1 : 8;
    u32 s2 : 16;
    int w4;
} Cmd;

int func_001914E8(char *, int, int, int);

int func_0019A448(Act *arg0, Cmd *arg1, int arg2, int arg3, int arg4, int arg5, int arg6) {
    u8 *base = arg0->c + 0x60;
    int v = base[arg3];
    int cur = base[*(u8 *)arg1 & 0x1F];

    if (arg6 == 0 && cur == v) {
        return 1;
    }
    arg1->w4 = arg4 << 8;
    arg1->idx = arg3;
    arg1->s2 = arg4;
    if (arg0->s28 != 0 && (arg0->sc6 & 0x46) == 0) {
        float f = arg5;
        arg1->b1 = f + f * 0.75f;
    } else {
        arg1->b1 = arg5;
    }
    if (*(int *)arg1 & 0x20) {
        Sub *s = ((Ctx *)arg0->ctx)->sub;
        s->f40 = s->f44 = s->f48 = 0.0f;
    }
    return func_001914E8(arg0->ctx, arg2, v, arg4);
}
