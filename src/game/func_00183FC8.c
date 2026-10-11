#include "common.h"

extern int D_00413F80[];
extern float D_00445894;
float func_001954A8(int, int *);
float func_001957B8(float, float);
void func_00195898(char *, int);
void func_001958B8(char *, void (*)(void), int);

void func_00183FC8(char *arg0, int arg1) {
    char *s;
    float a;
    float g;
    float t;

    s = arg0 + 0xE0;
    t = func_001954A8(*(int *)arg0 + 0xC0, D_00413F80);
    a = func_001957B8(*(float *)(*(char **)arg0 + 0xD4), t);
    g = (float)*(s16 *)(*(char **)(arg0 + 0xC) + 0x438) * D_00445894;
    if (0.0f <= a ? g < a : g < -a) {
        if (0.0f <= a) {
            *(u16 *)(s + 0x68) = 0x4000;
        } else {
            *(u16 *)(s + 0x68) = 0x2000;
        }
    } else {
        *(u16 *)(s + 0x68) = 0x1000;
    }
    *(u16 *)(arg0 + 0xC8) = *(u16 *)(s + 0x68);
    func_00195898(arg0, 5);
    func_001958B8(arg0, *(void (**)(void))(s + 0x18), arg1);
}
