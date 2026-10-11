#include "common.h"

float sqrtf(float);
extern float D_00413F80[];

typedef struct {
    char pad[0xC0];
    float x;
    char pad2[4];
    float y;
} Pos;

typedef struct {
    char pad[0x46];
    s16 s46;
} Dev;

typedef struct {
    char pad[0x1C];
    int count;
} Sub;

typedef struct {
    Pos *p00;
    char pad04[4];
    Dev *d08;
    char pad0C[4];
    int flags10;
    float f14;
    char pad18[0x28];
    Sub sub40;
} Actor;

void func_0011F708(Actor *a) {
    Pos *p;
    Sub *s;
    float *g;
    float dx;
    float dy;
    float dist;

    s = &a->sub40;
    if (s->count > 0) {
        s->count = s->count - 1;
    }
    if (a->d08->s46 == 0) {
        p = a->p00;
        g = D_00413F80;
        dx = p->x - g[0];
        dy = p->y - g[2];
        dist = sqrtf(dx * dx + dy * dy);
        if (!(dist < a->f14 + 100.0f)) {
            a->flags10 &= 0xFFFDFFFF;
            ((char *)a)[0x11] = 4;
        }
    }
}
