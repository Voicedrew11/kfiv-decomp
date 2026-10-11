#include "common.h"

typedef struct {
    char pad0[0x20];
    s64 flags;
    char pad28[0x4];
    int owner;
    char pad30[0x20];
} A;

extern A D_003F1750[];
extern int D_004468CC;

void func_001A2AF8(A *);

void func_001A2BC8(int arg0) {
    A *p;
    s64 m1;
    int i;

    if (D_004468CC > 0) {
        i = 0;
        m1 = 0x8000L << 17;
        p = D_003F1750;
        for (; i < 0x60; i++, p++) {
            if (p->flags & m1) {
                if (p->owner == arg0) {
                    func_001A2AF8(p);
                    if (D_004468CC == 0) {
                        break;
                    }
                }
            }
        }
    }
}
