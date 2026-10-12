#include "kf_entity.h"

extern float D_00413F80[];
extern f32 D_00446810[];

int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
float func_00194F80(void *, void *);
void func_0018BA10(f32 *, f32 *, f32, f32);
f32 func_0018A9F0(void *, void *);
float func_00195478(float *, char *);
float func_00194FB8(float);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
void func_00216778(void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_00155B58(Entity *);

void func_00155760(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 *position;
    f32 *destination;
    f32 height;
    f32 yaw;
    f32 step;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk1C = 0.0f;
        unkE0->unk6C = entity->params->unk438;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            unkE0->unk7A = 0;
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (unkE0->unk7A == 0) {
            position = entity->model->position;
            destination = unkE0->unk30;
            func_0018BA10(destination, position, func_00194F80(position, entity->spawn_info), entity->spawn_info->unk4C);
            height = func_0018A9F0(entity->unk14, destination);
            if (height != D_00446810[0]) {
                yaw = func_00195478(D_00413F80, (char *)destination);
                if (func_00194FB8(yaw - D_00413F80[5]) < -1.5707964f || func_00194FB8(yaw - D_00413F80[5]) > 1.5707964f) {
                    unkE0->unk7A = 1;
                    unkE0->unk30[1] += height;
                }
            }
        }
        func_00195ED0(entity, unk3D8, (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8, entity->model->unkFC->unk82);
        if (entity->model->unkFC->unk82 >= 0x23) {
            unkE0->unk6C--;
            ENTITY_FLAG_BITS(entity)->bit20 = 1;
            ENTITY_FLAG_BITS(entity)->bit18 = 1;
            ENTITY_FLAG_BITS(entity)->bit21 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            step = 1.3f / entity->params->unk438;
            entity->model->unkE0[0] -= step;
            entity->model->unkE0[1] += step;
            entity->model->unkE0[2] -= step;
            entity->unkD6 -= 0x80 / entity->params->unk438;
        }
        if (unkE0->unk6C < 0) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk6C = (u16)entity->params->unk43A + (u16)entity->params->unk43C;
            entity->unkD6 = 0;
            if (unkE0->unk7A == 1) {
                func_00216778(entity->model->position, unkE0->unk30);
            }
            func_001958B8((char *)entity, (void (*)(void))func_00155B58, 1);
        }
        break;
    }
}
