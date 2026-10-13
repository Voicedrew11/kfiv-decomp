#include "kf_entity.h"
#include "kf_shot.h"

float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
float func_00194F80(void *, void *);
float func_00195478(float *, char *);
float func_00195018(float, float, float);
void func_00216778(void *, void *);
void func_001B4F30(u8, void *, void *, void *, void *);

/* launches both shots of a record at once, whatever the animation frame */
s32 func_0017F0F0(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;
    s32 i;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        shot = Shot_Alloc();
        shot->unk22 = shot->unk20 = 0;
        shot->unk00 = 1;
        shot->unk28 = entity->params->unk238;
        from = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
        tip = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01 + 1);
        if (from != NULL && tip != NULL) {
            if (record->unk10[i].unk01 != 0xFF) {
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
            if (target != NULL) {
                pitch = func_00194FB8(-atan2(target[1] - from[1], func_00194F80(from, target)));
                yaw = func_00195478(from, (char *)target);
                angles[0] = func_00195018(angles[0], pitch, pitch_rate);
                angles[1] = func_00195018(angles[1], yaw, yaw_rate);
            }
            func_001B4F30(record->unk10[i].unk00, shot, from, angles, entity);
        }
    }
    return 0;
}
