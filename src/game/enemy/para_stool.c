/* ParaStool: the behaviour of entity type 43 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Para Stool. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern float player_state[];
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
double atan(double);
void *memset(void *, int, u32);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
float EntityMath_WrapAngle(float);
void Attack_RollPlayerAilments(void *);
s32 Attack_Run(Unk1B1470Request *, Unk1B1470Extra *);

static void ParaStool_Think(Entity *entity, s32 unknown);
static void ParaStool_ThinkCombat(Entity *entity, s32 unknown);
static void ParaStool_MeleeAttack0(Entity *entity);
static s32 ParaStool_CheckPartHitInPlayerView(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame, s32 current_frame, f32 angle);

/* Set-up of a type 43 entity: clears its work area and installs the type's handlers. */
void ParaStool_Init(Entity *entity) {
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
    ai_state->think = ParaStool_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = (void (*)(void))EntityAi_OnHit;
    ai_state->move = (void (*)(void))EntityAi_Move;
    ai_state->dormant = (void (*)(void))EntityAi_Dormant;
    ai_state->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with ParaStool_ThinkCombat for state 2. Counts down work
   word unk68, the pause before the next ParaStool_MeleeAttack0. */
static void ParaStool_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (ai_state->pause_timer > 0) {
        ai_state->pause_timer--;
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
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
        ParaStool_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but its one attack is
   ParaStool_MeleeAttack0, started only when the pause in work word unk68 is over (which it then
   sets to EntityParams.pitch_limit_deg); closing in never runs. */
static void ParaStool_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 range;
    f32 angle;
    s32 next_action;
    s32 side;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    range = 300.0f;
    angle = 0.5235988f;
    if (EntityAi_ShouldGuard(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    next_action = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->action_timer == 0 || next_action == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = next_action;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        // match: called with one argument here
        ((void (*)(Entity *))EntityAi_LowerChoiceWeight)(entity);
        switch (entity->combat_choice) {
        case 0:
            if (ai_state->pause_timer == 0) {
                ai_state->pause_timer = (u16)entity->params->pitch_limit_deg;
                EntityAi_StartAction(entity, ParaStool_MeleeAttack0, unknown);
                return;
            }
            ai_state->move_choice = -1;
            break;
        case 1:
            entity->target = player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = -1;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    if (entity->combat_choice == 1) {
        side = EntityAi_TurnFromPush(entity, direction);
        if (ai_state->move_choice == 0) {
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The melee attack with record 0 (busy bit 1): animation 20, as EntityAi_MeleeAttack, but only part
   0 is checked, by ParaStool_CheckPartHitInPlayerView. */
static void ParaStool_MeleeAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit55 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit56 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = ParaStool_CheckPartHitInPlayerView(entity, unk240, 0, elapsed, entity->model->level_tables->frame, 0.0f);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_80000;
        } else {
            entity->flags &= ~ENTITY_FLAG_80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* Helper of ParaStool_MeleeAttack0: the hit check of EntityAi_CheckMeleeHitAt and EntityAi_SendMeleeHit for one
   part of the record on the part's frames, written out, with conditions of its own: the entity must
   be within EntityParams.yaw_limit_deg degrees of the player's facing and unk434 degrees of the player's
   pitch, and Entity.heading_to_player within the angle of part 0 of its heading. It then calls Attack_RollPlayerAilments
   before the check (Attack_Run). */
static s32 ParaStool_CheckPartHitInPlayerView(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame, s32 current_frame, f32 angle) {
    Unk1B1470Request request;
    Unk1B1470Extra extra;
    f32 yaw;
    f32 pitch;
    f32 spread;

    if (record->hit_parts[index].first_frame != 0 && !(frame < record->hit_parts[index].first_frame && current_frame < record->hit_parts[index].first_frame) &&
        !(record->hit_parts[index].last_frame < frame && record->hit_parts[index].last_frame < current_frame)) {
        yaw = EntityMath_AngleDifference(player_state[5], EntityMath_AngleXZ(player_state, (char *)entity->model->position));
        pitch = (f32)atan((entity->model->position[1] - player_state[1] + 220.0f) / entity->distance_to_player) + player_state[4];
        if (ABS(yaw) < entity->params->yaw_limit_deg * 0.017453292f && ABS(pitch) < (s16)entity->params->action_frames * 0.017453292f) {
            spread = EntityMath_WrapAngle(record->hit_parts[0].half_angle_deg * 0.017453292f);
            if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < spread) {
                Attack_RollPlayerAilments(&entity->params->melee_attacks[0].hit_parts[0].unk12);
                memset(&request, 0, sizeof(request));
                memset(&extra, 0, sizeof(extra));
                Vector_Copy(request.position, entity->model->position);
                request.height = entity->height + 10.0f;
                request.radius = entity->radius + record->hit_parts[index].radius_bonus;
                request.facing = EntityMath_WrapAngle(angle + 3.1415927f);
                request.half_angle = EntityMath_WrapAngle(record->hit_parts[index].half_angle_deg * 0.017453292f);
                request.owner = entity;
                request.shape = 3;
                request.target_mask = 1;
                request.unk50 = record->hit_parts[index].unk00;
                request.unk62 = record->hit_parts[index].unk12;
                request.hit_armor_part = record->wear_armour_part;
                extra.is_magic_shot = 0;
                extra.strength_gauge = 0;
                extra.player_power = 0;
                extra.player_magic = 0;
                request.knockback = record->hit_parts[index].unk1E & 1;
                return Attack_Run(&request, &extra);
            }
        }
    }
    return 0;
}
