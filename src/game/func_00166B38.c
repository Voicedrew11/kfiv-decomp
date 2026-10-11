#include "common.h"

void func_00190A30(char *);
void func_00195898(char *, int);
void func_0019A310(char *, void (*)(void), int);
void func_00181378(void);
void func_001814B0(void);
void func_001816A0(void);

typedef struct {
    char pad0[0x5C];
    int f5C;
    char pad1[0xD8 - 0x60];
    u64 flags;
} Actor;

void func_00166B38(char *arg0) {
    Actor *a = (Actor *)arg0;

    func_00190A30(arg0);
    a->flags &= ~0x400;
    a->f5C = 0;
    a->flags &= ~0x200;
    a->flags &= ~0x10000000;
    if (((a->flags >> 36) & 3) == 1) {
        if ((int)((s64)(a->flags << 13) >> 32) & 1) {
            func_0019A310(arg0, func_00181378, 0);
        } else {
            func_0019A310(arg0, func_001814B0, 0);
        }
    } else if (((a->flags >> 36) & 3) == 2) {
        func_0019A310(arg0, func_001816A0, 0);
    }
    func_00195898(arg0, 2);
}
