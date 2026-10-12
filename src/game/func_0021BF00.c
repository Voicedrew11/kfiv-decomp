#include "common.h"

int func_0021BFD8(u8 *, int, u8 *, int, u8 *, int, u8 *, int);
void func_0021C6F0(int *, int *, int *, int *, int *);
void func_0021C790(int *, int);
extern int D_004379F0[];

int func_0021BF00(int arg0, int *arg1, u32 arg2) {
    int sp0, sp4, sp8, spC;
    u32 a;
    int b;
    int rem;
    int len;
    u32 end;
    int v;
    int ret;

    v = *(int *)(arg2 + 0x50008);
    rem = arg1[3] - 4;
    end = arg2 + v;
    a = arg1[2] + 4;
    if (a >= end) {
        a -= v;
    }
    len = end - a;
    len = (rem < len) ? rem : len;
    b = rem - len;
    func_0021C6F0(D_004379F0, &sp0, &sp4, &sp8, &spC);
    ret = func_0021BFD8(sp0, sp4, sp8, spC, a, len, arg2, b);
    func_0021C790(D_004379F0, ret);
    return ret > 0;
}
