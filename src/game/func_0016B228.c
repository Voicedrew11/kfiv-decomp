#include "kf_entity.h"
#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern EntitySpawnInfo *D_004467D0[2];

int func_00192C98(void *, int, int, u32, int);
void func_001035A8(u32, int);
void func_001A32A8(Entity *);

void func_0016B228(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0, 0, entity->params->unkC4, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x40000;
        entity->flags |= 0x100000;
        entity->flags |= 0x200000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            ENTITY_FLAG_BITS(entity)->bit26 = 1;
            entity->unkA0 = 20;
            func_001035A8(D_0038CD20[entity->group_id].tag, entity->spawn_info - D_004467D0[entity->group_id]);
            func_001A32A8(entity);
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}
