extern int D_00446F78;

void func_00109A50(void);
void func_00109A80(int, long);
void func_00109AD0(void);
void func_0010A100(void *);

typedef struct {
    long tag;              /* 0x00 GIF tag */
    long zero;             /* 0x08 */
    int ctl;               /* 0x10 */
    unsigned char rgba[4]; /* 0x14 */
    unsigned char pad[8];  /* 0x18 */
    unsigned short x0;     /* 0x20 */
    unsigned short y0;     /* 0x22 */
    unsigned int u0;       /* 0x24 */
    int w;                 /* 0x28 */
    int h;                 /* 0x2c */
    unsigned short x1;     /* 0x30 */
    unsigned short y1;     /* 0x32 */
    unsigned int u1;       /* 0x34 */
    int v1;                /* 0x38 */
    int z;                 /* 0x3c */
} SpritePkt;

void func_00106DC8(void) {
    SpritePkt p;
    int uVar1;
    int n;
    int th;
    unsigned long c;

    uVar1 = 0x1180;
    if (D_00446F78 != 0) {
        uVar1 = 0;
    }
    func_00109A50();
    n = 9;
    func_00109A80(0x3f, 0xffffffffL);
    func_00109A80(0x42, 0x44);
    func_00109A80(0x47, 0x30000);
    func_00109A80(0x4e, 0x10a000118L);
    func_00109AD0();

    th = 9;
    c = uVar1 | 0x28028000L;
    p.zero = 0;
    p.tag = c | ((unsigned long)th << 30);
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

    p.x1 = 0x7000;
    n = 9;
    p.x0 = 0x6bf8;
    p.y1 = 0x8df0;
    p.y0 = 0x71e8;
    p.v1 = 0x400;
    p.z = 0x1c00;
    p.w = 0;
    p.h = 0;
    do {
        n = n - 1;
        func_0010A100(&p);
        p.x0 += 0x400;
        p.x1 += 0x400;
        p.w += 0x400;
        p.v1 += 0x400;
    } while (n >= 0);

    func_00109A50();
    func_00109A80(0x4e, 0xa000118L);
    func_00109AD0();
}
