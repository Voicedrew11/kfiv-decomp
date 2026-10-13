#include "kf_entity.h"
#include "kf_gold.h"
#include "kf_map.h"
#include "kf_effect.h"

extern EntitySpawnInfo *D_004467D0[2];
extern MapSlot D_0038CD20[];
extern int D_00446770;

s32 func_001C8CB0(Entity *);
void func_001BBB60(s32);
void func_001035A8(u32, int);
int func_002109C8(int, int);

/* what every kind does when its entity dies */
void func_0018DCD8(Entity *entity) {
    func_001C8CB0(entity);
    KF_Gold_DropFromEntity(entity);
    if ((s32)(entity->flags >> 38) & 3) {
        func_001BBB60(entity->params->unk0E);
    }
    func_001035A8(D_0038CD20[entity->group_id].tag, entity->spawn_info - D_004467D0[entity->group_id]);
    KF_Effect_FreeForOwner(entity);
    if (entity->spawn_info->unk36 != 0 && entity->group_id == D_00446770) {
        func_002109C8(entity->spawn_info->unk36, 0);
    }
}
