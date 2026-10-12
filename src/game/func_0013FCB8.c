#include "kf_entity.h"

extern float D_00413F80[];

float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
float func_00194F80(void *, void *);
float func_00195478(float *, char *);
float func_00195018(float, float, float);
void func_00216778(void *, void *);
int func_00195E48(void *, int, void *, void *);
void func_0018DA38(Entity *, s32, s32, s32);
int func_00192C98(void *, int, int, u32, int);
int rand(void);
void func_001958B8(char *, void (*)(void), int);

void func_0013FCB8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angles[4];
    f32 target[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;

    if (entity->unkA0 == 0 && (s16)entity->params->unk434 != 0) {
        from = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432);
        tip = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432 + 1);
        if (from == NULL || tip == NULL) {
            return;
        }
        if (entity->params->unk432 != 0xFF) {
            delta_x = tip[0] - from[0];
            delta_y = tip[1] - from[1];
            delta_z = tip[2] - from[2];
            angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
            angles[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
            angles[2] = angles[3] = 0.0f;
        } else {
            func_00216778(angles, entity->model->unkD0);
            angles[1] = func_00194FB8(angles[1] + 3.1415927f);
        }
        func_00216778(target, D_00413F80);
        target[1] -= 190.0f;
        pitch = func_00194FB8(-atan2(target[1] - from[1], func_00194F80(from, target)));
        yaw = func_00195478(from, (char *)target);
        angles[0] = func_00195018(angles[0], pitch, entity->params->unk436);
        angles[1] = func_00195018(angles[1], yaw, entity->params->unk438);
        func_00195E48(entity, entity->params->unk430, from, angles);
        func_0018DA38(entity, entity->params->unk43A, entity->params->unk43C, entity->params->unk43E);
        entity->unkA0 = entity->params->unk434 + rand() % 3;
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
