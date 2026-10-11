#include "kf_gs.h"

typedef struct {
    int v[8][11][2];
} Grid __attribute__((aligned(16)));

extern int D_00446740;
extern int D_00467680[8][11][2];
extern int D_00467940[8][11][2];
extern int D_00467C00[8][11][2];
extern float D_00467EC0[8][11][2];

int rand(void);
unsigned int fptoui(float);
unsigned int *func_00109990(int);
void func_00109650(unsigned int *, unsigned int *);
void func_00109E18(void);
void func_00109A50(void);
void func_00109A80(int, long);
void func_00109AD0(void);

void func_00108020(int a0, int a1, int a2, int a3, float t) {
    char buf[0x40];
    int j, i, k, l;
    int x, y, b;
    long t0, t1;
    float dx, dy;
    unsigned int *p0;
    unsigned int *p;

    if (D_00446740 != 0) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 11; j++) {
                D_00467680[i][j][0] = D_00467940[i][j][0] = j << 6;
                D_00467680[i][j][1] = D_00467940[i][j][1] = i << 6;
                if (j > 0 && j < 10) {
                    D_00467940[i][j][0] -= 8;
                    D_00467940[i][j][0] += (int)(rand() * 17.0f / 2147483648.0f);
                }
                if (i > 0 && i < 7) {
                    D_00467940[i][j][1] -= 8;
                    D_00467940[i][j][1] += (int)(rand() * 17.0f / 2147483648.0f);
                }
                D_00467940[i][j][0] <<= 4;
                D_00467940[i][j][1] <<= 4;
                D_00467680[i][j][0] <<= 4;
                D_00467680[i][j][1] <<= 4;
            }
        }
    }

    *(Grid *)D_00467C00 = *(Grid *)D_00467940;

    for (i = 1; i < 7; i++) {
        for (j = 1; j < 10; j++) {
            dx = dy = 0.0f;
            dx += D_00467C00[i - 1][j][0] * 0.1f;
            dy += D_00467C00[i - 1][j][1] * 0.1f;
            dx += D_00467C00[i + 1][j][0] * 0.1f;
            dy += D_00467C00[i + 1][j][1] * 0.1f;
            dx += D_00467C00[i][j - 1][0] * 0.1f;
            dy += D_00467C00[i][j - 1][1] * 0.1f;
            dx += D_00467C00[i][j + 1][0] * 0.1f;
            dy += D_00467C00[i][j + 1][1] * 0.1f;
            dx -= D_00467C00[i][j][0] * 0.4f;
            dy -= D_00467C00[i][j][1] * 0.4f;
            D_00467EC0[i][j][0] += dx;
            D_00467EC0[i][j][1] += dy;
            D_00467940[i][j][0] += (int)(D_00467EC0[i][j][0] * 0.05f);
            D_00467940[i][j][1] += (int)(D_00467EC0[i][j][1] * 0.05f);
        }
    }

    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEXFLUSH, 0xFFFFFFFF);
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_TEX0_1, 0x26802ABC0L);
    func_00109A80(GS_REG_TEX1_1, 0);
    func_00109AD0();

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 10; j++) {
            p0 = p = func_00109990(0x11);
            *p++ = 0x1000000F;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            t0 = 0xC0AE400000008001L;
            *p++ = 0x6C0DC000;
            *(long *)p = t0;
            p += 2;
            *(long *)p = 0x513513513513L;
            p += 2;
            for (k = 0; k < 2; k++) {
                for (l = 0; l < 2; l++) {
                    b = D_00467940[i + k][j + l][0];
                    x = D_00467680[i + k][j + l][0];
                    *p++ = x + fptoui((b - x) * t);
                    y = D_00467680[i + k][j + l][1];
                    *p++ = y + fptoui((D_00467940[i + k][j + l][1] - y) * t);
                    *p++ = 0;
                    *p++ = 0;
                    *p++ = a0;
                    *p++ = a1;
                    *p++ = a2;
                    *p++ = a3;
                    *p++ = (((j + l) << 6) + 0x6C0) << 4;
                    *p++ = (((i + k) << 6) + 0x720) << 4;
                    *p++ = 0;
                    *p++ = 0;
                }
            }
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
    }

    D_00446740 = 0;
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    func_00109E18();
}
