#include "kf_entity.h"

extern float D_00413F84[];

float atan2f(float, float);
float func_00195388(float *, float, float);
float func_001954A8(float *, float *);
float func_001957B8(float, float);

void func_00149290(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->unkF8;
    f32 pitch;
    f32 pitch_limit;
    f32 half_pitch;
    f32 yaw;
    f32 yaw_limit;
    f32 half_yaw;
    f32 yaw_to_target;

    if (entity->target == NULL || entity->unkC6 != 0) {
        unkE0->unk48 = parts[30].unk50[0];
        unkE0->unk4C = parts[30].unk50[1];
        unkE0->unk50 = parts[31].unk50[0];
        unkE0->unk54 = parts[31].unk50[1];
    } else {
        pitch = atan2f(D_00413F84[0] - entity->model->position[1], entity->distance_to_player);
        pitch_limit = 0.5235988f;
        if (pitch_limit < pitch) {
            pitch = pitch_limit;
        } else if (pitch < -pitch_limit) {
            pitch = -pitch_limit;
        }
        half_pitch = pitch * 0.5f;
        func_00195388(&unkE0->unk48, half_pitch, 10.0f);
        func_00195388(&unkE0->unk50, half_pitch, 10.0f);
        parts[20].unk50[0] = unkE0->unk48;
        parts[21].unk50[0] = unkE0->unk50;
        yaw_to_target = func_001954A8(entity->model->position, entity->target);
        yaw = func_001957B8(entity->model->unkD0[1], yaw_to_target);
        yaw_limit = 0.6981317f;
        if (yaw_limit < yaw) {
            yaw = yaw_limit;
        } else if (yaw < -yaw_limit) {
            yaw = -yaw_limit;
        }
        half_yaw = yaw * 0.5f;
        func_00195388(&unkE0->unk4C, half_yaw, 10.0f);
        func_00195388(&unkE0->unk54, half_yaw, 10.0f);
        parts[20].unk50[1] = unkE0->unk4C;
        parts[21].unk50[1] = unkE0->unk54;
    }
}
