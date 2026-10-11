#include "common.h"

void func_00190A30(char *);
void func_00195898(char *, int);
void func_0019A310(char *, void (*)(void), int);
void func_001409D0(char *);
void func_00181378(void);
void func_001814B0(void);
void func_001816A0(void);

typedef struct {
    u64 z0 : 32;
    u64 b32 : 1;
    u64 b33 : 1;
    u64 z34 : 30;
} Flags;

typedef struct {
    char pad0[0x5C];
    int f5C;
    char pad60[0xD8 - 0x60];
    union { Flags b; u64 l; } flags;
} Actor;

typedef struct {
    char pad0[0x7A];
    s8 c7a;
} Sub;

void func_00140650(char *arg0) {
    Actor *a = (Actor *)arg0;
    Sub *s = (Sub *)(arg0 + 0xE0);
    u64 f;
    u64 state;

    func_00190A30(arg0);
    a->f5C = 0;
    f = a->flags.l & ~0x200;
    a->flags.l = f;
    state = (f >> 36) & 3;
    if (state == 1) {
        if ((int)(f >> 19) & 1) {
            func_0019A310(arg0, func_00181378, 0);
        } else {
            func_0019A310(arg0, func_001816A0, 0);
        }
    } else if (state == 2) {
        func_0019A310(arg0, func_001816A0, 0);
    }
    func_00195898(arg0, 2);
    s->c7a = 1;
    a->flags.b.b32 = 1;
    a->flags.b.b33 = 1;
    func_001409D0(arg0);
}
