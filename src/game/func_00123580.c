#include "kf_ent.h"

extern float D_00413F80[];

float func_00124DE0(void *);
void func_001243E8(char);
void func_00123AB8(void *);
void func_00124D48(void *);
void func_00124BC8(void *);
void func_001A32A8(void *);

/* Free entities that are out of range of the player. */
void func_00123580(void) {
    Ent *s = D_004467E0;
    while (s != 0) {
        EntInfo *v;
        float f;
        float d;
        int hit;
        Ent *cur;

        f = func_00124DE0(s);
        s->dist = f;
        v = s->info;
        d = D_00413F80[1] - v->y;
        if (!(0.0f <= d)) {
            d = -d;
        }
        hit = 0;
        if ((float)v->range + 300.0f < f || (v->yrange != 0 && (float)v->yrange + 150.0f < d)) {
            hit = 1;
        }
        cur = s;
        if (hit != 0) {
            s = s->next;
            func_001243E8(cur->unkD3);
            func_00123AB8(cur);
            func_00124D48(cur);
            func_00124BC8(cur);
            func_001A32A8(cur);
        } else {
            s = s->next;
        }
    }
}
