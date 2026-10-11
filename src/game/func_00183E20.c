#include "common.h"

typedef struct {
    char b0;
    char b1;
    char b2;
    char pad[0x40 - 3];
} Entry;

extern Entry D_0034B6D0[];

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00184778(void);

typedef struct {
    char pad0[0xC];
    char *ctl;
    char pad1[0x4C];
    int f5C;
    char pad2[0xD8 - 0x60];
    u64 flags;
} Actor;

void func_00183E20(char *arg0) {
    Actor *a = (Actor *)arg0;

    func_00190A30(arg0);
    a->flags &= ~0x400;
    a->f5C = 0;
    a->flags &= ~0x200;
    func_0019A310(arg0, func_00184778, 0);
    D_0034B6D0[*(s16 *)(a->ctl + 0x43A)].b2 = 1;
}
