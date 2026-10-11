#include "kf_entity.h"

extern float D_00413F80[];

float func_001954A8(float *, float *);
void func_00195898(char *, u8);
void func_001958B8(char *, void (*)(void), int);

void func_00155B58(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 step;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk6A = 0;
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
        break;
    case 1:
        if (entity->params->unk436 < unkE0->unk6A++) {
            entity->model->unkD0[1] = func_001954A8(entity->model->position, (f32 *)D_00413F80);
            entity->action_step++;
        }
        break;
    case 2:
        unkE0->unk6C--;
        if (unkE0->unk6C < 0) {
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit27 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->action_step++;
        } else if (entity->params->unk43C < unkE0->unk6C) {
            step = 1.3f / entity->params->unk43A;
            entity->model->unkE0[0] += step;
            entity->model->unkE0[1] -= step;
            entity->model->unkE0[2] += step;
            entity->unkD6 += 0x80 / entity->params->unk43A;
        } else if (unkE0->unk6C == entity->params->unk43C) {
            entity->model->unkE0[0] = 1.3f;
            entity->model->unkE0[1] = 1.3f;
            entity->model->unkE0[2] = 1.3f;
            entity->unkD6 = 0x80;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00195898((char *)entity, 2);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk68 = entity->params->unk434;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
