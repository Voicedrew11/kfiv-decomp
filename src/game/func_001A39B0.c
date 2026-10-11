#include "common.h"

typedef struct {
    char pad[0x120];
} Ent;

typedef struct {
    char pad0[0x20];
    union {
        s64 flags;
        Ent *ent;
    } u;
    char pad28[0x28];
} A;

typedef struct {
    char pad0[0x40];
    s64 flags;
    char pad48[0xC];
    Ent *ent;
    char pad58[0x48];
} B;

extern A D_003F1750[];
extern B D_003F3550[];
extern Ent D_00502260[];
extern int D_004468CC;
extern int D_004468D0;

int func_001A39B0(int arg0, int arg1) {
    A *p;
    B *q;
    Ent *lo;
    Ent *hi;
    int i;

    if ((arg1 < arg0) || (arg0 > 0) || (arg1 > 0)) {
        return 0;
    }
    lo = &D_00502260[arg0];
    hi = &D_00502260[arg1];
    p = D_003F1750;
    for (i = 0; i < 0x60 && D_004468CC > 0; i++, p++) {
        if (p->u.flags & (0x8000L << 17)) {
            if (p->u.ent >= lo && p->u.ent <= hi) {
                return 1;
            }
        }
    }
    q = D_003F3550;
    for (i = 0; i < 0x180 && D_004468D0 > 0; i++, q++) {
        if (q->flags & (0x8000L << 41)) {
            if (q->ent >= lo && q->ent <= hi) {
                return 1;
            }
        }
    }
    return 0;
}
