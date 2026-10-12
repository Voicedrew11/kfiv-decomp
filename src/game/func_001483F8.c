#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0018D600(char **, int, u32, int);
void func_00216778(void *, void *);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

extern float D_00413F80[];

void func_001483F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->unk3D8[0];
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            unkE0->unk7A = 1;
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            unkE0->unk7A = 1;
        } else {
            unkE0->unk7A = 2;
        }
        if (entity->model->unkFC->unk82 == 0x16 && (entity->unk2A == 0 || unk3D8->unk0C != 0)) {
            func_0018D600((char **)entity, 0x373, 4, (int)entity->model->unkD0);
            func_0018D600((char **)entity, 0x373, 8, (int)entity->model->unkD0);
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(target, D_00413F80);
        target[1] -= 190.0f;
        func_0018CF30(entity, (EntityParamsUnk3D8 *)unk3D8, elapsed, entity->model->unkFC->unk82, target, 1.5707964f, 1.5707964f);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
