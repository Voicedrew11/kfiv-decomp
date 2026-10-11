#include "common.h"

extern int D_004468D0;
extern char D_003F3550[];
void func_001A2A18(void *);

typedef struct {
    char pad0[0x40];
    u64 flags;
    char pad48[0x18];
    int f60;
    char pad64[0xA0 - 0x64];
} B;

void func_001A2B28(int arg0) {
    B *q;
    int i;

    if (D_004468D0 > 0) {
        q = (B *)D_003F3550;
        for (i = 0; i < 0x180; i++, q++) {
            if (q->flags & (0x8000L << 41)) {
                if (q->f60 == arg0) {
                    func_001A2A18(q);
                    if (D_004468D0 == 0) {
                        break;
                    }
                }
            }
        }
    }
}
