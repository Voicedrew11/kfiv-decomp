#include "common.h"

typedef struct Sub {
    char pad0[0xEE];
    u16 lo;
    u16 hi;
} Sub;

typedef struct Inner {
    char pad0[0x25];
    char f25;
    char f26;
} Inner;

typedef struct Obj {
    char pad0[0x10];
    Inner in;
    char pad1[0x14];
    u16 cur;
    char pad2[0x6];
    Sub *sub;
} Obj;

int func_001A2928(int, void *, int, void *);

void func_001A9F98(Obj *arg0, int arg1, int arg2) {
    Inner *s;
    Sub *sub;
    int var;

    s = &arg0->in;
    sub = arg0->sub;
    var = sub->hi;
    if (arg0->cur < sub->lo) {
        var += sub->lo - arg0->cur;
    }
    s->f26 = var;
    arg0->cur = (char) var;
    func_001A2928(arg2, arg0, arg1, sub);
    s->f25 = 1;
}
