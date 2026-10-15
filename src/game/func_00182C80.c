#include "kf_entity.h"

extern u32 D_00446ED4;

void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void func_0018E9C0(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
void EntityAi_ThinkCombat(Entity *, s32);

/* The default think handler: every tenth frame updates what the entity knows of the player
   (EntityAi_SensePlayer); while the entity is not busy it steps Entity.state and calls the think function
   of that state. */
void EntityAi_Think(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (!(ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26)) {
        EntityAi_CheckFarFromSpawn((char *)entity);
    }
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        EntityAi_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}
