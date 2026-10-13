#include "kf_entity.h"

extern u32 D_00446ED4;

int rand(void);
float sinf(float);
float cosf(float);
float atan2f(float, float);
int func_00192C98(void *, int, int, u32, int);
void func_0018D600(char **, int, u32, int);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
void func_00195898(char *, u8);
void func_001958B8(char *, void (*)(void), int);
void func_00195588(f32 *, f32);
void Vector_Scale(float *, float *, float);
void func_0016F348(Entity *);

void func_0016F930(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 max_angle;
    f32 min_angle;
    f32 half;
    f32 degree;
    f32 launch_angle;
    f32 angle;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        half = 0.5f;
        func_00192C98(entity, 0x12, 0, entity->params->unkC4, 1);
        degree = 0.017453292f;
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->bit23 = 0;
        ENTITY_FLAG_BITS(entity)->bit24 = 0;
        unkE0->unk54 = entity->model->unkE0[0];
        unkE0->unk58 = entity->model->unkE0[1];
        unkE0->unk5C = entity->model->unkE0[2];
        entity->model->unkE0[0] = 0.75f;
        entity->model->unkE0[1] = 0.75f;
        entity->model->unkE0[2] = half;
        min_angle = 105.0f - (f32)(s16)entity->params->unk434 * half;
        max_angle = (f32)(s16)entity->params->unk434 * half + -130.0f;
        angle =
            entity->unk68 + ((f32)(rand() % ((s16)entity->params->unk434 + 1)) - (f32)(s16)entity->params->unk434 * half) * degree;
        entity->model->unkD0[1] = angle;
        max_angle *= degree;
        min_angle *= degree;
        if (entity->model->unkD0[1] < min_angle && 0.0f <= entity->model->unkD0[1]) {
            entity->model->unkD0[1] = min_angle;
        }
        if (max_angle < entity->model->unkD0[1] && entity->model->unkD0[1] < 0.0f) {
            entity->model->unkD0[1] = max_angle;
        }
        unkE0->unk50 =
            (entity->distance_to_player + (f32)(rand() % (entity->params->unk436 + 1)) - (f32)entity->params->unk436 * 0.5f - 500.0f) /
                1300.0f * 24.0f + 24.0f;
        if (unkE0->unk50 < 24.0f) {
            unkE0->unk50 = 24.0f;
        }
        if (48.0f < unkE0->unk50) {
            unkE0->unk50 = 48.0f;
        }
        launch_angle = 0.6981317f;
        unkE0->unk48 = -unkE0->unk50 * sinf(launch_angle);
        unkE0->unk4C = unkE0->unk50 * cosf(launch_angle);
        unkE0->unk68 = 0;
        unkE0->unk7C = 0;
        entity->action_step++;
        break;
    case 1:
        unkE0->unk48 += (f32)entity->params->unk430 * ((f32)unkE0->unk68 / 30.0f);
        if ((f32)entity->params->unk432 < unkE0->unk48) {
            unkE0->unk48 = (f32)entity->params->unk432;
        }
        if (D_00446ED4 % 3U == 0) {
            func_0018D600((char **)entity, entity->params->unk43C, 0xFF, (int)entity->model->unkD0);
        }
        if (unkE0->unk7C == 0) {
            unkE0->unk7C = func_00195AC0(entity, entity->params->unk240, 0, 1, 2);
        }
        if ((s32)(entity->flags >> 44) & 1) {
            ENTITY_FLAG_BITS(entity)->bit27 = 0;
            entity->model->unkD0[0] = 0.0f;
            unkE0->unk6A = 600;
            entity->model->unkE0[0] = unkE0->unk54;
            entity->model->unkE0[1] = unkE0->unk58;
            entity->model->unkE0[2] = unkE0->unk5C;
            unkE0->unk7B = 1;
            func_0018D600((char **)entity, entity->params->unk43E, 0xFF, (int)entity->model->unkD0);
            func_00195898((char *)entity, 2);
            entity->flags &= ~0x200000;
            entity->flags &= ~0x100000;
            entity->flags &= ~0x40000;
            entity->flags &= ~0x10000000;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_001958B8((char *)entity, (void (*)(void))func_0016F348, 1);
            return;
        }
        break;
    }
    unkE0->unk68++;
    func_00195588(direction, entity->model->unkD0[1]);
    Vector_Scale(direction, direction, unkE0->unk4C);
    entity->unk84 += direction[0];
    entity->unk88 += unkE0->unk48;
    entity->unk8C += direction[2];
    entity->model->unkD0[0] = atan2f(unkE0->unk48, unkE0->unk4C);
}
