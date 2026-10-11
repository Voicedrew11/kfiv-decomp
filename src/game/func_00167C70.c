#include "kf_entity.h"

int rand(void);
float func_00194FB8(float);
int func_00198730(void *, float, int);
int func_001995A0(void *, float, int);
void func_00190B28(Entity *);

s32 func_00167C70(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 chance;
    s32 i;
    s32 roll;
    s32 sum;
    f32 angle;

    angle = func_00194FB8(entity->model->unkD0[1] + entity->unkE0.unk48);
    chance = func_00198730(entity, angle, 0) ? (entity->unkC0[0] * entity->params->unk240[0].unk00) >> 6 : 0;
    chances[0] = chance;
    total += chance;
    chance = func_00198730(entity, angle, 1) ? (entity->unkC0[1] * entity->params->unk240[1].unk00) >> 6 : 0;
    chances[1] = chance;
    total += chance;
    chance = func_00198730(entity, angle, 2) ? (entity->unkC0[2] * ((EntityParamsUnk240 *)entity->params->unk350)->unk00) >> 6 : 0;
    chances[2] = chance;
    total += chance;
    chance = func_001995A0(entity, angle, 0) ? (entity->unkC0[3] * ENTITY_PARAMS_UNK3D8(entity->params, 0)->unk00) >> 6 : 0;
    chances[3] = chance;
    total += chance;
    chance = func_001995A0(entity, angle, 1) ? (entity->unkC0[4] * ENTITY_PARAMS_UNK3D8(entity->params, 1)->unk00) >> 6 : 0;
    chances[4] = chance;
    total += chance;
    chance = func_001995A0(entity, angle, 2) ? (entity->unkC0[5] * ENTITY_PARAMS_UNK3D8(entity->params, 2)->unk00) >> 6 : 0;
    chances[5] = chance;
    total += chance;
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->bits58 = choice;
        ENTITY_FLAG_BITS(entity)->bit57 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits58 = choice - 3;
        ENTITY_FLAG_BITS(entity)->bit57 = 1;
    }
    func_00190B28(entity);
    return (s32)(entity->flags >> 57) & 1;
}
