#include "kf_entity.h"

extern PlayerData D_00413F80;

void func_0019A050(Entity *, f32 *, void *);
int func_00192C98(void *, int, int, u32, int);

void func_0012B880(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 vector[4];
    f32 turn;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    vector[0] = vector[1] = vector[2] = vector[3] = turn = 0.0f;
    if (entity->unkC8 & 0x800) {
        if (entity->target != NULL) {
            func_0019A050(entity, &unkE0->unk1C, entity->target);
        }
    } else if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
        turn = unkE0->unk1C;
        if (entity->unkC8 & 0x400) {
            turn += turn;
        }
        entity->unk94 += turn;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
        turn = unkE0->unk1C;
        if (entity->unkC8 & 0x400) {
            turn += turn;
        }
        entity->unk94 += turn;
    } else {
        unkE0->unk1C = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, (s32)((f32)entity->params->unkC4 * 0.5f), 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x8000) {
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x800) {
        func_00192C98(entity, 4, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}
