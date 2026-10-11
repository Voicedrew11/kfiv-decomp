#include "common.h"

int func_00192C98(void *, int, int, u32, int);

typedef struct {
    char pad0[0xD8];
    s64 flags;
} Obj;

void func_001389C8(Obj *arg0, int arg1) {
    s64 f;

    func_00192C98(arg0, 0x11, 0, 0, arg1);
    f = arg0->flags & ~0x3800;
    f |= 0x400000;
    f |= 0x800000;
    f |= 0x01000000;
    f |= 0x40000;
    f |= 0x200000;
    f |= 0x10000000;
    f |= 0x10000;
    arg0->flags = f;
}
