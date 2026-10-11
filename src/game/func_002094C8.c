#include "common.h"

extern char D_0038CC50[];
extern int D_00447388;
extern void *D_00447390;
extern void *D_0044739C;
extern int D_004473A0;
extern void *D_004473A4;
extern int D_004473A8;

void func_00108AC8(void *);
void func_0010E8E0(void *, int, int, int, int);

void func_002094C8(void) {
    int temp_v0;
    void *temp_a0;

    if (*(s16 *)(*(char **)((char *)D_0044739C + 0xFC) + 0x82) <= 0 && D_004473A8 <= 0) {
        char *tbl = D_0038CC50;

        D_00447388 = 0;
        D_00447390 = 0;
        ((int *)tbl)[0xC] = 0;
        ((int *)tbl)[0x10] = 0;
        D_0044739C = 0;
        D_004473A0 = 0;
        D_004473A4 = 0;
        return;
    }
    if (*(s16 *)(*(char **)((char *)D_0044739C + 0xFC) + 0x82) > 0) {
        func_00108AC8(D_0044739C);
    }
    if (D_004473A8 > 0) {
        temp_v0 = D_004473A8 - *(int *)((char *)D_004473A4 + 4);
        D_004473A8 = temp_v0;
        if (temp_v0 < 0) {
            D_004473A8 = 0;
        }
        func_0010E8E0(D_0044739C, 0x80, 0x80, 0x80, D_004473A8);
    }
    temp_a0 = *(void **)((char *)D_0044739C + 0xF8);
    *(s64 *)((char *)temp_a0 + 0x68) &= ~0x20000000000L;
}
