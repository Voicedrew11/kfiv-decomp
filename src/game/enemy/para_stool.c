/* ParaStool: the behaviour of entity type 43 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Para Stool. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "core/file.h"
#include "enemy/enemy.h"
#include "player/player.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include <math.h>
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_math.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

static void ParaStool_Think(Entity *entity, s32 run_now);
static void ParaStool_ThinkCombat(Entity *entity, s32 run_now);
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
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = ParaStool_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
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

/* The think handler: as EntityAi_Think, with ParaStool_ThinkCombat for state 2. Counts down
   ai_state.pause_timer, the pause before the next ParaStool_MeleeAttack0. */
static void ParaStool_Think(Entity *entity, s32 run_now) {
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
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    switch (entity->state) {
    case ENTITY_STATE_UNAWARE:
        EntityAi_ThinkIdle(entity, run_now);
        return;
    case ENTITY_STATE_SEARCHING:
        EntityAi_ThinkPursue(entity, run_now);
        return;
    case ENTITY_STATE_NOTICED:
        ParaStool_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but its one attack is
   ParaStool_MeleeAttack0, started only when the pause in ai_state.pause_timer is over (which it then
   sets to EntityParams.pitch_limit_deg); closing in never runs. */
static void ParaStool_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 range;
    f32 angle;
    s32 next_action;
    s32 side;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    range = 300.0f;
    angle = DEG_TO_RAD(30);
    if (EntityAi_ShouldGuard(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    next_action = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (entity->action_timer == 0 || next_action == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = next_action;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, next_action);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (ai_state->pause_timer == 0) {
                ai_state->pause_timer = (u16)entity->params->pitch_limit_deg;
                EntityAi_StartAction(entity, ParaStool_MeleeAttack0, run_now);
                return;
            }
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (entity->combat_choice == COMBAT_CHOICE_CLOSE_IN) {
        side = EntityAi_TurnFromPush(entity, direction);
        if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The melee attack with record 0 (busy bit 1): animation 20, as EntityAi_MeleeAttack, but only part
   0 is checked, by ParaStool_CheckPartHitInPlayerView. */
static void ParaStool_MeleeAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        ENTITY_FLAG_BITS(entity)->melee_0_hit = FALSE;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = FALSE;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = ParaStool_CheckPartHitInPlayerView(entity, unk240, 0, elapsed, entity->model->level_tables->frame, 0.0f);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Helper of ParaStool_MeleeAttack0: the hit check of EntityAi_CheckMeleeHitAt and EntityAi_SendMeleeHit for one
   part of the record on the part's frames, written out, with conditions of its own: the entity must
   be within EntityParams.yaw_limit_deg degrees of the player's facing and action_frames degrees of the player's
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
        yaw = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));
        pitch = (f32)atan((entity->model->position[1] - player_state.position.y + 220.0f) / entity->distance_to_player) + player_state.rotation.x;
        if (ABS(yaw) < DEG_TO_RAD(entity->params->yaw_limit_deg) && ABS(pitch) < (s16)entity->params->action_frames * DEG_TO_RAD(1)) {
            spread = EntityMath_WrapAngle(DEG_TO_RAD(record->hit_parts[0].half_angle_deg));
            if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < spread) {
                Attack_RollPlayerAilments(&entity->params->melee_attacks[0].hit_parts[0].ailment_chances);
                memset(&request, 0, sizeof(request));
                memset(&extra, 0, sizeof(extra));
                Vector_Copy(request.position, entity->model->position);
                request.height = entity->height + 10.0f;
                request.radius = entity->radius + record->hit_parts[index].radius_bonus;
                request.facing = EntityMath_WrapAngle(angle + PI);
                request.half_angle = EntityMath_WrapAngle(DEG_TO_RAD(record->hit_parts[index].half_angle_deg));
                request.owner = entity;
                request.shape = 3;
                request.target_mask = 1;
                request.strengths = record->hit_parts[index].strengths;
                request.ailment_chances = record->hit_parts[index].ailment_chances;
                request.hit_armor_part = record->wear_armour_part;
                extra.is_magic_shot = 0;
                extra.strength_gauge = 0;
                extra.player_power = 0;
                extra.player_magic = 0;
                request.knockback = record->hit_parts[index].hit_flags & 1;
                return Attack_Run(&request, &extra);
            }
        }
    }
    return 0;
}
