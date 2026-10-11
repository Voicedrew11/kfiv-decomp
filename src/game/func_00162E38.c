#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
float sqrtf(float);
float atan2f(float, float);
char *func_0010CAB8(char *, u32);
void func_0018D600(char **, int, u32, int);
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00163310(Entity *, s32);

void func_00162E38(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind162BC8 *aims = (EntityUnkE0Kind162BC8 *)unkE0;
    f32 delta[4];
    f32 *from;
    f32 *tip;
    f32 *first_aim;
    f32 *second_aim;
    u16 frames;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    unkE0->unk40 = 0.5f;
    first_aim = aims->unk80;
    second_aim = aims->unk90;
    from = (f32 *)func_0010CAB8((char *)entity->model, 0x1E);
    tip = (f32 *)func_0010CAB8((char *)entity->model, 0x1C);
    delta[0] = tip[0] - from[0];
    delta[1] = tip[1] - from[1];
    delta[2] = tip[2] - from[2];
    first_aim[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
    first_aim[1] = atan2f(delta[0], delta[2]);
    first_aim[2] = 0.0f;
    from = (f32 *)func_0010CAB8((char *)entity->model, 0x1F);
    tip = (f32 *)func_0010CAB8((char *)entity->model, 0x1D);
    delta[0] = tip[0] - from[0];
    delta[1] = tip[1] - from[1];
    delta[2] = tip[2] - from[2];
    second_aim[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
    second_aim[1] = atan2f(delta[0], delta[2]);
    second_aim[2] = 0.0f;
    func_0018D600((char **)entity, 0x3D7, 0x1E, (int)first_aim);
    func_0018D600((char **)entity, 0x3D7, 0x1F, (int)second_aim);
    if (unkE0->unk68 > 0) {
        if (unkE0->unk68 > (s16)entity->params->unk434 - entity->params->unk436) {
            from = (f32 *)func_0010CAB8((char *)entity->model, 4);
            tip = (f32 *)func_0010CAB8((char *)entity->model, 5);
            delta[0] = tip[0] - from[0];
            delta[1] = tip[1] - from[1];
            delta[2] = tip[2] - from[2];
            aims->unk30[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
            aims->unk30[1] = atan2f(delta[0], delta[2]);
            aims->unk30[2] = 0.0f;
            func_0018D600((char **)entity, rand() % 3 + 0x21F, 4, (int)aims->unk30);
            frames = entity->params->unk438;
            entity->params->unk240[0].unk10[0].unk00.unk00[8] = frames;
            entity->params->unk240[0].unk10[1].unk00.unk00[8] = frames;
            entity->params->unk240[0].unk10[2].unk00.unk00[8] = frames;
            entity->params->unk240[1].unk10[0].unk00.unk00[8] = frames;
            entity->params->unk240[1].unk10[1].unk00.unk00[8] = frames;
            entity->params->unk240[1].unk10[2].unk00.unk00[8] = frames;
            ((EntityParamsUnk240 *)entity->params->unk350)->unk10[0].unk00.unk00[8] = frames;
            ((EntityParamsUnk240 *)entity->params->unk350)->unk10[1].unk00.unk00[8] = frames;
            ((EntityParamsUnk240 *)entity->params->unk350)->unk10[2].unk00.unk00[8] = frames;
        }
        unkE0->unk68--;
    } else {
        entity->params->unk240[0].unk10[0].unk00.unk00[8] = 0;
        entity->params->unk240[0].unk10[1].unk00.unk00[8] = 0;
        entity->params->unk240[0].unk10[2].unk00.unk00[8] = 0;
        entity->params->unk240[1].unk10[0].unk00.unk00[8] = 0;
        entity->params->unk240[1].unk10[1].unk00.unk00[8] = 0;
        entity->params->unk240[1].unk10[2].unk00.unk00[8] = 0;
        ((EntityParamsUnk240 *)entity->params->unk350)->unk10[0].unk00.unk00[8] = 0;
        ((EntityParamsUnk240 *)entity->params->unk350)->unk10[1].unk00.unk00[8] = 0;
        ((EntityParamsUnk240 *)entity->params->unk350)->unk10[2].unk00.unk00[8] = 0;
    }
    if (unkE0->unk6C > 0) {
        unkE0->unk6C--;
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk6C = entity->params->unk43A;
    }
    if (unkE0->unk6A > 0) {
        if (D_00446ED4 % 3U == 0) {
            func_0018D600((char **)entity, 0x25C, 0, (int)entity->model->unkD0);
        }
        ENTITY_FLAGS(entity) |= 0x8000L << 20;
        unkE0->unk6A--;
    } else {
        ENTITY_FLAGS(entity) &= ~(0x8000L << 20);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_00163310(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
