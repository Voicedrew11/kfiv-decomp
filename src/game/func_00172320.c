#include "kf_entity.h"

extern PlayerData D_00413F80;

int rand(void);
void func_0018B778(float, void *, void *, void *);
f32 func_0018A9F0(void *, void *);
int func_0018EAC0(void *, void *);
int func_00172BB8(Entity *);
void func_001958B8(char *, void (*)(void), int);

void func_00172320(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0) {
        func_0018B778((f32)entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
        entity->target = unkE0->unk30;
        entity->unkA0 = rand() % 60 + 0x3C;
        if (entity->spawn_info->unk40 == 0) {
            if (func_0018A9F0(entity->unk14, entity->model->position) < (f32)entity->params->unk436) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 2;
            } else if (func_0018A9F0(entity->unk14, entity->model->position) > (f32)(s16)entity->params->unk434) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -2;
            }
        } else if (entity->spawn_info->unk40 == 1 || entity->spawn_info->unk40 == 2) {
            if (entity->spawn_info->spawn_y - entity->model->position[1] <
                    (f32)entity->spawn_info->unk42 * (entity->spawn_info->unk43 != 0 ? 100.0f : -100.0f) ||
                ENTITY_FLAG_BITS(entity)->bit44 == 1) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 2;
            } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                       ENTITY_FLAG_BITS(entity)->bit46 == 1) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -2;
            }
        }
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else {
            unkE0->unk79 = 0;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        // match: retail passes two more arguments here and takes the result as 16 bits
        entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
