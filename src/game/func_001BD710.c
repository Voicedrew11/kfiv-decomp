#include "common.h"

typedef struct {
    char pad[0x1C8];
    u64 flags;
} State;

extern State D_00413F80;
extern void (*D_004145C8[])(u16, u16);
extern int D_00447128;

int func_001BD710(u16 arg0, u16 arg1) {
    State *g = &D_00413F80;

    if ((int)(g->flags >> 36) & 1) {
        D_004145C8[D_00447128](arg0, arg1);
    }
    return (int)(g->flags >> 36) & 1;
}
