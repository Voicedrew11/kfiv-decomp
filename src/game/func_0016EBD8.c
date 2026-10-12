#include "kf_entity.h"

float atan2f(float, float);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
int func_00195E48(void *, int, void *, void *);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_0016EBD8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Array *unk3D8 = entity->params->unk3D8;
    f32 direction[4];
    f32 *base;
    f32 *tip;
    f32 delta_x;
    f32 delta_z;
    s32 elapsed;
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->record.unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->record.unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        for (frame = (s16)entity->params->unk434; frame < entity->params->unk436; frame += entity->params->unk438) {
            if ((entity->unk2A == 0 || unk3D8->unk0C != 0) && elapsed < frame && entity->model->unkFC->unk82 >= frame) {
                base = (f32 *)func_0010CAB8((char *)entity->model, unk3D8->unk11);
                tip = (f32 *)func_0010CAB8((char *)entity->model, unk3D8->unk11 + 1);
                if (base != NULL && tip != NULL) {
                    delta_x = tip[0] - base[0];
                    delta_z = tip[2] - base[2];
                    direction[0] = -0.08726646f;
                    direction[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                    direction[2] = direction[3] = 0.0f;
                    func_00195E48(entity, unk3D8->unk10, base, direction);
                }
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk3D8->record.unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->record.unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
