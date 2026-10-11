#include "common.h"

void func_00216D20(void *, void *);
extern char D_0038CC50[];
extern char D_00413F80[];

void func_001BB390(void *arg0, int arg1) {
    float *s;
    float temp_f0;

    if (D_0038CC50[0] != 0) {
        if (!(*(int *)(D_0038CC50 + 0x30) & 1)) {
            s = (float *)D_00413F80;
            if (((u8 *)s)[0x1A8] == 0) {
                if (((int *)s)[0x58 / 4] > 0) {
                    s[0x88] = s[0] - ((float *)arg0)[0];
                    s[0x89] = 0;
                    s[0x8A] = s[2] - ((float *)arg0)[2];
                    func_00216D20(&s[0x88], &s[0x88]);
                    temp_f0 = ((float)(arg1 * arg1) / (float)((int *)s)[0x58 / 4]) * 99.0f;
                    s[0x8C] = temp_f0;
                    if (temp_f0 > 99.0f) {
                        s[0x8C] = 99.0f;
                    }
                }
            }
        }
    }
}
