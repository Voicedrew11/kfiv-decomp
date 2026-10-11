#include "kf_gs.h"
#include "common.h"

extern int D_00446F78;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A100(void *);

typedef struct {
    s64 tag;              /* 0x00 GIF tag */
    s64 zero;             /* 0x08 */
    int reg;               /* 0x10 = 0x116 */
    u8 rgba[4]; /* 0x14 */
    s64 pad18;            /* 0x18 unused */
    s16 x0, y0;          /* 0x20 */
    u32 z0;       /* 0x24 = -1 */
    int f0;                /* 0x28 scrolled by 0x400 */
    int g0;                /* 0x2C = 0 */
    s16 x1, y1;          /* 0x30 */
    u32 z1;       /* 0x34 = -1 */
    int f1;                /* 0x38 scrolled by 0x400 */
    int g1;                /* 0x3C = 0x1C00 */
} KfPacket;

void func_00107E70(int r, int g, int b) {
    char pad[0x60];
    KfPacket pkt;
    int flag;
    int n;

    flag = D_00446F78 ? 0 : 0x1180;
    n = 9;
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 10);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109AD0();

    pkt.tag = (0x28028000L | flag) | (0x9000L << 18);
    pkt.zero = 0;
    pkt.reg = 0x116;
    pkt.rgba[0] = r;
    pkt.rgba[1] = g;
    pkt.rgba[2] = b;
    pkt.rgba[3] = 0x80;
    pkt.x0 = 0x6C00;
    pkt.y0 = 0x7200;
    pkt.x1 = 0x7000;
    {
        u32 ky = 0x8DF8;
        pkt.y1 = ky;
    }
    pkt.z0 = 0xFFFFFFFFU;
    pkt.z1 = 0xFFFFFFFFU;
    pkt.f1 = 0x400;
    pkt.g1 = 0x1C00;
    pkt.f0 = 0;
    pkt.g0 = 0;

    do {
        n = n - 1;
        func_0010A100(&pkt);
        pkt.x0 += 0x400;
        pkt.x1 += 0x400;
        pkt.f0 += 0x400;
        pkt.f1 += 0x400;
    } while (n >= 0);

    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
    func_00109AD0();
    func_00109E18();
    func_00109E88();
}
