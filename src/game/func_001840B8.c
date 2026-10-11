#include "common.h"

extern int D_00413F80[];
extern float D_00445898;
void func_00184330(void);
float func_001954A8(int, int *);
float func_001957B8(float, float);
void func_00195898(char *, int);
void func_001958B8(char *, void (*)(void), int);

void func_001840B8(char *arg0, int arg1) {
    char *s;
    float a;
    float g;
    float t;
    float lim;

    s = arg0 + 0xE0;
    t = func_001954A8(*(int *)arg0 + 0xC0, D_00413F80);
    a = func_001957B8(*(float *)(*(char **)arg0 + 0xD4), t);
    g = (float)*(s16 *)(*(char **)(arg0 + 0xC) + 0x438) * D_00445898;
    if (0.0f <= a ? g < a : g < -a) {
        func_001958B8(arg0, func_00184330, arg1);
    } else {
        *(s16 *)(s + 0x68) = 0x1000;
        *(s16 *)(arg0 + 0xC8) = 0x1000;
        func_00195898(arg0, 5);
        func_001958B8(arg0, *(void (**)(void))(s + 0x18), arg1);
    }
}
