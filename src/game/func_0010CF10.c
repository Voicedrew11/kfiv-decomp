#include "common.h"

typedef struct { char pad[0x78]; u64 w; } Ent;
void func_0010CF10(char *o) {
    int i = 0;
    u16 total = *(u16 *)(o + 0xF0);
    Ent *e = *(Ent **)(o + 0xF8);
    s64 *s = *(s64 **)(o + 0x1A4);
    for (; i < total; i++, e++) {
        int j = 0;
        int n = (int)(e->w >> 53);
        char *p = *(char **)&e->w;
        for (; j < n; j++) {
            char *cur = p;
            u16 u = *(u16 *)p;
            int m;
            int k = 0;
            char *d;
            p += 0x30;
            *(s64 *)p = *s++;
            m = *(int *)(cur + 0x90) & 0x7FFF;
            d = cur + 0xB0;
            p += 0x10;
            *(s64 *)p = *s++;
            p += 0x10;
            *(s64 *)p = *s++;
            p += 0x10;
            *(s64 *)p = *s++;
            for (; k < m; k++) {
                *(s64 *)(d + 0x20) = *s++;
                d += 0x40;
            }
            p = cur + (u + 1) * 16;
        }
    }
}
