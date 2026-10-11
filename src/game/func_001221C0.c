#include "common.h"

extern char D_0038CD20[];
extern char D_0038CC50[];
extern void *D_00420D50[];

int func_00103AE0(u8, u8, void *, u8);
int func_00103B38(u8, u8);
void func_0011E3A0(char *);
void func_0011F148(char *);
void func_001BC1C0(int);

void func_001221C0(char *arg0) {
    char *temp_a2;
    u8 temp_a3;
    u8 temp_s2;
    int temp_s0;
    char *entry;
    char *g;

    temp_a2 = arg0 + 0x40;
    temp_a3 = temp_a2[0x1C];
    entry = D_0038CD20 + (arg0[0x10] & 1) * 0x60;
    temp_s2 = entry[1];
    if ((s8) temp_a2[0x1C] > 0) {
        temp_a2[0x1C] = temp_a3 - 1;
    }
    g = D_0038CC50;
    if ((*(u32 *)(g + 0x24) >> 1) & 1) {
        if (D_00420D50[0] == arg0) {
            arg0[0x11] = 5;
            temp_s0 = func_00103AE0(temp_s2, (*(char **)(arg0 + 8))[0x3A], temp_a2, temp_a3);
            func_00103B38(temp_s2, (*(char **)(arg0 + 8))[0x3A]);
            func_0011E3A0(arg0);
            func_0011F148(arg0);
            func_001BC1C0(temp_s0);
            *(int *)(arg0 + 0x10) &= 0xFFFDFFFF;
            *(int *)(g + 0x30) |= 1;
        }
    }
}
