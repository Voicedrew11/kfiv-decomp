#include "common.h"

int func_0019A928(f32 duration, void *start, void *end, u8 kind_a, int kind_b, int unk32, u32 kind);

/* Starts a point pool entry with its unk32 slot fixed at -1; returns the callee's id. */
int func_0019A900(f32 duration, void *start, void *end, int kind_a, int kind_b, int kind) {
    return func_0019A928(duration, start, end, kind_a, kind_b, -1, kind);
}
