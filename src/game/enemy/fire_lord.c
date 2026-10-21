/* FireLord: the behaviour of entity type 24 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Fire Lord. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/effect.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "gfx/draw_list.h"
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_math.h"
#include "model/motion.h"
#include "player/player.h"

/* EntityAiStateKind131390.distance_band: the distance to the player, far to near */
#define FIRE_LORD_BAND_APPROACH                0   /* far: walks in towards the player */
#define FIRE_LORD_BAND_SHOOT_OR_APPROACH       1   /* shoots, or approaches */
#define FIRE_LORD_BAND_SIDE_ANIM_OR_APPROACH   2   /* side animation, or approaches */
#define FIRE_LORD_BAND_CLOSE_IN                3   /* closes in on the player */
#define FIRE_LORD_BAND_BACK_AWAY               4   /* backs away from the player */

/* Entity.action_step values of FireLord_MeleeAttack0 past the shared ones */
#define FIRE_LORD_MELEE_STEP_WINDUP            2   /* turns towards the player until frame 8, then pauses */
#define FIRE_LORD_MELEE_STEP_LUNGE             3   /* steps forward until close or the pause runs out */
#define FIRE_LORD_MELEE_STEP_SWING             4   /* swings the blade (bones 8-9), ends when the motion ends */

static void FireLord_Animate(Entity *entity);
static void FireLord_Think(Entity *entity, s32 run_now);
static void FireLord_ThinkIdle(Entity *entity, s32 run_now);
static void FireLord_ThinkCombat(Entity *entity, s32 run_now);
static void FireLord_ThinkCombatApproach(Entity *entity, int invoke_now);
static void FireLord_ThinkCombatShootOrApproach(Entity *entity, s32 run_now);
static void FireLord_ThinkCombatSideAnimOrApproach(Entity *entity, s32 run_now);
static void FireLord_ThinkCombatCloseIn(Entity *entity, s32 run_now);
static void FireLord_ThinkCombatBackAway(Entity *entity, s32 run_now);
static void FireLord_PlayAnim6Or7Alternately(Entity *entity);
static void FireLord_MeleeAttack0(Entity *entity);
static void FireLord_MeleeAttack1(Entity *entity);
static void FireLord_PlayAnim6Or7ThenMelee0(Entity *entity);
static void FireLord_Move(Entity *entity);
static s32 FireLord_ShouldEvadeSwingOrMagic(Entity *entity, f32 max_distance, f32 max_angle);

/* Set-up of a type 24 entity: clears its work area and installs the type's handlers. */
void FireLord_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = FireLord_Think;
    ai_state->animate = FireLord_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = FireLord_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
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
    fire_lord_older_afterimage = *(ModelInstanceBlock *)entity->model;
    fire_lord_model[0] = *(ModelInstanceBlock *)entity->model;
}

/* The animate handler: as EntityAi_Animate, without that one's test of the EntityParams kind bits
   (0x6000000). */
static void FireLord_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->look_at_target;
        limits[1] = DEG_TO_RAD(entity->params->pitch_limit_deg);
        limits[0] = DEG_TO_RAD(-entity->params->pitch_limit_deg);
        limits[3] = DEG_TO_RAD(entity->params->yaw_limit_deg);
        limits[2] = DEG_TO_RAD(-entity->params->yaw_limit_deg);
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* The think handler: as EntityAi_Think, with FireLord_ThinkIdle for state 0 and
   FireLord_ThinkCombat for states 1 and 2. Before that it keeps the model's last two frames in
   fire_lord_model, each at half the alpha of the one before, clears work byte unk7E, and counts down
   work word unk6A, which it sets to EntityParams.action_frames while player_state.cast_launched_shot is not 0. Counts
   down work byte unk7D. */
static void FireLord_Think(Entity *entity, s32 run_now) {
    EntityAiStateKind131390 *ai_state = (EntityAiStateKind131390 *)&entity->ai_state;
    ModelInstanceBlock *block;
    ModelInstanceBlock *previous;

    block = &fire_lord_model[1];
    previous = block - 1;
    *block = *previous;
    fire_lord_model[0] = *(ModelInstanceBlock *)entity->model;
    fire_lord_model[0].model.alpha *= 0.5f;
    fire_lord_model[1].model.alpha *= 0.5f;
    ai_state->afterimage_active = 0;
    if (ai_state->player_cast_timer > 0) {
        ai_state->player_cast_timer--;
    }
    if (player_state.cast_launched_shot != 0) {
        ai_state->player_cast_timer = entity->params->action_frames;
    }
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
            case ENTITY_STATE_SEARCHING:
            case ENTITY_STATE_NOTICED:
                FireLord_ThinkCombat(entity, run_now);
                break;
            case ENTITY_STATE_UNAWARE:
            default:
                FireLord_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
    if (ai_state->distance_band_timer > 0) {
        ai_state->distance_band_timer--;
    }
}

