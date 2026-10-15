/* The behaviour of entity type 72 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 D_003E6E40[][2];

static void EntityType72_Think(void);

/* Set-up of a type 72 entity: clears its work area and installs the type's handlers. */
void EntityType72_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        // match: the pointer has to be formed inside the loop
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = (void (*)(Entity *, s32))EntityType72_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = NULL;
    unkE0->move = NULL;
    unkE0->dormant = NULL;
    unkE0->death = NULL;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: empty. It takes no arguments. */
static void EntityType72_Think(void) {
}
