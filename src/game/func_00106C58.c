#include "kf_gs.h"
#include "common.h"

extern int D_00446F78;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);

typedef struct {
    s64 tag;              /* 0x00 GIF tag */
    s64 zero;             /* 0x08 */
    int ctl;               /* 0x10 */
    u8 rgba[4]; /* 0x14 */
    u8 pad[8];  /* 0x18 */
    u16 x0;     /* 0x20 */
    u16 y0;     /* 0x22 */
    u32 u0;       /* 0x24 */
    int w;                 /* 0x28 */
    int h;                 /* 0x2c */
    u16 x1;     /* 0x30 */
    u16 y1;     /* 0x32 */
    u32 u1;       /* 0x34 */
    int v1;                /* 0x38 */
    int z;                 /* 0x3c */
} SpritePkt;

void func_00106C58(void) {
    SpritePkt p;
    int uVar1;
    int n;
    int th;
    u64 c;

    uVar1 = 0x1180;
    if (D_00446F78 != 0) {
        uVar1 = 0;
    }
    func_00109A50();
    n = 9;
    func_00109A80(GS_REG_TEXFLUSH, 0xffffffffL);
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10a000118L);
    func_00109AD0();
    th = 9;
    c = uVar1 | 0x28028000L;
    p.zero = 0;
    p.tag = c | ((u64)th << 30);
    p.rgba[0] = 0x80;
    p.rgba[1] = 0x80;
    p.u1 = 0xFFFFFFFF;
    p.ctl = 0x156;
    p.rgba[2] = 0x80;
    p.v1 = 0;
    p.rgba[3] = 0x40;
    p.z = 0;
    p.x1 = 0x6c00;
    p.y1 = 0x7210;
    p.x0 = 0x6ff8;
    p.y0 = 0x8e08;
    p.u0 = 0xFFFFFFFF;
    p.w = 0x400;
    p.h = 0x1c00;
    do {
        n = n - 1;
        func_0010A100(&p);
        p.x0 += 0x400;
        p.x1 += 0x400;
        p.w += 0x400;
        p.v1 += 0x400;
    } while (n >= 0);

    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xa000118L);
    func_00109AD0();
}
