#include "common.h"

typedef struct E {
    char pad[0x78];
    u64 addr : 32;
    u64 pad2 : 21;
    u64 count : 11;
} E;

typedef struct O {
    char pad[0xF0];
    u16 n;
    char pad2[6];
    E *e;
} O;

void func_0010CAE8(O *arg0, s64 arg1, s64 arg2, s64 arg3, s64 arg4) {
    E *e;
    char *a;
    char *base;
    int i;
    int j;
    int c;
    int n;
    u16 cnt;

    n = arg0->n;
    e = arg0->e;
    for (i = 0; i < n; i++, e++) {
        c = e->count;
        a = (char *)(int)e->addr;
        for (j = 0; j < c; j++) {
            base = a;
            cnt = *(u16 *)base;
            a += 0x30;
            if (arg1 != -1) {
                *(s64 *)a = arg1;
            }
            a += 0x10;
            if (arg2 != -1) {
                *(s64 *)a = arg2;
            }
            a += 0x10;
            if (arg3 != -1) {
                *(s64 *)a = arg3;
            }
            a += 0x10;
            if (arg4 != -1) {
                *(s64 *)a = arg4;
            }
            a = base + (cnt + 1) * 0x10;
        }
    }
}
