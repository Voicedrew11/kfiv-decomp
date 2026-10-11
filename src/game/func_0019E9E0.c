#include "common.h"

extern char D_00500158[];
int func_001CE8B0(u32);

typedef struct {
    char pad0[0x3A];
    char a3a;
    char a3b;
    char pad1[0x40 - 0x3C];
} Ent;

extern Ent D_003EF710[];

int func_0019E9E0(char arg0)
{
    int i;

    for (i = 0; i < 0x80; i++) {
        if (D_003EF710[i].a3a == arg0) {
            break;
        }
    }
    if (i >= 0x80) {
        return -1;
    }
    if ((D_003EF710 + i)->a3b > 0) {
        func_001CE8B0((D_003EF710 + i)->a3b);
        (D_003EF710 + i)->a3a = -1;
        D_00500158[(D_003EF710 + i)->a3b - 0x54] = -1;
    } else {
        (D_003EF710 + i)->a3a = -1;
    }
}
