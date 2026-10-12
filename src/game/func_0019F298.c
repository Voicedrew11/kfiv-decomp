#include "common.h"

extern int D_004468A4;
extern int D_004470D4;
extern int D_004470F4;
extern char D_002496BF[];
extern char D_004FF3C0[];
extern int func_001CBD20(void);
extern int func_00102238(char *, int, int);
extern int func_001025C0(int, int, int);
extern int func_00102790(int, int);
extern void func_001024B0(int);
extern int func_001CD6E0(u32);
extern int sprintf(char *, char *, ...);

typedef struct {
    int *w00;
    char *w04;
    int w08;
    int w0C;
    int w10;
} Ctx;

int func_0019F298(Ctx *arg0, int arg1) {
    int temp_v0;
    int temp_v0_2;

    if (func_001CBD20() != 0) {
        switch (D_004468A4) {
        case 0:
            temp_v0 = ((u32)&D_002496BF >> 6) << 6;
            D_004470F4 = temp_v0;
            *arg0->w00 = temp_v0;
            D_004470F4 = temp_v0 + 0xA0000;
            sprintf(D_004FF3C0, arg0->w04, arg1);
            temp_v0_2 = func_00102238(D_004FF3C0, 1, 1);
            D_004470D4 = temp_v0_2;
            func_001025C0(temp_v0_2, *arg0->w00, -1);
            D_004468A4 = D_004468A4 + 1;
            break;
        case 1:
            if (func_00102790(D_004470D4, 1) == 0) {
                func_001024B0(D_004470D4);
                D_004468A4 = 0;
                func_001CD6E0(arg0->w10);
                return 1;
            }
            break;
        }
    }
    return -1;
}
