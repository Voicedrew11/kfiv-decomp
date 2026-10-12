#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_0018B778(float, void *, void *, void *);
float func_00194F80(void *, void *);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00181C60(Entity *);
void func_00182240(Entity *);
void func_001273D8(Entity *);

/* chase: attack when in reach, else go for the player or a point near the spawn point for a while */
void func_00126E58(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
                return;
            }
            func_001958B8((char *)entity, (void (*)(void))func_001273D8, unknown);
            return;
        case 1:
            if (rand() % 100 < entity->params->unk430) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 120 + 480;
                unkE0->unk78 = 0;
            } else {
                func_0018B778(entity->spawn_info->unk4C, entity->unkE0.unk30, entity->spawn_info, entity->model->position);
                entity->target = entity->unkE0.unk30;
                entity->unkA0 = rand() % 120 + 480;
                unkE0->unk78 = 1;
            }
            break;
        case 2:
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        if (entity->target != NULL) {
            if (func_00194F80(entity->model->position, entity->target) > entity->radius + 100.0f + 30.0f) {
                entity->unkC8 = 1;
            }
        }
        break;
    case 1:
        if (entity->target != NULL) {
            if (func_00194F80(entity->model->position, entity->target) > 50.0f) {
                entity->unkC8 = 1;
            }
        }
        break;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
