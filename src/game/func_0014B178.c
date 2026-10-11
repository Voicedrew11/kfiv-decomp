#include "common.h"

float func_00194FB8(float);
extern float D_004452DC;
extern float D_004452E0;

typedef struct {
    char pad0[0x50];
    float f50;
    char pad54[0x14];
    s16 h68;
    char pad6A[0x11];
    s8 c7b;
    s8 c7c;
} Sub;

typedef struct {
    char pad0[0x50];
    float f50;
} Ctl;

void func_0014B178(char **arg0) {
    Sub *s = (Sub *)((char *)arg0 + 0xE0);
    Ctl *c = (Ctl *)(*(char **)(*arg0 + 0xF8) + 0x180);
    float w;

    if (c != 0) {
        if (s->h68 >= s->c7c) {
            s->f50 += (float)s->c7b / (float)s->c7c * D_004452DC;
            if (s->c7b * D_004452DC < s->f50) {
                s->f50 = s->c7b * D_004452DC;
            }
        } else {
            s->f50 -= (float)s->c7b / (float)s->c7c * D_004452E0;
            if (s->f50 < 0.0f) {
                s->f50 = 0.0f;
            }
        }
        w = s->f50;
        c->f50 = w;
        c->f50 = func_00194FB8(w);
        s->h68--;
    }
}
