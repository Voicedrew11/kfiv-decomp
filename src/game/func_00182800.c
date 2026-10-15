#include "kf_entity.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Think(Entity *, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);

extern s32 D_003E6E40[][2];

void func_00182800(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = EntityAi_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26) {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction((char *)entity, unkE0->dormant, 1);
    } else {
        EntityAi_StartAction((char *)entity, unkE0->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}
