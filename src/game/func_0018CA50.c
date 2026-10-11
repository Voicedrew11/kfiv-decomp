#include "common.h"

int func_0018B3C0(float, int *, int, int);
extern int D_00413F80[];
extern void *D_004467E0;

typedef struct E {
    int p0;
    char pad4[0x18];
    struct E *next;
    char pad20[0x18];
    float f38;
    char pad3C[0x9C];
    s64 flags;
} E;

int func_0018CA50(int *arg0) {
    E *s;
    s64 temp;

    s = D_004467E0;
    while (s != 0) {
        temp = s->flags;
        if (((int)(temp >> 1) & 1) != 0 && ((int)(temp >> 2) & 3) < 2 && s != (E *)arg0) {
            if (func_0018B3C0(s->f38, D_00413F80, *arg0 + 0xC0, s->p0 + 0xC0) != 0) {
                return 1;
            }
        }
        s = s->next;
    }
    return 0;
}
