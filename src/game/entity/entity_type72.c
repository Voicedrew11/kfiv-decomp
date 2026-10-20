/* The behaviour of entity type 72 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers that function installs. */
#include "entity/entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
extern s32 entity_type_handlers[][2];

static void EntityType72_Think(Entity *entity, s32 run_now);

/* Set-up of a type 72 entity: clears its work area and installs the type's handlers. */
void EntityType72_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        // match: the pointer has to be formed inside the loop
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = EntityType72_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = NULL;
    ai_state->move = NULL;
    ai_state->dormant = NULL;
    ai_state->death = NULL;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    ENTITY_FLAG_BITS(entity)->not_targetable = TRUE;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: empty. */
static void EntityType72_Think(Entity *entity, s32 run_now) {
}
