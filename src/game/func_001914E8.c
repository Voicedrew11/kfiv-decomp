#include "common.h"

typedef struct {
    u8 n;
    char pad0[0xB];
    u16 *p;
} Ent;

typedef struct {
    char pad0[0x6C];
    u8 tag;
    char pad1[0xB];
    u64 lo : 39;
    u64 f : 7;
} Rec;

typedef struct {
    char pad0[0xF0];
    u16 count;
    char pad1[6];
    Rec *recs;
    Ent **ents;
} Ctx;

int func_001914E8(Ctx *arg0, int arg1, int arg2, int arg3) {
    Ent **tbl = &arg0->ents[arg2];
    Ent *e;
    int i;

    for (e = *tbl, i = 0; i < arg0->count; i++, e++) {
        Rec *r = &arg0->recs[i];
        int v;
        int n;
        u16 *p;

        if (r->tag == arg1 && e->p != 0 && e->n != 0) {
            p = e->p;
            n = e->n;
            for (v = 0; v < n; v++) {
                if (arg3 < *p)
                    break;
                p += 9;
            }
            if (v == n) {
                r->f = 0;
            } else if (v != 0) {
                r->f = v - 1;
            }
        }
    }
    return 1;
}
