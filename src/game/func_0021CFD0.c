#include "common.h"

#define HW_1000B410 (*(volatile u32 *)0x1000B410)
#define HW_1000B420 (*(volatile u32 *)0x1000B420)
#define HW_1000B430 (*(volatile u32 *)0x1000B430)

typedef struct {
    int w[4];
} Tag;

typedef struct {
    char *buf;
    Tag *tag;
    int n;
    int fC;
    int f10;
    int f14;
    int pad18;
    int f1C;
    int f20;
    int f24;
    int f28;
    char pad2C[0x44 - 0x2C];
    int f44;
    s64 f48;
} S;

void func_0021CDB8(int);
void func_0021CE20(Tag *, int, int, int);

int func_0021CFD0(S *arg0) {
    int i;

    arg0->f44 = 1;
    arg0->fC = 0;
    arg0->f10 = 0;
    arg0->f14 = 0;
    arg0->f48 = 0;
    arg0->f1C = 0;
    arg0->f20 = 0;
    arg0->f24 = 0;
    arg0->f28 = 0;
    for (i = 0; i < arg0->n; i++) {
        func_0021CE20(&arg0->tag[i], (int)(arg0->buf + i * 0x800), 3, 0x80);
    }
    func_0021CE20(&arg0->tag[i], (int)arg0->tag & 0x0FFFFFFF, 2, 0);
    HW_1000B420 = 0;
    HW_1000B410 = (int)arg0->buf & 0x0FFFFFFF;
    HW_1000B430 = (int)arg0->tag & 0x0FFFFFFF;
    func_0021CDB8(5);
    return 1;
}
