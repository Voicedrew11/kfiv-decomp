#include "common.h"

typedef struct { char pad[0x78]; u64 w; } Ent;
typedef struct {
    char p0[0xF0];
    u16 cnt;
    char p1[6];
    Ent *ents;
    char p2[0xA8];
    s64 *out;
} Obj;

s64 *func_0010CE08(Obj *o, s64 *a) {
    s64 *s;
    int i;
    u16 total;
    Ent *e;
    int m;

    o->out = a;
    e = o->ents;
    s = a;
    total = o->cnt;
    for (i = 0; i < total; i++, e++) {
        int j = 0;
        int n = (int)(e->w >> 53);
        s64 *p = *(s64 **)&e->w;
        for (; j < n; j++) {
            s64 *cur = p;
            u16 u = *cur;
            int k = 0;
            s64 *d;
            p += 6;
            *s++ = *p;
            m = *(int *)(cur + 0x12) & 0x7FFF;
            d = cur + 0x16;
            p += 2;
            *s++ = *p;
            p += 2;
            *s++ = *p;
            p += 2;
            *s++ = *p;
            for (; k < m; k++) {
                *s++ = d[4];
                d += 8;
            }
            p = cur + (u + 1) * 2;
        }
    }
    return s;
}
