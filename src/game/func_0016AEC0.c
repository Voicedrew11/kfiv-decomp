#include "common.h"

typedef struct {
    char pad0[0x18];
    int p18;
} Sub;

typedef struct {
    char pad0[0x5C];
    char *f5C;
    char pad60[0xA2 - 0x60];
    u8 fA2;
    char padA3[0xC6 - 0xA3];
    u16 fC6;
    s16 fC8;
    char padCA[0xD7 - 0xCA];
    u8 fD7;
    u64 bits;
    Sub sub;
} Actor;

extern char D_00413F80[];
extern u32 D_00446ED4;
void func_0016B920(void);

s16 func_00190BE0(Actor *);
void func_001958B8(Actor *, int, int);
void func_001958F8(Actor *, void (*)(void), int, int, int);
int func_001992F0(Actor *, int);
void func_00196870(Actor *, int);

void func_0016AEC0(Actor *arg0, int arg1) {
    Sub *s = &arg0->sub;

    if (D_00446ED4 % 10U == arg0->fD7 % 10U) {
        func_00196870(arg0, 1);
    }
    arg0->bits &= ~0x400;
    if (arg0->fC6 == 0) {
        u64 t = arg0->bits | 0x200;
        u32 hi;

        arg0->bits = t;
        hi = (u32) (t >> 63);
        if (hi == 0 && arg0->fA2 == 0 && func_001992F0(arg0, 2) != 0) {
            func_001958F8(arg0, func_0016B920, arg1, 1, 2);
        } else {
            arg0->f5C = D_00413F80;
            arg0->fC8 = func_00190BE0(arg0);
            func_001958B8(arg0, s->p18, arg1);
        }
    }
    arg0->bits &= ~0x3800;
}
