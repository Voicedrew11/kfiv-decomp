#include "common.h"

typedef struct {
    char pad0[0xC];
    u16 h0C;
    u8 b0E;
    char pad1[0x14 - 0xF];
} Rec;

extern char D_00358928[];
extern char D_0041A6F8[];

int func_001CA788(int arg0) {
    int i;
    int j;
    Rec *e;
    char *q;
    char *base = D_00358928;

    q = D_0041A6F8 + arg0 * 0x20;
    for (i = 0; i < 0x80; i++) {
        for (j = 0, e = (Rec *)(i * 0x140 + (int)base); j < 0x10; j++) {
            if (e->b0E != 0 && e->h0C == arg0) {
                if (arg0 >= 0x96 || (*(u16 *)(q + 0xE) >> 1 & 1)) {
                    return 0;
                }
            }
            e++;
        }
    }
    return 1;
}
