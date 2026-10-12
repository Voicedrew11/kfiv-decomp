#include "kf_entity.h"

extern float D_00413F80[];

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_001992F0(Entity *, s32);
float func_001957B8(float, float);
s16 func_00190BE0(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_001958F8(Entity *, void (*)(void), int, int, int);
void func_00181C60(Entity *);
void func_00130EF8(Entity *);

void func_001311C8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA2 == 0 && func_001992F0(entity, 1)) {
        func_001958F8(entity, (void (*)(void))func_00130EF8, unknown, 1, 0);
        return;
    }
    if (entity->unkA0 == 0) {
        angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
        if (ABS(angle) > 0.5235988f) {
            entity->target = D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
        } else {
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = -1;
        }
    }
    if (unkE0->unk78 == 0) {
        entity->unkC8 = func_00190BE0(entity);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
