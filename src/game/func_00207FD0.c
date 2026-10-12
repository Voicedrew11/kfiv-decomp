#include "common.h"

extern u8 spell_levels[6][5];
extern u16 spell_experience[6][5];

int atoi(const char *);
char *strchr(const char *, int);

void func_00207FD0(int arg0, char *arg1) {
    int idx;
    char *p;

    if ((u8)(arg1[-2] - 0x31) < 5) {
        idx = arg1[-2] - 0x31;
        atoi(arg1);
        spell_levels[arg0][idx] = atoi(arg1);
        p = strchr(arg1, 0x2C);
        if (p != 0) {
            spell_experience[arg0][idx] = atoi(p + 1);
        }
    }
}
