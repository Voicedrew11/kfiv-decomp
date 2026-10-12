#include "common.h"

int func_001CBAE0(int, ...);
void func_001CD6E0(int);
int printf(char *);
int sceSifFreeIopHeap(int);
int sceSifSetDma(int *, int);
extern char D_0043F688[];
extern void *D_005BF860[];

typedef struct {
    int w00;
    int w04;
    int w08;
    int w0C;
    int w10;
    int w14;
    int w18;
    int w1C;
} Req;

int func_001CCDF0(int *arg0, Req *arg1) {
    int temp_v0;
    u32 temp_a1;
    u32 temp_v1;

    temp_a1 = arg1->w08;
    temp_v1 = arg1->w18;
    if (temp_v1 < temp_a1) {
        D_005BF860[2] = (void *)temp_v1;
    } else {
        D_005BF860[2] = (void *)temp_a1;
    }
    D_005BF860[0] = (void *)arg0[3];
    D_005BF860[1] = (void *)arg1->w10;
    D_005BF860[3] = 0;
    if (func_001CBAE0(1, 0x80, ((char *)arg1->w00)[0x1A], 0) == 0) {
        printf(D_0043F688);
        return 0;
    }
    temp_v0 = sceSifSetDma((int *)D_005BF860, 1);
    if (temp_v0 == 0) {
        sceSifFreeIopHeap(arg1->w10);
        func_001CD6E0(arg1->w14);
        return -1;
    }
    arg1->w1C = temp_v0;
    return 1;
}
