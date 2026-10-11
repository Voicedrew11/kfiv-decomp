#include "common.h"

int SignalSema(int);
int WaitSema(int);

typedef struct {
    char pad0[0x14];
    int f14;
    char pad1[0x40 - 0x18];
    int sema;
    char pad2[0x48 - 0x44];
    s64 f48;
} Obj;

void func_0021D1B8(void *arg0, int arg1) {
    Obj *o = arg0;

    WaitSema(o->sema);
    o->f14 = o->f14 + arg1;
    o->f48 = arg1 + o->f48;
    SignalSema(o->sema);
}
