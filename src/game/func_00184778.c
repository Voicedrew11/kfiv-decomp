#include "kf_entity.h"

extern u8 D_00349690[0x80][0x40];

int func_00192C98(void *, int, int, u32, int);
void func_0019A290(Entity *, s32);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void func_001958B8(char *, void (*)(void), int);

void func_00184778(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParams *params;
    s32 kind;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        if (entity->spawn_info->unk40 == 1
            && D_00349690[entity->spawn_info->variant][entity->spawn_info->unk42] == entity->spawn_info->unk43) {
            func_00192C98(entity, 0x12, 0, 5, 1);
        } else {
            func_00192C98(entity, 0xF, 0, 5, 1);
        }
        params = entity->params;
        if (params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            kind = 1;
            if ((((EntityParamsWord *)params)->unk00 & 0x6000000) == 0x2000000) {
                kind = 2;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (!(((EntityParamsWord *)params)->unk00 & 0x06000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_0019A290(entity, entity->params->unkC8);
            func_0019A310((char *)entity, NULL, 0);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
