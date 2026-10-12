#include "common.h"
#include "kf_entity.h"

LinkedPoint *func_0019B260(void);
void func_0019B318(LinkedPoint *, u32);
void func_00216778(void *, void *);

/* Takes a node from the pool, links it into list `kind` and fills it in; returns its id or -1. */
int func_0019A928(f32 duration, void *start, void *end, u8 kind_a, int kind_b, int unk32, u32 kind) {
    LinkedPoint *tween;

    if (kind >= 2) return -1;
    tween = func_0019B260();
    if (tween == NULL) return -1;
    func_0019B318(tween, kind);
    func_00216778(tween, start);
    tween->follow = start;
    func_00216778(&tween->target, end);
    tween->kind_a = kind_a;
    tween->kind_b = kind_b;
    tween->unk32 = unk32;
    tween->target.w = 1.0f;
    tween->duration = duration;
    tween->position.w = 1.0f / duration;
    tween->progress_scale = 1.0f / (duration * duration);
    return tween->handle;
}
