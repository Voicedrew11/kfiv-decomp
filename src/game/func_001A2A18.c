#include "common.h"

extern int D_004468C8;
extern int D_004468D0;
void func_0019AA50(s16);
void func_0019CD70(s8);
void func_001A2B28(void *);
void func_001A2BC8(void *);

typedef struct {
    char pad0[0x40];
    u64 b0 : 32;
    s16 h44;
    s8 c46;
    u64 b56 : 1;
    u64 b57 : 1;
    u64 b58 : 1;
    u64 b59 : 5;
    char pad48[0x48];
    int f90;
} Obj;

void func_001A2A18(Obj *a) {
    a->b56 = 0;
    a->f90 = 4;
    D_004468D0--;
    if (a->c46 > 0) {
        D_004468C8--;
    }
    func_0019CD70(a->c46);
    a->c46 = -1;
    if (a->h44 != -1) {
        func_0019AA50(a->h44);
    }
    if (*(s64 *)((char *)a + 0x40) & (0x8000L << 42)) {
        func_001A2BC8(a);
    }
    if (*(s64 *)((char *)a + 0x40) & (0x8000L << 43)) {
        func_001A2B28(a);
    }
}
