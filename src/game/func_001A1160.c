#include "common.h"

extern char D_002496BF[];
extern int D_003EF560[];
extern int D_003EF678[];
extern int D_004470F4;

int sprintf(char *, const char *, ...);
int FlushCache(int);
int func_001A1360(char *, char *, int, int, int);
int func_001A1438();
int func_001CD6B8(int);
void func_001CD7E0(int);

void func_001A1160(int arg0) {
    char sp[128];
    char sp80[128];
    int temp_s1;
    int temp_v1;
    int temp_a0;

    temp_s1 = arg0 + 0x96;
    sprintf(sp, (char *)D_003EF678[1], temp_s1);
    sprintf(sp80, (char *)D_003EF678[2], temp_s1);
    temp_v1 = ((u32)D_002496BF >> 6) << 6;
    D_003EF560[0] = temp_v1;
    temp_a0 = temp_v1 + 0x20000;
    D_003EF560[1] = temp_a0;
    D_003EF560[2] = 0;
    D_004470F4 = temp_a0 + 0x8000;
    while (func_001A1438() == 0);
    D_003EF678[4] = func_001A1360(sp, sp80, D_003EF678[0], D_003EF678[6], D_003EF678[7]);
    while (func_001A1438() == 0);
    while (func_001CD6B8(0) == 0);
    func_001CD7E0(D_003EF678[4]);
    FlushCache(0);
}
