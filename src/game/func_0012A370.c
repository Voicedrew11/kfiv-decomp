#include "kf_entity.h"

float func_001957B8(float, float);
void func_001958B8(char *, void (*)(void), int);
void func_0012A778(Entity *);
void func_0012A8E0(Entity *);

void func_0012A370(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (ABS(angle) > 0.43633232f) {
        if (angle >= 0.0f) {
            func_001958B8((char *)entity, (void (*)(void))func_0012A8E0, unknown);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_0012A778, unknown);
        }
    } else {
        func_001958B8((char *)entity, unkE0->unk18, unknown);
    }
}
