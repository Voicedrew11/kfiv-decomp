#include "common.h"

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
    char pad0[0xC];
    char *pC;
} A;

void *func_001B1390();
int func_001B4F30(u16, void *, void *, void *, void *);

int func_00195E48(A *arg0, int arg1, void *arg2, void *arg3) {
    P *p;

    p = func_001B1390();
    p->x22 = p->x20 = 0;
    p->a = 1;
    p->x28 = *(U64P *)(arg0->pC + 0x238);
    return func_001B4F30((u16)arg1, p, arg2, arg3, arg0);
}
