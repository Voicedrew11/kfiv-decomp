#include "kf_entity.h"

extern float D_00413F80[];

int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
float func_001957B8(float, float);
float func_00195478(float *, char *);
void func_001958B8(char *, void (*)(void), int);

void func_00166800(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 landed;

    entity->unkC6 |= 8;
    switch (entity->state) {
    case 0:
        if (func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position)) > 0.0f) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
        }
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->state++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            unkE0->unk48[0] = entity->params->unk43A * 0.8f;
            entity->state = 5;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 4) {
            entity->unk88 = -unkE0->unk48[0];
            unkE0->unk48[0] -= entity->params->unk43E * 0.1f;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            func_00192C98(entity, 9, 0, entity->params->unkC4 >> 1, 1);
            entity->state++;
        }
        break;
    case 3:
        entity->unk88 = -unkE0->unk48[0];
        unkE0->unk48[0] -= entity->params->unk43E * 0.1f;
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            entity->state++;
        }
        break;
    case 4:
        entity->unk88 = -unkE0->unk48[0];
        unkE0->unk48[0] -= entity->params->unk43E * 0.1f;
        landed = (s32)(ENTITY_FLAGS(entity) >> 44) & 1;
        if (landed == 1) {
            entity->unk84 = 0.0f;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            ENTITY_FLAGS(entity) |= 0x4000;
            ENTITY_FLAGS(entity) &= ~0x10000000;
            entity->state++;
        }
        break;
    case 5:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
