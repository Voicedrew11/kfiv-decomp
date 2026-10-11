#include "common.h"

extern int D_00446770;
void *func_0011E1F0(int, int);

void func_00213010(char *arg0) {
    char *temp_a0;
    char *temp_v0;

    temp_v0 = func_0011E1F0(D_00446770, *(int *)(arg0 + 0x8));
    if (temp_v0 != 0) {
        if (*(int *)temp_v0 != 0) {
            if (**(u8 **)(temp_v0 + 4) == 9) {
                temp_a0 = *(char **)(temp_v0 + 8);
                *(u16 *)(temp_a0 + 0x44) = *(u16 *)(arg0 + 0x10);
                *(u16 *)(temp_a0 + 0x46) = *(u16 *)(arg0 + 0x18);
                *(u16 *)(temp_a0 + 0x48) = *(u16 *)(arg0 + 0x20);
                *(u16 *)(temp_a0 + 0x4A) = *(u16 *)(arg0 + 0x28);
            }
        }
    }
    *(int *)arg0 = 0;
}
