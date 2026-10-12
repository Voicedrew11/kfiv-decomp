#include "kf_entity.h"

float sqrtf(float);
float atan2f(float, float);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
void func_00195588(f32 *, f32);
void KF_Vector_Scale(float *, float *, float);
int func_001A1C58(int, void *, void *, void *);
int func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_001325B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    f32 position[4];
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unk240->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (entity->model->unkFC->unk82 >= 8) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            unkE0->unk68 = 15;
            entity->action_step++;
        }
        break;
    case 3:
        func_00195588(position, entity->model->unkD0[1]);
        KF_Vector_Scale(position, position, 30.0f);
        entity->unk84 += position[0];
        entity->unk88 += position[1];
        entity->unk8C += position[2];
        unkE0->unk68--;
        if (entity->distance_to_player - entity->radius - 100.0f < 200.0f || unkE0->unk68 < 0) {
            entity->action_step++;
        }
        break;
    case 4:
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        from = (f32 *)func_0010CAB8((char *)entity->model, 8);
        tip = (f32 *)func_0010CAB8((char *)entity->model, 9);
        if (from != NULL && tip != NULL) {
            if (entity->model->unkFC->unk82 >= 0x14 && entity->model->unkFC->unk82 <= 0x1A) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = from[0] - unkE0->unk48;
                delta_y = from[1] - unkE0->unk4C;
                delta_z = from[2] - unkE0->unk50;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk48 + delta_x * i / count;
                    position[1] = unkE0->unk4C + delta_y * i / count;
                    position[2] = unkE0->unk50 + delta_z * i / count;
                    func_001A1C58(0x3A0, position, angles, entity);
                }
            }
            unkE0->unk48 = from[0];
            unkE0->unk4C = from[1];
            unkE0->unk50 = from[2];
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!((s32)(entity->flags >> 54) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 55) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 56) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, elapsed, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk240->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
