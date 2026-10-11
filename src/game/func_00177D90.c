#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
float func_001957B8(float, float);
void func_00216778(void *, void *);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

extern float D_00413F80[];

void func_00177D90(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->bits58);
    f32 target[4];
    f32 angle;
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->unk10 & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->state++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        break;
    case 2:
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if (entity->model->unkFC->unk82 >= 0x2A) {
            entity->unkA0 = 0x5A;
            entity->state++;
        }
        break;
    case 3:
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        func_001957B8(entity->model->unkD0[1], entity->unk68);
        angle = func_001957B8(unkE0->unk4C, entity->unk68);
        if (entity->unkA0 == 0 || entity->distance_to_player < 200.0f || ABS(angle) <= 0.010471975f) {
            entity->state++;
        }
        break;
    case 4:
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(target, D_00413F80);
        target[1] -= 170.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.17453294f, 0.17453294f);
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
    unkE0->unk4C = entity->unk68;
}