/* The think function of state 0: no command, flag bit 9 clear; starts the move action. */
static void FireLord_ThinkIdle(Entity *entity, s32 run_now) {
    entity->move_command = 0;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    EntityAi_StartAction(entity, entity->ai_state.move, run_now);
}

/* The think function of states 1 and 2: every 10 frames (work byte unk7D) sorts the distance to the
   player, less the radius and 100, into a band kept in work byte unk7C (0: above 1200, 1: above
   800, 2: above 500, 3: above 100, 4: nearer), and calls the function of the band:
   FireLord_ThinkCombatApproach, FireLord_ThinkCombatShootOrApproach,
   FireLord_ThinkCombatSideAnimOrApproach, FireLord_ThinkCombatCloseIn or
   FireLord_ThinkCombatBackAway. */
static void FireLord_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiStateKind131390 *ai_state = (EntityAiStateKind131390 *)&entity->ai_state;
    f32 distance;

    if (ai_state->distance_band_timer <= 0) {
        distance = entity->distance_to_player - entity->radius - 100.0f;
        if (distance > 1200.0f) {
            ai_state->distance_band = FIRE_LORD_BAND_APPROACH;
            ai_state->distance_band_timer = 10;
        } else if (distance <= 1200.0f && distance > 800.0f) {
            ai_state->distance_band = FIRE_LORD_BAND_SHOOT_OR_APPROACH;
            ai_state->distance_band_timer = 10;
        } else if (distance <= 800.0f && distance > 500.0f) {
            ai_state->distance_band = FIRE_LORD_BAND_SIDE_ANIM_OR_APPROACH;
            ai_state->distance_band_timer = 10;
        } else if (distance <= 500.0f && distance > 100.0f) {
            ai_state->distance_band = FIRE_LORD_BAND_CLOSE_IN;
            ai_state->distance_band_timer = 10;
        } else {
            ai_state->distance_band = FIRE_LORD_BAND_BACK_AWAY;
            ai_state->distance_band_timer = 10;
        }
    }
    switch (ai_state->distance_band) {
    case FIRE_LORD_BAND_APPROACH:
        FireLord_ThinkCombatApproach(entity, run_now);
        break;
    case FIRE_LORD_BAND_SHOOT_OR_APPROACH:
        FireLord_ThinkCombatShootOrApproach(entity, run_now);
        break;
    case FIRE_LORD_BAND_SIDE_ANIM_OR_APPROACH:
        FireLord_ThinkCombatSideAnimOrApproach(entity, run_now);
        break;
    case FIRE_LORD_BAND_CLOSE_IN:
        FireLord_ThinkCombatCloseIn(entity, run_now);
        break;
    case FIRE_LORD_BAND_BACK_AWAY:
        FireLord_ThinkCombatBackAway(entity, run_now);
        break;
    default:
        FireLord_ThinkCombatApproach(entity, run_now);
        break;
    }
}

/* Band 0 of FireLord_ThinkCombat (and its default): goes for the player, with the command of
   EntityAi_MoveCommand (range 50, far marked). */
static void FireLord_ThinkCombatApproach(Entity *entity, int invoke_now) {
    EntityAiState *sub = &entity->ai_state;

    entity->target = &player_state;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f);
    EntityAi_StartAction(entity, sub->move, invoke_now);
}

/* Band 1 of FireLord_ThinkCombat: when EntityAi_CanShoot finds the player in reach of a ranged
   attack, starts EntityAi_RangedAttack with the record EntityAi_PickRangedAttack draws; else goes for the
   player as band 0 does. */
