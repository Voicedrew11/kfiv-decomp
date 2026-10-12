#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
float func_00195388(float *, float, float);

/* moves a flying entity: turn, pitch down or up while unkC8 asks for it, pick the animation */
void func_00146158(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 value;
    f32 pitch;
    f32 rate;

    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (unkE0->unk1C > entity->params->turn_limit) {
            unkE0->unk1C = entity->params->turn_limit;
        }
    } else {
        unkE0->unk1C = 0.0f;
    }
    value = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        value += value;
    }
    entity->unk94 += value;
    if (entity->unkC8 & 0x10) {
        pitch = -0.5235988f;
    } else if (entity->unkC8 & 0x20) {
        pitch = 0.5235988f;
    } else {
        pitch = 0.0f;
    }
    if (ABS(pitch - entity->model->unkD0[0]) > 0.5235988f) {
        rate = 60.0f;
    } else {
        rate = 30.0f;
    }
    value = entity->model->unkD0[0];
    func_00195388(&value, pitch, rate);
    entity->unk90 += value - entity->model->unkD0[0];
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x10) {
        func_00192C98(entity, 8, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        func_00192C98(entity, 9, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}
