extern int D_00446F78;
extern float D_00420D30[];
extern float D_00420D40[];
extern float D_004323E0[16];
extern float D_00431AF0[16];

void func_00109A50(void);
void func_00109A80(int, long);
void func_00109AD0(void);
void func_0010A100(void *);
void sceVu0MulMatrix(void *, void *, void *);
void func_00216798(void *);
void func_00216CA8(float, void *, void *);
void func_00216C48(void *, void *, void *);
void func_002167E8(int *, void *);

typedef struct {
    long tag;
    long zero;
    int reg;
    unsigned char rgba[4];
    long pad;
    unsigned short x0, y0;
    unsigned int f24;
    int f28;
    int f2c;
    unsigned short x1, y1;
    unsigned int f34;
    int f38;
    int f3c;
} Pkt;

void func_00107528(void) {
    float m[16];
    float v[4];
    int xy[2];
    Pkt pk;
    Pkt *pp;
    int uVar3;
    int x, y, n;

    uVar3 = D_00446F78 ? 0 : 0x1180;

    func_00109A50();
    func_00109A80(0x42, 10);
    func_00109A80(0x47, 0x30000);
    func_00109A80(0x4E, 0x10A000118L);
    func_00109A80(0x22, 3);
    func_00109AD0();

    sceVu0MulMatrix(m, D_004323E0, D_00431AF0);
    func_00216798(m);
    func_00216CA8(1000.0f, v, D_00420D30);
    func_00216C48(v, v, D_00420D30 - 4);
    v[3] = 1.0f;
    func_002167E8(xy, v);

    x = xy[0] - 0x8000;
    if (x < 0) {
        x = xy[0] - 0x7FFF;
    }
    y = xy[1] - 0x8000;
    if (y < 0) {
        y = xy[1] - 0x7FFF;
    }
    x = x / 2;
    y = y / 2;
    if (x > 0x80) {
        x = 0x80;
    } else if (x < -0x80) {
        x = -0x80;
    }
    if (y > 0x80) {
        y = 0x80;
    } else if (y < -0x80) {
        y = -0x80;
    }
    if (D_00420D40[0] < 125.0f) {
        y = 0;
    }

    pk.reg = 0x116;
    pk.tag = 0x28028000L | uVar3;
    pk.tag |= 0xA400L << 20;
    pk.zero = 0;
    pk.rgba[0] = 0x80;
    pk.rgba[1] = 0x80;
    pk.rgba[2] = 0x80;
    pk.rgba[3] = 0x80;
    pk.f24 = 0xFFFFFFFF;
    pk.f34 = 0xFFFFFFFF;

    if (x < 1) {
        pk.x0 = 0x6BF8;
        pk.x1 = 0x6E00;
        pk.f28 = 0;
        pk.f38 = 0x200;
    } else {
        pk.x0 = 0x9400;
        pk.x1 = 0x91F8;
        pk.f28 = 0x2800;
        pk.f38 = 0x2600;
    }
    if (y < 1) {
        pk.y0 = 0x71F8;
        pk.y1 = 0x8E00;
        pk.f2c = 0;
        pk.f3c = 0x1C00;
    } else {
        pk.y0 = 0x8E00;
        pk.y1 = 0x71F8;
        pk.f2c = 0x1C00;
        pk.f3c = 0;
    }

    pk.x0 += x;
    pk.x1 += x;
    pk.y0 += y;
    pk.y1 += y;

    if (x < 1) {
        x = 0x200;
    } else {
        x = -0x200;
    }

    pp = &pk;
    n = 0x13;
    do {
        n--;
        func_0010A100(pp);
        pk.x0 += x;
        pk.x1 += x;
        pk.f28 += x;
        pk.f38 += x;
    } while (n >= 0);

    func_00109A50();
    func_00109A80(0x22, 0);
    func_00109A80(0x4E, 0xA000118L);
    func_00109AD0();
}