static void FireLord_ThinkCombatShootOrApproach(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (EntityAi_CanShoot(entity)) {
        EntityAi_PickRangedAttack(entity);
        EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    entity->target = &player_state;
    entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f);
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* Band 2 of FireLord_ThinkCombat: when the heading is within 20 degrees of Entity.heading_to_player, flips
   work byte unk7B and starts FireLord_PlayAnim6Or7Alternately; else goes for the player as band 0 does. */
static void FireLord_ThinkCombatSideAnimOrApproach(Entity *entity, s32 run_now) {
    EntityAiStateKind131390 *ai_state = (EntityAiStateKind131390 *)&entity->ai_state;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (!(ABS(angle) > DEG_TO_RAD(20))) {
        ai_state->alternate_anim_flag = ai_state->alternate_anim_flag == FALSE;
        EntityAi_StartAction(entity, FireLord_PlayAnim6Or7Alternately, run_now);
        return;
    }
    entity->target = &player_state;
    entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f);
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* Band 3 of FireLord_ThinkCombat (the player 100 to 500 away): as EntityAi_ThinkCombat, closing
   in on the player with the forward command; but in place of the guard it starts
   FireLord_PlayAnim6Or7ThenMelee0 when FireLord_ShouldEvadeSwingOrMagic says so, and melee records 0 and 1 have
   the type's own actions (FireLord_MeleeAttack0, FireLord_MeleeAttack1). */
static void FireLord_ThinkCombatCloseIn(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (FireLord_ShouldEvadeSwingOrMagic(entity, 600.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, FireLord_PlayAnim6Or7ThenMelee0, 1);
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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack0, run_now);
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack1, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
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
    if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
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
            if (turn > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* Band 4 of FireLord_ThinkCombat (the player nearer than 100): as
   FireLord_ThinkCombatCloseIn, but where that one closes in this one gives command bit 2
   (backwards), with a turn bit when the target is off the heading by more than the turn limit. */
static void FireLord_ThinkCombatBackAway(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (FireLord_ShouldEvadeSwingOrMagic(entity, 600.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, FireLord_PlayAnim6Or7ThenMelee0, 1);
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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack0, run_now);
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack1, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
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
    if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
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
            if (turn > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                direction[0] = EntityMath_HeadingTo(entity->model->position, entity->target);
                angle = EntityMath_AngleDifference(entity->model->rotation[1], direction[0]);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                    } else {
                        entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                    }
                }
                entity->move_command |= MOVE_COMMAND_BACK;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* An action (busy bit 0x1000) that FireLord_ThinkCombatSideAnimOrApproach starts: plays animation 6 or 7
   (work byte unk7B picks) to its end, turning to Entity.heading_to_player. Sets work byte unk7E. */
static void FireLord_PlayAnim6Or7Alternately(Entity *entity) {
    EntityAiStateKind131390 *ai_state = (EntityAiStateKind131390 *)&entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    ai_state->afterimage_active = 1;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
    switch (entity->action_step) {
    case ACTION_STEP_START:
        if (ai_state->alternate_anim_flag != FALSE) {
            Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
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

/* The melee attack with record 0 (busy bit 1): animation 20 to frame 8; then, the motion stopped,
   it moves 30 a frame along the heading for up to 15 frames or until the player is within 200; then
   plays on with the hit checks of EntityAi_MeleeAttack. On frames 20 to 26 it starts effect 0x3A0
   along the path of the point Model_GetSkinnedBonePoint gives for 8, pointing to the one it gives for 9. */
static void FireLord_MeleeAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];
    f32 position[4];
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case FIRE_LORD_MELEE_STEP_WINDUP:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (entity->model->level_tables->frame >= 8) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ai_state->pause_timer = 15;
            entity->action_step++;
        }
        break;
    case FIRE_LORD_MELEE_STEP_LUNGE:
        EntityMath_FacingVector((Vector4 *)position, entity->model->rotation[1]);
        Vector_Scale(position, position, 30.0f);
        entity->move_delta_x += position[0];
        entity->move_delta_y += position[1];
        entity->move_delta_z += position[2];
        ai_state->pause_timer--;
        if (entity->distance_to_player - entity->radius - 100.0f < 200.0f || ai_state->pause_timer < 0) {
            entity->action_step++;
        }
        break;
    case FIRE_LORD_MELEE_STEP_SWING:
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 8);
        tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 9);
        if (from != NULL && tip != NULL) {
            if (entity->model->level_tables->frame >= 0x14 && entity->model->level_tables->frame <= 0x1A) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                angles[2] = angles[3] = 0.0f;
                delta_x = from[0] - ai_state->part_angle_x;
                delta_y = from[1] - ai_state->part_angle_y;
                delta_z = from[2] - ai_state->part_angle_z;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = ai_state->part_angle_x + delta_x * i / count;
                    position[1] = ai_state->part_angle_y + delta_y * i / count;
                    position[2] = ai_state->part_angle_z + delta_z * i / count;
                    Effect_StartSpawner(0x3A0, position, angles, entity);
                }
            }
            ai_state->part_angle_x = from[0];
            ai_state->part_angle_y = from[1];
            ai_state->part_angle_z = from[2];
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_1_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_2_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, entity->model->level_tables->frame);
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

/* The melee attack with record 1 (busy bit 1): animation 21, as EntityAi_MeleeAttack. On frames 23
   to 27 it starts effect 0x3A0 along the path of the point Model_GetSkinnedBonePoint gives for 16, pointing to
   the one it gives for 17. */
