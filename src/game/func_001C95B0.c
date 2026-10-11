#include "common.h"

extern int D_0038CC80[3];
extern void *D_004469D4;

typedef struct KfTarget {
    char pad[0xC4];
    float f0C4;
} KfTarget;

typedef struct KfNode {
    int f00;
    int f04;
    int f08;
    int f0C;
    int f10;
    int f14;
    KfTarget *f18;
    int f1C;
    int f20;
    struct KfNode *f24;
    float f28;
    float f2C;
    float f30;
} KfNode;

void func_001C95B0(void) {
    KfNode *p;
    int flags;
    float s;

    p = (KfNode *)D_004469D4;
    if (!(D_0038CC80[0] & 0x10) && p != 0) {
        do {
            if (p->f04 != 0) {
                flags = p->f0C;
                if (((u32)flags >> 2) & 1) {
                    s = p->f2C + p->f30;
                    p->f2C = s;
                    if (p->f28 <= s) {
                        KfTarget *new_var = p->f18;
                        p->f0C = flags & -5;
                        do { } while (0);
                        new_var->f0C4 = p->f28;
                    } else {
                        p->f18->f0C4 = s;
                        p->f30 = p->f30 + 1.0f;
                    }
                }
            }
            p = p->f24;
        } while (p != 0);
    }
}
