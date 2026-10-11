#include "common.h"

extern int D_003EF544[];
extern int D_00446858;
extern int D_0044685C;
extern int D_004470EC;
extern int D_004470F4;

void func_00102218(void);
int func_00102230(void);
int func_00102238(char *, int, int);
int func_001025C0(int, int, int);
int func_00102900(int);
int func_001CBD20(void);
int sprintf(char *, char *, ...);

typedef struct {
    int *w00;
    int pad04;
    char *w08;
    int pad0C[6];
    int w24;
} Ctx;

int func_0019EE28(Ctx *arg0, int arg1) {
    char sp[128];
    int temp_v0;
    int temp_v1;
    int *v;

    if (func_00102230() != 0) {
        D_0044685C = 1;
        return -1;
    }
    if (func_001CBD20() == 0) {
        return -1;
    }
    temp_v1 = D_004470F4;
    D_003EF544[0] = temp_v1;
    D_004470F4 = ((u32)(temp_v1 + 0x2003F) >> 6) << 6;
    sprintf(sp, arg0->w08, arg1);
    temp_v0 = func_00102238(sp, 1, 1);
    arg0->w24 = temp_v0;
    D_004470EC = func_00102900(temp_v0);
    func_00102218();
    v = arg0->w00;
    func_001025C0(arg0->w24, v[1], -1);
    D_00446858 += 1;
    return 0;
}
