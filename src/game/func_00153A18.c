#include "kf_entity.h"

extern float D_00413F80[];

float func_00195478(float *, char *);
float func_001957B8(float, float);
int func_0019A448(void *, void *, int, int, int, int, int);
int func_00199EB8(void *, void *, float);
void func_001958B8(char *, void (*)(void), int);

void func_00153A18(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        if (func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position)) * 57.29578f > 0.0f) {
            func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 6, 0, entity->params->unkC4 >> 1, 1);
            if (unkE0->unk79 == 0) {
                func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 6, 0, entity->params->unkC4 >> 1, 1);
            }
        } else {
            func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 7, 0, entity->params->unkC4 >> 1, 1);
            if (unkE0->unk79 == 0) {
                func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->action_step++;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
