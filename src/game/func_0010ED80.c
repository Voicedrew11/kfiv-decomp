#include "common.h"

typedef struct {
    u64 a : 14;
    u64 pad14 : 23;
    u64 b : 14;
    u64 pad51 : 10;
    u64 c : 3;
} Tag;

typedef struct { char pad[0x78]; u64 w; } Ent;

void func_0010ED80(char *o, int arg1, int arg2) {
    int i = 0;
    int first = arg2 != 0;
    u16 total = *(u16 *)(o + 0xF0);
    Ent *e = *(Ent **)(o + 0xF8);
    for (; i < total; i++, e++) {
        int j = 0;
        int n = (int)(e->w >> 53);
        char *p = *(char **)&e->w;
        for (; j < n; j++) {
            char *cur = p;
            u16 u = *(u16 *)cur;
            p = cur + 0x40;
            if (arg1 != 0) {
                ((Tag *)p)->a = arg1;
            }
            if (arg2 != 0) {
                ((Tag *)p)->b = arg2;
            }
            if (first) {
                first = 0;
                ((Tag *)p)->c = 2;
            }
            p = cur + (u + 1) * 16;
        }
    }
}
