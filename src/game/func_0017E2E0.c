#include "kf_entity.h"

extern float D_00413F80[];

int rand(void);
void func_0018B778(float, void *, void *, void *);
int func_0018EAC0(void *, void *);
s32 func_0017EA98(Entity *);
void func_001958B8(char *, void (*)(void), int);

void func_0017E2E0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0) {
        func_0018B778((f32)entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
        entity->unk5C = (s32)unkE0->unk30;
        entity->unkA0 = rand() % 60 + 60;
        if (D_00413F80[1] - entity->model->position[1] < entity->params->unk436) {
            entity->unk5C = (s32)D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        }
        if (D_00413F80[1] - entity->model->position[1] > (s16)entity->params->unk434) { // match: read signed here
            entity->unk5C = (s32)D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->unk5C = (s32)D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->unk5C = (s32)D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else {
            unkE0->unk79 = 0;
        }
    }
    side = func_0018EAC0(entity, direction);
    if (side > 0) {
        entity->unkC8 = 0x201;
    } else if (side < 0) {
        entity->unkC8 = 0x101;
    } else {
        // match: called like func_00190F90, with a speed and a mode the callee ignores
        entity->unkC8 = ((s32 (*)(f32, Entity *, s32))func_0017EA98)(0.0f, entity, 0);
    }
    func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, unknown);
}
