#include "common.h"

typedef struct {
    char pad0[0x10];
    int i10;
    char pad1[0x38];
    u16 u4C;
    char pad2[6];
    void *p54;
} Actor;

typedef struct {
    char pad0[0xEC];
    u16 uEC;
} Target;

void func_001AF298(Actor *, char *);

void func_001A41F0(Actor *arg0) {
    char state;

    arg0->i10 = arg0->i10 - 1;
    if (((Target *)arg0->p54)->uEC == arg0->u4C) {
        func_001AF298(arg0, &state);
    }
    arg0->u4C = arg0->u4C + 1;
}
