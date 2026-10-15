#include "kf_entity.h"

extern u32 D_00446ED4;

int EntityAi_ThinkReturn(void *, int);
int func_00185460(void *, int);
void EntityAi_SensePlayer(Entity *, s32);
void EntityAi_CheckFarFromSpawn(char *);

void func_00185390(Entity *entity, s32 run_now) {
    s32 mask = ~0x20000000;

    entity->flags &= mask;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    if ((s32)(entity->flags >> 53) & 1) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    func_00185460(entity, run_now);
}
