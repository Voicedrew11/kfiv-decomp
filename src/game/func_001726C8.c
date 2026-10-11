#include "common.h"

typedef struct {
    char pad0[0xC];
    u8 *ctl;
    char pad1[0xD8 - 0x10];
    u64 bits;
} Actor;

int func_0019A448(void *, void *, int, int, int, int, int);

void func_001726C8(Actor *a) {
    a->bits &= ~0x3800ULL;
    func_0019A448(a, (char *)a + 0x160, 0, 3, 0, a->ctl[0xC4], 0);
    func_0019A448(a, (char *)a + 0x168, 1, 3, 0, a->ctl[0xC4], 0);
}
