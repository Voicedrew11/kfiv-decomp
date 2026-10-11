#include "common.h"

extern u8 D_0036A130[];

typedef struct {
    int v;
} __attribute__((packed)) Unal;

int func_001060C0(void) {
    Unal hdr[2];
    int count;
    int i;
    u8 *base;
    u8 *p;
    u8 *tbl;
    u8 *blk;
    int sum;
    int k;

    p = D_0036A130;
    hdr[0] = *(Unal *)D_0036A130;
    base = p;
    p = base + 4;
    if (hdr[0].v != 0x15400) {
        return 0;
    }
    hdr[1] = *(Unal *)p;
    count = (u32)hdr[1].v >> 7;
    blk = base;
    tbl = base + hdr[1].v;
    for (i = 0; i < count; i++) {
        sum = 0;
        for (k = 0x7F; k >= 0; k--) {
            sum += *blk++;
        }
        sum &= 0xFF;
        if (sum != *tbl) {
            return 0;
        }
        tbl++;
    }
    return 1;
}
