#include "kf_entity.h"

extern int D_00414184[];

int func_00192C98(void *, int, int, u32, int);

void func_0013F6A0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    if (entity == (Entity *)D_00414184[0] && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
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
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x100) {
        func_00192C98(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x200) {
        func_00192C98(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}
