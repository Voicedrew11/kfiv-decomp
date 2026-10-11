#include "common.h"

extern char D_003F3550[];
void func_001A2A18(void *);

typedef struct {
    char pad0[0x40];
    u64 flags;
    char pad48[0xC];
    void *p54;
    char pad58[0xA0 - 0x58];
} B;

typedef struct {
    char pad0[0x100];
    u16 u100;
} K;

typedef struct {
    char pad0[0x4C];
    u16 u4C;
} A;

void func_001A9AE0(A *arg0) {
    B *q;
    int i;

    q = (B *)D_003F3550;
    for (i = 0; i < 0x180; i++, q++) {
        if (q->flags & (0x8000L << 41)) {
            if (((K *)q->p54)->u100 == 0x23) {
                arg0->u4C++;
                return;
            }
        }
    }
    func_001A2A18(arg0);
}
