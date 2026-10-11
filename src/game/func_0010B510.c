#include "common.h"

typedef union U { s64 l; char c; } U;
typedef struct E {
    int off;
    int pad[3];
} E;
typedef struct T {
    int pad0;
    u32 cnt;
    int pad8[2];
    E ent[1];
} T;
void func_0010B510(T *arg0, int arg1, int arg2) {
    E *e;
    u32 i;
    e = arg0->ent;
    i = 0;
    if (arg0->cnt != 0) do {
        char *s = (char *)arg0 + e[i].off;
        u64 t0;
        s += 0x40;
        if (arg1 != 0) {
            ((U *)s)->l = (((U *)s)->l & ((((s64)-0x4000 << 16) | 0xFFFF) << 16 | 0xFFFF)) | ((s64)(arg1 & 0x3FFF) << 32);
        }
        t0 = ((U *)(s + 0x40))->l;
        s += 0x50;
        if ((int)((t0 >> 15) & 1) == 0) {
            s += (int)(t0 & 0x7FFF) * 16;
            s += 0x20;
            if (arg2 != 0) {
                ((U *)s)->l = (((U *)s)->l & ((((s64)-0x4000 << 16) | 0xFFFF) << 16 | 0xFFFF)) | ((s64)(arg2 & 0x3FFF) << 32);
            }
        }
        i++;
    } while (i < arg0->cnt);
}
