#include "kf_entity.h"

float sinf(float);
float cosf(float);
double acos(double);
void Vector_Normalize(void *, void *);
void Vector_Cross(void *, void *, void *);

/* while the entity is pushed (flags 45, 46, 48) from the front: the heading that turns it a degree
   past side on to the push, written to `heading_out`. Returns the side it turns to (1, -1), or 0
   when there is nothing to do. */
int func_0018EAC0(void *entity_arg, void *heading_out) {
    Entity *entity = entity_arg;
    f32 facing[4];
    f32 push[4];
    f32 angle;

    *(f32 *)heading_out = 0.0f;
    if (!(entity->flags & (0xB000L << 33))) {
        return 0;
    }
    angle = entity->model->unkD0[1];
    facing[0] = -sinf(angle);
    facing[1] = 0.0f;
    facing[2] = -cosf(angle);
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
    Vector_Normalize(push, push);
    angle = facing[0] * push[0] + facing[2] * push[2];
    if (angle >= 0.0f) {
        return 0;
    }
    angle = (f32)acos(angle) - 1.5707964f;
    Vector_Cross(facing, push, facing);
    if (facing[1] <= 0.0f) {
        angle = entity->model->unkD0[1] + angle + 0.017453292f;
        *(f32 *)heading_out = angle;
        return 1;
    }
    angle = entity->model->unkD0[1] - angle - 0.017453292f;
    *(f32 *)heading_out = angle;
    return -1;
}
