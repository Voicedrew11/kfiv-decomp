typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    int v[4];
} Rect;

extern int D_00446F78;
extern Rect D_0043E480;
extern Rect D_0043E490;

unsigned int *func_00109990(int);
void func_00109650(unsigned int *, unsigned int *);
void func_00109A50(void);
void func_00109A80(int, long);
void func_00109AD0(void);

void func_00107270(int alpha) {
    Rect xy = D_0043E480;
    Rect uv = D_0043E490;
    int col[4] = { 0x80, 0x80, 0x80, alpha };
    int base;
    int th;
    unsigned long c;
    unsigned int *p0;
    unsigned int *p;

    base = 0x1180;
    if (D_00446F78 == 0) {
        base = 0;
    }
    func_00109A50();
    func_00109A80(0x42, 0x44);
    func_00109A80(0x47, 0x51001);
    func_00109AD0();

    p0 = p = func_00109990(0xD);
    *p++ = 0x1000000B;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0x6C09C000;
    *(unsigned long *)p = 0x80AF400000008001L;
    p += 2;
    *(unsigned long *)p = 0x513513EEL;
    p += 2;
    th = 9;
    c = base | 0x28028000L;
    *(unsigned long *)p = c | ((unsigned long)th << 30);
    p += 2;
    *p++ = 6;
    *p++ = 0;
    *(unsigned long *)p = 0;
    p += 2;
    *p++ = 0x14;
    *p++ = 0;
    *p++ = uv.v[0] << 4;
    *p++ = uv.v[1] << 4;
    *p++ = 0;
    *p++ = 0;
    *(u_long128 *)p = *(u_long128 *)col;
    p += 4;
    *p++ = (xy.v[0] + 0x6C0) << 4;
    *p++ = (xy.v[1] + 0x720) << 4;
    *p++ = 0xFFFFFFFF;
    *p++ = 0;
    *p++ = uv.v[2] << 4;
    *p++ = uv.v[3] << 4;
    *p++ = 0;
    *p++ = 0;
    *(u_long128 *)p = *(u_long128 *)col;
    p += 4;
    *p++ = (xy.v[2] << 4) + 0x6BF8;
    *p++ = (xy.v[3] << 4) + 0x71F8;
    *p++ = 0xFFFFFFFF;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0x15000276;
    p[3] = 0;
    p[2] = 0;
    p[1] = 0;
    p[0] = 0;
    func_00109650(p0, p);
}
