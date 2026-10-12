#include "common.h"

void func_001AEC48(char *);
void func_001AED58(char *);
int func_001AF0B0(char *);
void func_001AF298(char *, char *);
void func_001AF538(float, void *, void *);
void func_00216778(void *, void *);
void sceVu0AddVector(char *, char *, char *);

void func_001ABC40(char *arg0) {
    char *s1;
    char *t;
    u16 n;

    func_001AEC48(arg0);
    s1 = arg0 + 0x10;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    t = *(char **)(arg0 + 0x54);
    n = *(u16 *)(arg0 + 0x4C);
    if (*(u16 *)(t + 0xEC) < n) {
        char *p = *(char **)(arg0 + 0x50) + 0xC0;
        sceVu0AddVector(p, p, s1);
    } else if (n < *(u16 *)(t + 0xEC)) {
        func_00216778(*(char **)(arg0 + 0x50) + 0xC0, *(void **)(arg0 + 0x64));
    } else {
        func_001AF538(*(float *)(t + 0xD8), s1, *(void **)(arg0 + 0x68));
    }
    func_001AF298(arg0, s1 + 0x1C);
    *(u16 *)(arg0 + 0x4C) += 1;
}
