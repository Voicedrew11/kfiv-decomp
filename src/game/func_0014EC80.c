#include "common.h"

typedef struct {
    char pad0[0xC];
    u8 *ctl;
    char pad1[0xFC - 0x10];
    int fc;
} Actor;

int func_0019A448(void *, void *, int, int, int, int, int);

void func_0014EC80(Actor *arg0) {
    arg0->fc = 0;
    func_0019A448(arg0, (char *)arg0 + 0x160, 0, 1, 0, arg0->ctl[0xC4], 0);
    func_0019A448(arg0, (char *)arg0 + 0x168, 1, 1, 0, arg0->ctl[0xC4], 0);
}
