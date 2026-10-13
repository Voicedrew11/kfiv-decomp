#include "common.h"
#include "kf_shot.h"

char *func_0010CAB8(char *, u32);
void func_001B4F30(u8, void *, void *, void *, void *);
typedef struct { s64 v; } __attribute__((packed)) U64P;

typedef struct {
    char a;
    char pad[0x1F];
    u16 x20;
    s16 x22;
    char pad2[4];
    U64P x28;
} P;

typedef struct {
    char *s0;
    char pad[8];
    char *sC;
    char pad2[0x1A];
    u16 x2A;
} A;

int func_00173D38(A *arg0, u8 *arg1, int arg2, int arg3, void *arg4) {
    u8 *q;
    int i;
    P *p;
    char *r;

    if (arg0->x2A != 0 && arg1[0xC] == 0) {
        return 0;
    }
    q = arg1;
    for (i = 0; i < 3; i++) {
        p = Shot_Alloc();
        p->a = 1;
        p->x20 = 0;
        p->x22 = 0;
        p->x28 = *(U64P *)(arg0->sC + 0x238);
        r = func_0010CAB8(arg0->s0, q[0x11]);
        if (r != 0) {
            func_001B4F30(q[0x10], p, r, arg4, arg0);
        }
        q += 4;
    }
    return 0;
}
