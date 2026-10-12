#include "kf_entity.h"

float func_00195388(float *, float, float);
int func_0019A448(void *, void *, int, int, int, int, int);

void func_00135280(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 value;
    f32 roll;
    f32 pitch;
    f32 pitch_change;
    f32 pitch_rate;

    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
        roll = 0.34906584f;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (unkE0->unk1C > entity->params->turn_limit) {
            unkE0->unk1C = entity->params->turn_limit;
        }
        roll = -0.34906584f;
    } else {
        unkE0->unk1C = 0.0f;
        roll = unkE0->unk1C;
    }
    value = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        value += value;
    }
    entity->unk94 += value;
    func_00195388(&entity->model->unkD0[2], roll, 30.0f);
    if (entity->unkC8 & 0x10) {
        pitch = -0.5235988f;
    } else if (entity->unkC8 & 0x20) {
        pitch = 0.5235988f;
    } else {
        pitch = 0.0f;
    }
    pitch_change = pitch - entity->model->unkD0[0];
    if (ABS(pitch_change) > 0.5235988f) {
        pitch_rate = 60.0f;
    } else {
        pitch_rate = 30.0f;
    }
    value = entity->model->unkD0[0];
    func_00195388(&value, pitch, pitch_rate);
    entity->unk90 += value - entity->model->unkD0[0];
    func_0019A448(entity, unkE0->unk80, 0, 3, 0, entity->params->unkC4, 0);
}
