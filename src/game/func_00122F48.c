#include "common.h"

typedef struct {
    int phase;
    int period;
} Wave;

typedef struct {
    void *ptr;
    char pad[0x3C];
    Wave w;
} Obj;

int func_00122F48(Obj *arg0) {
    Wave *w;
    int phase;
    int period;
    float f;
    int v;

    w = &arg0->w;
    phase = (w->phase + 1) % w->period;
    arg0->w.phase = phase;
    period = w->period;
    if (phase < period / 2) {
        f = (float)phase / (float)period * 0.5f;
    } else {
        f = (float)(period - phase) / (float)period * 0.5f;
    }
    v = (int)(f * 128.0f);
    *(s16 *)((char *)arg0->ptr + 0x146) = v;
    return v;
}
