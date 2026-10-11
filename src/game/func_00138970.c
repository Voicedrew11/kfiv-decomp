#include "common.h"

int func_00192C98(void *, int, int, u32, int);

typedef struct {
    char pad0[0x1C];
    int f1C;
} Sub;

typedef struct {
    char pad0[0xC];
    struct {
        char pad0[0xC4];
        u8 c4;
    } *c;
    char pad10[0xC8];
    u64 b0 : 18;
    u64 b18 : 1;
    u64 b19 : 45;
    Sub sub;
} Actor;

void func_00138970(Actor *a) {
    Sub *s = &a->sub;

    func_00192C98(a, 1, 0, a->c->c4, 0);
    s->f1C = 0;
    a->b18 = 1;
}
