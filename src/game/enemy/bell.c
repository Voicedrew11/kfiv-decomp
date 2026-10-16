/* Bell: the behaviour of entity type 38 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: chr.prm record 60, model c0060
   (a bell), 65,535 hit points, one spawn in 007.map. Struck, it calls one of the falloff_sound_players handlers
   (Sound_PlayFalloff), then waits EntityParams.action_frames frames before it answers another hit. Prima's
   official guide (2004): shoot the bell with a bow to ring it and destroy the skeletons. */
#include "entity/entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
extern PlayerData player_state;
float EntityMath_AngleXZ(float *, char *);
float EntityMath_WrapAngle(float);
// match: the definition names only the three integer arguments; the floats go on to the handler it calls in their registers
void Sound_PlayFalloff(s32, s32, s32, f32, f32, f32);

static void Bell_Animate(Entity *entity);
static void Bell_Think(Entity *entity);

/* Set-up of a type 38 entity: clears its work area and installs the type's handlers. */
void Bell_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = (void (*)(Entity *, s32))Bell_Think;
    unkE0->animate = (void (*)(void))Bell_Animate;
    unkE0->on_hit = NULL;
    unkE0->move = NULL;
    unkE0->dormant = NULL;
    unkE0->death = NULL;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    ENTITY_FLAG_BITS(entity)->bit18 = 1;
    ENTITY_FLAG_BITS(entity)->bit21 = 1;
    ENTITY_FLAG_BITS(entity)->bit16 = 1;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(unkE0->look_part_angles, unk138 + 0x50);
    }
    unkE0->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: plays the motion a step on (Motion_Step) and sets flag bit 34 when it has
   ended; no blend and no head turn. */
static void Bell_Animate(Entity *entity) {
    ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
}

/* The think handler (it takes the entity only): no states and no actions. In a frame with flag bit
   43 set and the timer Entity.action_timer at 0 it sets flag bit 29, calls Sound_PlayFalloff with
   EntityParams.yaw_limit_deg, unk430 and unk42E, Entity.sound_bank, Entity.distance_to_player_3d and the angle of the entity
   off the player's facing, and sets the timer to EntityParams.action_frames; otherwise flag bit 29 is
   clear. It always clears flag bit 43. */
static void Bell_Think(Entity *entity) {
    u64 flags;
    f32 angle;

    flags = entity->flags & ~ENTITY_FLAG_20000000;
    entity->flags = flags;
    if ((s32)(flags >> 43) & 1) {
        if (entity->action_timer == 0) {
            entity->flags = flags | ENTITY_FLAG_20000000;
            angle = EntityMath_AngleXZ(player_state.position, (char *)entity->model->position);
            angle = EntityMath_WrapAngle(angle - player_state.heading);
            Sound_PlayFalloff((u8)entity->params->yaw_limit_deg, (u8)entity->params->pitch_limit_deg, entity->sound_bank,
                          (f32)entity->params->sound_id, entity->distance_to_player_3d, angle);
            entity->action_timer = entity->params->action_frames;
        }
    }
    entity->flags &= ~ENTITY_FLAG_HIT_BY_PLAYER;
}
