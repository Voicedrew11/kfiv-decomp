#include "kf_entity.h"

float sinf(float);
float cosf(float);
void func_00216D20(void *, void *);

/* whether the push the entity is under (flags 45, 46, 48) points away from `heading` */
s32 func_0018EC78(Entity *entity, f32 heading) {
    f32 facing[4];
    f32 push[4];

    // match: the early return and the second `return FALSE` below leave a label in front of the body
    // until jump2 merges the two, which keeps `heading` copied back into $f12 for sinf
    if (!(entity->flags & (0xB000L << 33))) {
        return FALSE;
    }
    facing[0] = -sinf(heading);
    facing[1] = 0.0f;
    facing[2] = -cosf(heading);
    facing[3] = 1.0f;
    push[0] = push[1] = push[2] = push[3] = 0.0f;
    if (entity->flags & (0xC000L << 31)) {
        push[0] += entity->unkA4[0];
        push[2] += entity->unkA4[2];
    }
    if ((s32)(entity->flags >> 48) & 1) {
        push[0] += entity->unkB0[0];
        push[2] += entity->unkB0[2];
    }
    func_00216D20(push, push);
    if (facing[0] * push[0] + facing[2] * push[2] >= 0.0f) {
        return FALSE;
    }
    return TRUE;
}
