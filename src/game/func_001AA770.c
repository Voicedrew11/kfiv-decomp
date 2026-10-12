#include "common.h"

void func_001AEC48(char *);
void func_001AED58(char *);
int func_001AF0B0(char *);
void func_001AF298(char *, char *);
void func_00216778(void *, void *);

void func_001AA770(char *arg0) {
    char *s1;
    char *s2;
    char state;

    state = 0;
    s1 = arg0 + 0x10;
    s2 = arg0 + 0x20;
    func_001AEC48(arg0);
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    func_00216778(*(char **)(arg0 + 0x50) + 0xC0, s1);
    func_00216778(*(char **)(arg0 + 0x50) + 0xD0, s2);
    func_00216778(s1, *(char **)(*(char **)(arg0 + 0x60) + 0x50) + 0xC0);
    func_00216778(s2, *(char **)(*(char **)(arg0 + 0x60) + 0x50) + 0xD0);
    func_001AF298(arg0, &state);
    *(u16 *)(arg0 + 0x4C) += 1;
}
