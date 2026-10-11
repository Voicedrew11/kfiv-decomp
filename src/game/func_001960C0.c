#include "common.h"

typedef struct {
    char pad0[0x138];
    int f138;
} Inner;

typedef struct {
    char pad0[0x10];
    u16 flags;
} Ctrl;

typedef struct {
    Inner *inner;
    int pad0[2];
    Ctrl *ctrl;
} Obj;

int func_0018E3D0(void *, int, int, int, int, int, float);

int func_001960C0(Obj *arg0, int arg1, int arg2, int arg3, int arg4, float arg5) {
    Inner *t = arg0->inner;
    int v = t->f138;

    if (v == 0 || !(arg0->ctrl->flags & 0x2000)) {
        return 0;
    }
    return func_0018E3D0(t, v, arg1, arg2, arg3, arg4, arg5);
}
