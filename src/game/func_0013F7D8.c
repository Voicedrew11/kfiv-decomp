#include "common.h"

extern char D_00413F80[];

double atan2(double, double);
float func_00194FB8(float);
void func_00195388(void *, float, float);
void func_00216778(void *, void *);

void func_0013F7D8(char *arg0) {
    char *ctrl;
    char *sub;
    char *s3;
    s16 a0;
    float buf[2];
    float f;

    ctrl = *(char **)arg0;
    s3 = *(char **)(ctrl + 0xF8) + 0x780;
    sub = arg0 + 0xE0;
    if (s3 != 0) {
        a0 = *(s16 *)(*(char **)(ctrl + 0xFC) + 0x82);
        if (a0 < 0x2B) {
            func_00216778(buf, D_00413F80);
            buf[1] = buf[1] + 400.0f;
            f = func_00194FB8(atan2(buf[1] - *(float *)(ctrl + 0xC4), *(float *)(arg0 + 0x64)));
            func_00195388(arg0 + 0x12C, f, 10.0f);
        } else if (a0 >= 0x36) {
            *(float *)(sub + 0x4C) = *(float *)(sub + 0x4C) * ((float)(0x43 - a0) / 14.0f);
        }
        *(float *)(s3 + 0x50) = *(float *)(s3 + 0x50) + *(float *)(sub + 0x4C);
    }
}