static void FireLord_MeleeAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[1];
    f32 position[4];
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x15, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 0x10);
        tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 0x11);
        if (from != NULL && tip != NULL) {
            if (entity->model->level_tables->frame >= 0x17 && entity->model->level_tables->frame <= 0x1B) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                angles[2] = angles[3] = 0.0f;
                delta_x = from[0] - ai_state->part_angle_x;
                delta_y = from[1] - ai_state->part_angle_y;
                delta_z = from[2] - ai_state->part_angle_z;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = ai_state->part_angle_x + delta_x * i / count;
                    position[1] = ai_state->part_angle_y + delta_y * i / count;
                    position[2] = ai_state->part_angle_z + delta_z * i / count;
                    Effect_StartSpawner(0x3A0, position, angles, entity);
                }
            }
            ai_state->part_angle_x = from[0];
            ai_state->part_angle_y = from[1];
            ai_state->part_angle_z = from[2];
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_1_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_2_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, entity->model->level_tables->frame);
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

/* A second action with animations 6 and 7 (busy bit 0x1000), which bands 3 and 4 start in place of
   the guard: plays 6 or 7, by the side of the player's facing the entity is on, to its end, turning
   to Entity.heading_to_player; then starts FireLord_MeleeAttack0 when EntityAi_CanUseMelee finds the player in
   reach of record 0. Sets work byte unk7E. */
static void FireLord_PlayAnim6Or7ThenMelee0(Entity *entity) {
    EntityAiStateKind131390 *ai_state = (EntityAiStateKind131390 *)&entity->ai_state;
    PlayerState *player;

    entity->busy_flags |= ENTITY_BUSY_1000;
    ai_state->afterimage_active = 1;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
    switch (entity->action_step) {
    case ACTION_STEP_START:
        EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        player = &player_state;
        if (EntityMath_AngleDifference(player->rotation.y, EntityMath_AngleXZ(&player->position.x, entity->model->position)) >= 0.0f) {
            Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (EntityAi_CanUseMelee(entity, 0)) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack0, 1);
            } else if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The move handler: turns as the command says (twice as fast with command bit 0x40 or 0x400) and
   picks the animation as EntityAi_Move does, but adds no movement of its own. */
static void FireLord_Move(Entity *entity) {
    EntityAiStateKind168160 *ai_state = (EntityAiStateKind168160 *)&entity->ai_state;
    EntityParams *params;
    f32 turn;

    if (entity == player_state.stepped_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
    params = entity->params;
    if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= params->turn_step_per_frame;
        if (ai_state->turn_rate < -params->turn_limit) {
            ai_state->turn_rate = -params->turn_limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += params->turn_step_per_frame;
        if (params->turn_limit < ai_state->turn_rate) {
            ai_state->turn_rate = params->turn_limit;
        }
    } else {
        ai_state->turn_rate = 0.0f;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & (MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_FAST)) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, params->motion_blend_frames, 0);
        } else {
            Motion_Start(entity, 3, 0, params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_DOWN) {
        Motion_Start(entity, 8, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        Motion_Start(entity, 9, 0, params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, params->motion_blend_frames >> 1, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, params->motion_blend_frames >> 1, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, params->motion_blend_frames, 0);
    }
}

/* Whether to start FireLord_PlayAnim6Or7ThenMelee0: while the player is early in a swing or work word unk6A
   runs (FireLord_Think sets it when he casts a magic), the entity is not busy, the player is within
   max_distance (less the radius and 100) and Entity.heading_to_player within max_angle of the heading; then
   EntityParams.dodge_chance / 5 times in 100. */
static s32 FireLord_ShouldEvadeSwingOrMagic(Entity *entity, f32 max_distance, f32 max_angle) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 distance;
    f32 angle;

    if (player_weapon_step[0] == 0 || held_item_model_ptr->level_tables->frame >= 6) {
        if (ai_state->sub_timer <= 0) {
            return FALSE;
        }
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (entity->busy_flags != 0) {
        return FALSE;
    }
    if (max_distance < distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    return rand() % 100 < entity->params->dodge_chance / 5;
}

/* Puts the two older copies of the model that FireLord_Think keeps in fire_lord_model (each at half the alpha
   of the one before) in draw list `list`, while work byte unk7E is set (an action with animation 6 or 7 ran
   this frame) and the entity is not dying. Entity_DrawAll calls it for spawn type 0xF. */
void FireLord_DrawAfterimages(Entity *entity, s32 list) {
    EntityAiStateKind131390 *ai_state = (EntityAiStateKind131390 *)&entity->ai_state;

    if (ai_state->afterimage_active != 0 && ENTITY_FLAG_BITS(entity)->dying == 0) {
        fire_lord_model[0].model.fog_block = ENTITY_MODEL(entity)->fog_block;
        fire_lord_model[0].model.light_block = ENTITY_MODEL(entity)->light_block;
        fire_lord_model[1].model.fog_block = ENTITY_MODEL(entity)->fog_block;
        fire_lord_model[1].model.light_block = ENTITY_MODEL(entity)->light_block;
        DrawList_Add(&fire_lord_model[0].model, 0, list);
        DrawList_Add(&fire_lord_model[1].model, 0, list);
    }
}
