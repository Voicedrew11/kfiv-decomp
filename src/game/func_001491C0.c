#include "common.h"

extern float D_00413F84[];

double atan2(double, double);
float func_00194FB8(float);
float func_00195388(float *, float, float);

void func_001491C0(char *arg0) {
    char *ctrl;
    char *s1;
    char *sub;
    float a;

    ctrl = *(char **)(arg0 + 0xC);
    s1 = *(char **)(*(char **)arg0 + 0xF8) + (*(s16 *)(ctrl + 0x434) << 7);
    sub = arg0 + 0xE0;
    if (s1 != 0) {
        a = func_00194FB8(atan2(D_00413F84[0] - *(float *)(*(char **)arg0 + 0xC4), *(float *)(arg0 + 0x64)));
        if (0.87266463f < a) {
            a = 0.87266463f;
        } else if (a < -0.87266463f) {
            a = -0.87266463f;
        }
        func_00195388((float *)(sub + 0x58), a, 5.0f);
        *(float *)(s1 + 0x50) = *(float *)(sub + 0x58);
    }
}
