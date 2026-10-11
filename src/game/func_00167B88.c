#include "common.h"

int func_00192C98(void *, int, int, u32, int);

typedef struct {
    char pad0[0xC];
    struct {
        char pad0[0xC4];
        u8 c4;
    } *c;
} Actor;

void func_00167B88(Actor *a) {
    func_00192C98(a, 0, 0, a->c->c4, 0);
}
