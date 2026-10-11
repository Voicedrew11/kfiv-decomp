#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_00192C98(void *, int, int, u32, int);

void func_00168C10(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    EntityParams *params;
    f32 turn;

    if (entity == D_00413F80.unk204 && (entity->params->unk10 & 0x800)) {
        entity->unkC8 = 0;
    }
    params = entity->params;
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= params->unk58;
        if (unkE0->unk1C < -params->unk5C) {
            unkE0->unk1C = -params->unk5C;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += params->unk58;
        if (params->unk5C < unkE0->unk1C) {
            unkE0->unk1C = params->unk5C;
        }
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x440) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, params->unkC4, 0);
        } else {
            func_00192C98(entity, 3, 0, params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        func_00192C98(entity, 5, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        func_00192C98(entity, 8, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        func_00192C98(entity, 9, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        func_00192C98(entity, 6, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 8) {
        func_00192C98(entity, 7, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 3, 0, params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, params->unkC4, 0);
    }
}
