typedef struct {
    int v[3];
} Offsets;

extern int D_00446F78;
extern Offsets D_0043E460;
extern unsigned int D_00446744;
extern unsigned int D_00446ED4;
extern int D_0038A178[];

void func_00109A50(void);
void func_00109A80(int, long);
void func_00109AD0(void);
void func_0010A100(void *);

struct Pkt {
    long hdr[2];
    int tag;
    unsigned char col[4];
    long pad;
    unsigned short a;
    unsigned short b;
    int c;
    int d;
    int e;
    unsigned short f;
    unsigned short g;
    int h;
    int i;
    int j;
};

void func_001067E0(void) {
    Offsets tbl = D_0043E460;
    struct Pkt pkt;
    int base;
    int th;
    unsigned long c;
    unsigned int i;
    unsigned int k;
    int t;
    int v;

    base = 0x1180;
    if (D_00446F78 != 0) {
        base = 0;
    }
    func_00109A50();
    func_00109A80(0x42, 0x80000000a8L);
    func_00109A80(0x47, 0x51001);
    func_00109A80(8, 5);
    func_00109AD0();
    th = 9;
    c = base | 0x28028000L;
    pkt.hdr[0] = c | ((unsigned long)th << 30);
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
