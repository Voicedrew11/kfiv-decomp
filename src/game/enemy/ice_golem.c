/* IceGolem: the behaviour of entity type 47 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Ice Golem. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include "player/player.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"

/* ai_state->move_choice: runs at the player when it is far or behind */
#define ICE_GOLEM_MOVE_CHOICE_RUN_IN 1

#define ENTITY_WORK_AREA_SIZE 0xA0

static void IceGolem_Think(Entity *entity, s32 run_now);
static void IceGolem_ThinkCombat(Entity *entity, s32 run_now);
static void IceGolem_Dormant(Entity *entity);
static void IceGolem_RangedAttack0(Entity *entity);

/* Set-up of a type 47 entity: clears its work area and installs the type's handlers. */
void IceGolem_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *choice_weights = entity->choice_weights;
    u8 *attack_weights = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: the three fills name their target inside the loop; choice_weights and attack_weights above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = ENTITY_WORK_BYTES(entity);

        work_area[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = IceGolem_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = IceGolem_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    if (ENTITY_FLAG_WORD(entity->spawn_info->spawn_flags) & (0x8000L << 43)) {
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

/* The think handler: as EntityAi_Think, with IceGolem_ThinkCombat for state 2. Every third
   frame it starts effect 0x3AD at point 0xFF of the model (EntityEvent_StartEffectAtPoint). */
static void IceGolem_Think(Entity *entity, s32 run_now) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            case ENTITY_STATE_SEARCHING:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                IceGolem_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
    if (frame_counter % 3U == 0) {
        EntityEvent_StartEffectAtPoint(entity, 0x3AD, 0xFF, entity->model->rotation);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat with IceGolem_RangedAttack0 as the
   ranged attack; when it starts to close in from more than 500 away, or with the target more than
   90 degrees off its heading, it does so with command bits 0x40 and 0x400 added (work byte unk78 is
   1). */
static void IceGolem_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, IceGolem_RangedAttack0, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 0x78;
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (500.0f < entity->distance_to_player || ABS(angle) > HALF_PI) {
                ai_state->move_choice = ICE_GOLEM_MOVE_CHOICE_RUN_IN;
            } else {
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            }
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 0x1E;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (entity->combat_choice == COMBAT_CHOICE_CLOSE_IN) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->turn_in_place == 0) {
                ai_state->turn_in_place = ABS(angle) > DEG_TO_RAD(30);
            }
            if (ai_state->turn_in_place != 0) {
                entity->move_command = EntityAi_TurnCommand(entity);
                if (entity->move_command == 0) {
                    ai_state->turn_in_place = 0;
                }
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            switch (ai_state->move_choice) {
            case MOVE_CHOICE_CLOSE_IN:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case ICE_GOLEM_MOVE_CHOICE_RUN_IN:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                entity->move_command |= MOVE_COMMAND_TURN_FAST;
                break;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, but it also sets flag bit 20 and clears
   bit 17 at the start, and from frame 41 of animation 2 on clears bit 20 and sets bit 17. */
static void IceGolem_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->draw_afterimage = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case DORMANT_STEP_SLEEP:
        if (entity->distance_to_player < (f32)entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
            if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            } else {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
            }
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_WAKE:
        if (entity->model->level_tables->frame >= 0x29) {
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->draw_afterimage = 1;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
            ENTITY_FLAG_BITS(entity)->draw_afterimage = 1;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23, the head following the target. On
   each frame from EntityParams.action_frames up to unk436, in steps of unk438, it launches the record's
   shot unk10 from the point Model_GetSkinnedBonePoint gives for the record's unk11, towards the next point,
   with a pitch of -5 degrees (EntityAi_LaunchShot). */
static void IceGolem_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8Array *unk3D8 = entity->params->ranged_attacks;
    f32 direction[4];
    f32 *base;
    f32 *tip;
    f32 delta_x;
    f32 delta_z;
    s32 elapsed;
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->look_at_target = 1;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->record.turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->record.turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        for (frame = (s16)entity->params->action_frames; frame < entity->params->near_distance; frame += entity->params->far_distance) {
            if ((entity->ailment_4_frames == 0 || unk3D8->ignores_ailment_4 != 0) && elapsed < frame && entity->model->level_tables->frame >= frame) {
                base = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), unk3D8->bone_index);
                tip = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), unk3D8->bone_index + 1);
                if (base != NULL && tip != NULL) {
                    delta_x = tip[0] - base[0];
                    delta_z = tip[2] - base[2];
                    direction[0] = -DEG_TO_RAD(5);
                    direction[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                    direction[2] = direction[3] = 0.0f;
                    EntityAi_LaunchShot(entity, unk3D8->magic_effect, base, direction);
                }
            }
        }
        if (entity->model->level_tables->frame > 0 && unk3D8->record.attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->record.attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}
