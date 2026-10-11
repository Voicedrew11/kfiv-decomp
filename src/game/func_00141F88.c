#include "common.h"

typedef struct {
    char pad0[0xC6];
    u16 fC6;
    char padC8[4];
    s8 fCC;
    char padCD[0xA];
    u8 fD7;
    u64 b0 : 41;
    u32 b41 : 1;
    u32 b42 : 11;
    u32 b53 : 1;
    u32 b54 : 10;
} Actor;

extern u32 D_00446ED4;
void func_00142080(Actor *, int);
void func_00182DE0(Actor *, int);
void func_00182FD8(Actor *, int);
void func_0018E9C0(Actor *);
void func_00196870(Actor *, int);
void func_00196CC0(Actor *);

void func_00141F88(Actor *a, int arg1)
{
    if (D_00446ED4 % 10U == a->fD7 % 10U) {
        func_00196870(a, 1);
    }
    if (a->fC6 != 0) {
        return;
}
    func_00196CC0(a);
    func_0018E9C0(a);
    if (a->b53) {
        func_00182DE0(a, arg1);
        return;
    }
    switch (a->fCC) {
    case 0:
        break;
    case 1:
    case 2:
        func_00142080(a, arg1);
        return;
    }
        func_00182FD8(a, arg1);
}
