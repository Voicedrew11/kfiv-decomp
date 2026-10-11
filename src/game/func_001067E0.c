#include "kf_gs.h"
#include "common.h"

typedef struct {
    int v[3];
} Offsets;

extern int D_00446F78;
extern Offsets D_0043E460;
extern u32 D_00446744;
extern u32 D_00446ED4;
extern int D_0038A178[];

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);

struct Pkt {
    s64 hdr[2];
    int tag;
    u8 col[4];
    s64 pad;
    u16 a;
    u16 b;
    int c;
    int d;
    int e;
    u16 f;
    u16 g;
    int h;
    int i;
    int j;
};

void func_001067E0(void) {
    Offsets tbl = D_0043E460;
    struct Pkt pkt;
    int base;
    int th;
    u64 c;
    u32 i;
    u32 k;
    int t;
    int v;

    base = 0x1180;
    if (D_00446F78 != 0) {
        base = 0;
    }
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x80000000a8L);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109A80(8, 5);
    func_00109AD0();
    th = 9;
    c = base | 0x28028000L;
    pkt.hdr[0] = c | ((u64)th << 30);
    pkt.hdr[1] = 0x60L;
    pkt.tag = 0x156;
    pkt.col[0] = 0x80;
    pkt.col[1] = 0x80;
    pkt.col[2] = 0x80;
    pkt.col[3] = 0x80;
    i = 0;
    if (D_00446744 != 0) {
        do {
            k = D_00446ED4 + i;
            t = tbl.v[k % 3];
            v = D_0038A178[i];
            pkt.i = 0x2800;
            pkt.a = t + 0x6bf8;
            pkt.j = 0x1c00;
            pkt.d = 0;
            pkt.e = 0;
            pkt.c = v;
            pkt.b = 0x7200;
            pkt.g = 0x8e00;
            pkt.f = t - 0x6c08;
            pkt.h = v;
            func_0010A100(&pkt);
            i++;
        } while (i < D_00446744);
    }
    func_00109A50();
    func_00109A80(8, 0);
    func_00109AD0();
}
