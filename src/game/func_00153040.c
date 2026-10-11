#include "kf_entity.h"

extern int D_00414184[];

int func_0019A448(void *, void *, int, int, int, int, int);

void func_00153040(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
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
            func_0019A448(entity, &slots->unk80, 0, 4, 0, entity->params->unkC4 >> 1, 0);
            if (unkE0->unk79 == 0) {
                func_0019A448(entity, &slots->unk88, 1, 4, 0, entity->params->unkC4 >> 1, 0);
            }
        } else {
            func_0019A448(entity, &slots->unk80, 0, 3, 0, entity->params->unkC4 >> 1, 0);
            if (unkE0->unk79 == 0) {
                func_0019A448(entity, &slots->unk88, 1, 3, 0, entity->params->unkC4 >> 1, 0);
            }
        }
    } else if (entity->unkC8 & 0x300) {
        func_0019A448(entity, &slots->unk80, 0, 3, 0, entity->params->unkC4 >> 1, 0);
        if (unkE0->unk79 == 0) {
            func_0019A448(entity, &slots->unk88, 1, 3, 0, entity->params->unkC4 >> 1, 0);
        }
    } else {
        func_0019A448(entity, &slots->unk80, 0, 0, 0, entity->params->unkC4 >> 1, 0);
        if (unkE0->unk79 == 0) {
            func_0019A448(entity, &slots->unk88, 1, 0, 0, entity->params->unkC4 >> 1, 0);
        }
    }
}
