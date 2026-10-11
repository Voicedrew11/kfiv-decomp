#include "common.h"

void func_001241A0(int);
typedef struct {
    union { int w; struct { u8 a; u16 b; } f; } u;
    s16 c; char d; int e; int f;
} Entry;
extern Entry D_003D6DF0[];
int func_00123DD8(int idx) {
    Entry *p;
    p = &D_003D6DF0[idx];
    if (p->u.w & 0x200) { return -1; }
    if (p->u.f.a == 0) {
        func_001241A0(idx);
        return idx;
    }
    if (p->u.f.a == 4 && !(*(int *)((char *)p + 4) & 0xFFFFFF)) {
        func_001241A0(idx);
        if (idx < 4 && p[1].u.f.a == 5) { p++; p->u.f.b = 0xFFFF; p->u.f.a = 0; p->c = 0; p->u.w &= ~0x100; }
        return idx;
    }
    if (p->u.f.a == 5 && func_00123DD8(idx - 1) != -1) {
        func_001241A0(idx);
        return idx;
    }
    func_001241A0(idx);
    return -1;
}
