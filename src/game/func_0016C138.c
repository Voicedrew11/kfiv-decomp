#include "common.h"

void func_00190A30(char *);
void func_00195898(char *, int);
void func_0019A310(char *, void (*)(void), int);
extern void func_00181378(void);
extern void func_001814B0(void);
extern void func_001816A0(void);

typedef struct {
    char pad0[0xD8];
    u64 flags;
} Actor;

void func_0016C138(char *arg0) {
    Actor *a = (Actor *)arg0;
    u64 f;
    u64 state;

    func_00190A30(arg0);
    a->flags &= ~0x200;
    f = a->flags;
    state = (f >> 36) & 3;
    if (state == 1) {
        if ((int)(f >> 19) & 1) {
            func_0019A310(arg0, func_00181378, 0);
        } else {
            func_0019A310(arg0, func_001814B0, 0);
        }
    } else if (state == 2) {
        func_0019A310(arg0, func_001816A0, 0);
    }
    func_00195898(arg0, 2);
}
