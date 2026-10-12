#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);

void func_00138608(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn = 0.0f;

    if (entity->unkC8 & 0x41) {
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
            unkE0->unk1C = turn;
        }
    } else {
        unkE0->unk1C = turn;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkD1 != 3 && entity->unkD1 != 4) {
            if (entity->unkC8 & 0x40) {
                func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 3, 0, entity->params->unkC4, 1);
            }
        } else if (entity->unkC8 & 0x40) {
            entity->unkD1 = 4;
            entity->model->unkFC->unk8C = entity->params->unk80[entity->unkD1];
        } else {
            entity->unkD1 = 3;
            entity->model->unkFC->unk8C = entity->params->unk80[entity->unkD1];
        }
    } else if (entity->unkC8 & 0x100) {
        func_00192C98(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x200) {
        func_00192C98(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
    entity->unkC6 = 0;
    entity->unkC8 = 0;
    entity->unkA0 = 0;
}
