typedef float Mat[4][4] __attribute__((aligned(16)));
typedef float Vec[4] __attribute__((aligned(16)));
typedef int IVec[4] __attribute__((aligned(16)));

typedef struct {
    long tag;              /* 0x00 GIF tag */
    long zero;             /* 0x08 */
    int reg;               /* 0x10 = 0x116 */
    unsigned char rgba[4]; /* 0x14 */
    long pad18;            /* 0x18 unused */
    unsigned short x0, y0; /* 0x20 */
    unsigned int z0;       /* 0x24 = -1 */
    int f0;                /* 0x28 */
    int g0;                /* 0x2C */
    unsigned short x1, y1; /* 0x30 */
    unsigned int z1;       /* 0x34 = -1 */
    int f1;                /* 0x38 */
    int g1;                /* 0x3C */
} KfPacket;

typedef struct Obj {
    int *model;            /* 0x00 */
    char pad04[0x18];
    struct Obj *next;      /* 0x1C */
    char pad20[0x1C];
    float height;          /* 0x3C */
    char pad40[0x24];
    float radius;          /* 0x64 */
    char pad68[0x70];
    long flags;            /* 0xD8 */
} Obj;

extern int D_00446F78;
extern Obj *D_004467E0;
extern float D_00420D00[];
extern Mat D_004323E0;
extern Mat D_00431AF0;
extern float D_00444BAC;

void func_00109A50(void);
void func_00109A80(int, long);
void func_00109AD0(void);
void func_0010A100(void *);
void func_00216778(float *, void *);
void func_00216798(Mat);
void func_00216BF0(float *, float *, float *);
void func_00216C48(float *, float *, float *);
void func_00216C78(float *, float *, float *);
void func_00216CA8(float *, float *, float);
void func_002167E8(IVec, float *);
void sceVu0MulMatrix(Mat, Mat, Mat);
void sceVu0Normalize(float *, float *);
float sceVu0InnerProduct(float *, float *);
float sqrtf(float);

void func_00107840(void) {
    Mat m;
    Vec v;
    Vec n;
    IVec xy;
    KfPacket pkt;
    int flag;
    Obj *obj;
    float dist, max, d;
    int xv, yv, i;
    int th;
    unsigned long c;

    flag = 0x1180;
    if (D_00446F78 != 0) flag = 0;
    dist = D_00420D00[18] > 1000.0f ? 1000.0f : D_00420D00[18];
    max = dist * dist;
    for (obj = D_004467E0; obj != 0; obj = obj->next) {
        if (((int)(obj->flags >> 1) & 1) == 0) continue;
        if (dist < obj->radius) continue;
        func_00216778(v, obj->model + 0x30);
        v[1] -= obj->height * 0.5f;
        func_00216C78(v, v, D_00420D00);
        func_00216BF0(n, v, v);
        d = n[0] + n[1] + n[2];
        if (max < d) continue;
        sceVu0Normalize(n, v);
        if (sceVu0InnerProduct(n, D_00420D00 + 4) < D_00444BAC) continue;
        max = d;
        dist = sqrtf(d);
    }

    sceVu0MulMatrix(m, D_004323E0, D_00431AF0);
    func_00216798(m);
    func_00216CA8(n, D_00420D00 + 12, dist);
    func_00216C48(n, n, D_00420D00 + 8);
    n[3] = 1.0f;
    func_002167E8(xy, n);

    xv = xy[0] - 0x8000;
    if (xv < 0) xv = xy[0] - 0x7FFF;
    yv = xy[1] - 0x8000;
    if (yv < 0) yv = xy[1] - 0x7FFF;
    xv = xv / 2;
    yv = yv / 2;
    if (xv > 0x80) xv = 0x80;
    else if (xv < -0x80) xv = -0x80;
    if (yv > 0x80) yv = 0x80;
    else if (yv < -0x80) yv = -0x80;

    func_00109A50();
    func_00109A80(0x47, 0x30000);
    func_00109A80(0x4E, 0x10A000118L);
    func_00109A80(0x22, 3);
    func_00109AD0();

    pkt.reg = 0x116;
    c = flag | 0x28028000L;
    pkt.zero = 0;
    pkt.rgba[0] = 0x80;
    pkt.rgba[1] = 0x80;
    pkt.rgba[2] = 0x80;
    th = 0x29;
    pkt.z0 = 0xFFFFFFFF;
    pkt.tag = c | ((unsigned long)th << 30);
    pkt.rgba[3] = 0x80;
    pkt.z1 = 0xFFFFFFFF;

    if (xv <= 0) {
        pkt.x0 = 0x6BF8;
        pkt.x1 = 0x6E00;
        pkt.f1 = 0x200;
        pkt.f0 = 0;
    } else {
        pkt.x0 = 0x9400;
        pkt.x1 = 0x91F8;
        pkt.f0 = 0x2800;
        pkt.f1 = 0x2600;
    }
    if (yv <= 0) {
        pkt.y0 = 0x71F8;
        pkt.y1 = 0x8E00;
        pkt.g1 = 0x1C00;
        pkt.g0 = 0;
    } else {
        pkt.y0 = 0x8E00;
        pkt.y1 = 0x71F8;
        pkt.g0 = 0x1C00;
        pkt.g1 = 0;
    }
    pkt.x0 += xv;
    pkt.x1 += xv;
    pkt.y0 += yv;
    pkt.y1 += yv;
    if (xv <= 0) xv = 0x200;
    else xv = -0x200;
    for (i = 19; i >= 0; i--) {
        func_0010A100(&pkt);
        pkt.x0 += xv;
        pkt.x1 += xv;
        pkt.f0 += xv;
        pkt.f1 += xv;
    }

    func_00109A50();
    func_00109A80(0x22, 0);
    func_00109A80(0x4E, 0xA000118L);
    func_00109AD0();
}
