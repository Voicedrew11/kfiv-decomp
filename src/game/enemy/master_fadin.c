/* MasterFadin: the behaviour of entity type 79 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Master Fadin. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "entity/entity_world.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 entity_type_handlers[][2];
void EntityEvent_Respawn(char *);
int Motion_Start(void *, int, int, u32, int);
extern float player_state[];
float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *Model_GetSkinnedBonePoint(char *, u32);
float EntityMath_WrapAngle(float);
float EntityMath_DistanceXZ(void *, void *);
float EntityMath_AngleXZ(float *, char *);
f32 EntityMath_Approach(f32, f32, f32);
void Vector_Copy(void *, void *);
int EntityAi_LaunchShot(void *, int, void *, void *);
void EntityEvent_StartCue(Entity *, s32, s32, s32);
void EntityEvent_OnDeath(Entity *);

static void MasterFadin_Think(Entity *entity, s32 unknown);
static void MasterFadin_ThinkShootAtPlayer(Entity *entity, s32 unknown);
static void MasterFadin_Dormant(Entity *entity);
static void MasterFadin_Death(Entity *entity);
static void MasterFadin_AlertPartner(Entity *entity);

/* Set-up of a type 79 entity: clears its work area and installs the type's handlers. */
void MasterFadin_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = MasterFadin_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->dormant = (void (*)(void))MasterFadin_Dormant;
    ai_state->death = (void (*)(void))MasterFadin_Death;
    ai_state->on_hit = NULL;
    ai_state->move = NULL;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: it sets Entity.state itself and does not call EntityAi_StepState. In mode 2
   (EntitySpawnInfo.spawn_mode) it takes the state of the entity whose spawn id is
   EntitySpawnInfo.variant, and starts the death action once that one's flag bits 2-3 are 2 or more.
   In mode 1 it calls EntityEvent_Respawn for every other entity with the same params whose flag bit 1 is
   clear or whose bits 2-3 are 3. While the entity is not busy it calls MasterFadin_ThinkShootAtPlayer in states 1
   and 2, and in state 0 starts what the move slot holds (the set-up function leaves it NULL). Keeps
   animation 0; once work word unk68 has run out it puts the state back to 0. */
static void MasterFadin_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    Entity *leader;
    Entity *other;

    if (entity->spawn_info->spawn_mode == 2) {
        leader = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (leader != NULL) {
            if (ENTITY_FLAG_BITS(leader)->bits2 >= 2) {
                // match: called as returning int; only then are the flags kept in $3 afterwards
                ((int (*)(char *, void (*)(void), int))EntityAi_StartAction)((char *)entity, ai_state->death, 0);
                ENTITY_FLAG_BITS(entity)->bit7 = FALSE;
                return;
            }
            entity->state = leader->state;
        }
    }
    if (entity->spawn_info->spawn_mode == 1) {
        for (other = active_entities_head; other != NULL; other = other->next) {
            if (other->params == entity->params && other != entity && ((s32)other->flags & ENTITY_FLAG_ACTIVE_LIST)) {
                if (!ENTITY_FLAG_BITS(other)->bit1 || ENTITY_FLAG_BITS(other)->bits2 >= 3) {
                    EntityEvent_Respawn((char *)other);
                }
            }
        }
    }
    if (entity->busy_flags == 0) {
        switch (entity->state) {
        case 1:
        case 2:
            MasterFadin_ThinkShootAtPlayer(entity, unknown);
            break;
        case 0:
        default:
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
            break;
        }
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    if (ai_state->pause_timer > 0) {
        ai_state->pause_timer--;
    } else {
        entity->state = 0;
    }
}

/* The think function of states 1 and 2: each time Entity.action_timer has run out it launches shot EntityParams.pitch_limit_deg
   from model point unk432 (along the line to the next point; point 0xFF: along the heading turned round),
   turned towards the player 190 above his feet by at most unk436 in pitch and unk438 in yaw, starts cue 0 and
   sets unkA0 to unk434. Keeps animation 0 and starts the move action. */
static void MasterFadin_ThinkShootAtPlayer(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angles[4];
    f32 target[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;

    if (entity->action_timer == 0) {
        from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, entity->params->yaw_limit_deg);
        tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, entity->params->yaw_limit_deg + 1);
        if (from != NULL && tip != NULL) {
            if (entity->params->yaw_limit_deg != 0xFF) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
            } else {
                Vector_Copy(angles, entity->model->rotation);
                angles[1] = EntityMath_WrapAngle(angles[1] + 3.1415927f);
            }
            Vector_Copy(target, player_state);
            target[1] -= 190.0f;
            pitch = EntityMath_WrapAngle(-atan2(target[1] - from[1], EntityMath_DistanceXZ(from, target)));
            yaw = EntityMath_AngleXZ(from, (char *)target);
            angles[0] = EntityMath_Approach(angles[0], pitch, entity->params->near_distance);
            angles[1] = EntityMath_Approach(angles[1], yaw, entity->params->far_distance);
            EntityAi_LaunchShot(entity, entity->params->pitch_limit_deg, from, angles);
            EntityEvent_StartCue(entity, 0, 0, 0);
            entity->action_timer = entity->params->action_frames;
        }
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant with flag bits 11 to 13 at 0; flag bit 43
   does not end the wait, it does not go to state 2, and it leaves flag bit 28 set. */
static void MasterFadin_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->wake_distance) {
            Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; in mode 2 it calls MasterFadin_AlertPartner where that
   one calls EntityEvent_OnDeath. */
static void MasterFadin_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags = (entity->flags & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_DYING_1;
        entity->flags |= ENTITY_FLAG_NO_PLAYER_COLLISION;
        entity->flags |= ENTITY_FLAG_NO_ENTITY_COLLISION;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->action_timer = 20;
            if (entity->spawn_info->spawn_mode != 2) {
                EntityEvent_OnDeath(entity);
            } else {
                MasterFadin_AlertPartner(entity);
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= ENTITY_FLAG_DYING_MASK;
            entity->flags &= ~ENTITY_FLAG_LIVE;
        }
        break;
    }
}

/* Helper of MasterFadin_Death: when the dying entity's partner (spawn id EntitySpawnInfo.variant)
   has flag bit 1 set and bits 2-3 at 0, puts it in state 2 and reloads its work word unk68 from
   EntityParams.unk43A. */
static void MasterFadin_AlertPartner(Entity *entity) {
    Entity *partner;

    partner = Entity_GetBySpawnId(entity->spawn_info->variant);
    if (partner != NULL) {
        if ((partner->flags & (ENTITY_FLAG_LIVE | ENTITY_FLAG_DYING_MASK)) == 2) {
            partner->state = 2;
            partner->ai_state.pause_timer = partner->params->unk43A;
        }
    }
}
