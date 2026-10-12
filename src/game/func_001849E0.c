#include "kf_entity.h"

extern u8 D_00349690[0x80][0x40];
extern char D_0034B690[0x40];
extern EntityGroupRecord D_0034B6D0[];

int func_00192C98(void *, int, int, u32, int);
void func_0018DCD8(Entity *);

void func_001849E0(Entity *entity) {
    EntityParams *params;
    s32 kind;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        if (entity->spawn_info->unk40 == 1
            && D_00349690[entity->spawn_info->variant][entity->spawn_info->unk42] == entity->spawn_info->unk43) {
            func_00192C98(entity, 0x13, 0, 5, 1);
        } else {
            func_00192C98(entity, 0x10, 0, 5, 1);
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
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkA0 = 20;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            func_0018DCD8(entity);
            D_0034B6D0[ENTITY_CONTROLLER(entity)->unk43A].unk00 = ((s32)(entity->flags >> 38) & 3) ? 1 : 2;
            if ((s32)(entity->flags >> 38) & 3) {
                D_0034B690[0xD] = 1;
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}
