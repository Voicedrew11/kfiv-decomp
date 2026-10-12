#include "common.h"

extern int func_001CBD20(void);
extern int func_001CBAC0(void);
extern int func_001CD020(int, int, int, int, int, int, int);
extern void func_001CD6E0(int);
int func_001CDF90(int, int, int, u32);
extern void func_001CE408(int);
extern void func_0019C7D0(int);
extern u8 D_00349681[];
extern int D_00446858;
extern int D_0044685C;
extern int D_00446860;

typedef struct {
    int *ctl;
    int pad04[3];
    int f10;
    int f14;
    int f18;
    int f1C;
} Job;

int func_0019EC90(Job *arg0) {
    int *v;
    int v2;
    int t;
    int r;

    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CBAC0() != 0) {
        return -1;
    }
    t = arg0->f10;
    if (t < 0) {
        v = arg0->ctl;
        r = func_001CD020(arg0->f18, v[1], v[4], v[0], v[3], 0, arg0->f1C);
        arg0->f10 = r;
        if (r >= 0) {
            func_001CD6E0(r);
        }
        return -1;
    } else {
        if (arg0->f14 < 0) {
            v = arg0->ctl;
            r = func_001CDF90(arg0->f18, t, v[2], v[5]);
            arg0->f14 = r;
            if (r >= 0) {
                func_001CD6E0(arg0->f10);
                func_001CE408(arg0->f14);
            }
            return -1;
        }
        D_0044685C = 1;
        D_00446860 -= 3;
        if (D_00446860 < 0) {
            D_00446860 = 0;
        }
        v2 = D_00446860;
        func_0019C7D0((v2 < D_00349681[0]) ? v2 : D_00349681[0]);
        if (D_00446860 == 0) {
            D_00446858 += 1;
            return 0;
        }
        return -1;
    }
}
