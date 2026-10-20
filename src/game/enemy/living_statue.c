/* LivingStatue: the behaviour of entity type 82 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Living Statue. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Think(Entity *, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
void EntityAi_OnHit(Entity *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Start(void *, int, int, u32, int);

static void LivingStatue_ThinkHoldAnim1(Entity *entity, s32 run_now);

/* Set-up of a type 82 entity: clears its work area and installs the type's handlers. */
void LivingStatue_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    if (entity->spawn_info->spawn_mode != 0) {
        ai_state->animate = EntityAi_Animate;
        ai_state->think = LivingStatue_ThinkHoldAnim1;
    } else {
        ai_state->think = EntityAi_Think;
        ai_state->animate = EntityAi_Animate;
        ai_state->on_hit = EntityAi_OnHit;
        ai_state->move = EntityAi_Move;
        ai_state->dormant = EntityAi_Dormant;
        ai_state->death = EntityAi_Death;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler of a spawn whose EntitySpawnInfo.spawn_mode is set: keeps the entity in animation 1
   with flag bits 16, 18, 21 to 25 and 28 set and starts no action. */
static void LivingStatue_ThinkHoldAnim1(Entity *entity, s32 run_now) {
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    Motion_Start(entity, 1, 0, 0, 0);
}
