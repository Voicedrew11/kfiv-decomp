#include "common.h"

extern char D_003F1750[];
extern char D_003F3550[];
extern int D_004468CC;
extern int D_004468D0;

void func_00216C48(char *, int, char *);

void func_001A1B48(int arg0) {
    char *p;
    char *q;
    char *temp;
    s64 m1;
    s64 m2;
    int i;
    int n;

    m1 = 0x8000L << 17;
    p = D_003F1750;
    q = D_003F3550;
    n = D_004468CC;
    for (i = 0; i < 0x60; i++, p += 0x50) {
        if (*(s64 *)(p + 0x20) & m1) {
            func_00216C48(p, arg0, p);
            n--;
            if (n <= 0) {
                break;
            }
        }
    }

    m2 = 0x8000L << 41;
    n = D_004468D0;
    for (i = 0; i < 0x180; i++, q += 0xA0) {
        if (*(s64 *)(q + 0x40) & m2) {
            n--;
            temp = *(char **)(q + 0x50) + 0xC0;
            func_00216C48(temp, arg0, temp);
            func_00216C48(q + 0x70, arg0, q + 0x70);
            if (n <= 0) {
                break;
            }
        }
    }
}
