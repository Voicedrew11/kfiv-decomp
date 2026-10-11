#include "common.h"

typedef struct {
    char pad0[0x20];
    s64 flags;
    int key;
    char pad2C[0x24];
} A;

typedef struct {
    char pad0[0x40];
    s64 flags;
    char pad48[0x14];
    int key;
    char pad5C[0x40];
} B;

extern A D_003F1750[];
extern B D_003F3550[];
extern int D_004468CC;
extern int D_004468D0;

void func_001A2A18(void *);
void func_001A2AF8(void *);

void func_001A32A8(int arg0) {
    A *p;
    B *q;
    int i;

    p = D_003F1750;
    q = D_003F3550;
    for (i = 0; i < 0x60 && D_004468CC > 0; i++, p++) {
        if (p->flags & (0x8000L << 17)) {
            if (p->key == arg0) {
                func_001A2AF8(p);
            }
        }
    }
    for (i = 0; i < 0x180 && D_004468D0 > 0; i++, q++) {
        if (q->flags & (0x8000L << 41)) {
            if (q->key == arg0) {
                func_001A2A18(q);
            }
        }
    }
}
