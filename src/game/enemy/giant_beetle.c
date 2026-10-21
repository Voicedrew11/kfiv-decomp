/* GiantBeetle: the behaviour of entity type 76 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Giant Beetle. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include "input/input.h"
#include "player/player.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"

static void GiantBeetle_Animate(Entity *entity);
static void GiantBeetle_OnHit(Entity *entity);
static void GiantBeetle_Think(Entity *entity, s32 run_now);
static void GiantBeetle_ThinkWander(Entity *entity, s32 run_now);
static void GiantBeetle_ThinkCombat(Entity *entity, s32 run_now);
static void GiantBeetle_TurnParts7And12(Entity *entity);
static void GiantBeetle_TurnParts23To32(Entity *entity);
static void GiantBeetle_MeleeAttack0(Entity *entity);
static void GiantBeetle_ShotAttackShakingCamera(Entity *entity);
static void GiantBeetle_Death(Entity *entity);

/* Set-up of a type 76 entity: clears its work area and installs the type's handlers. */
void GiantBeetle_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = GiantBeetle_Think;
    ai_state->animate = GiantBeetle_Animate;
    ai_state->on_hit = GiantBeetle_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = GiantBeetle_Death;
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
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn; outside the dormant action GiantBeetle_TurnParts7And12 and
   GiantBeetle_TurnParts23To32 turn model parts instead. */
static void GiantBeetle_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        if (!(entity->busy_flags & ENTITY_BUSY_DORMANT)) {
            GiantBeetle_TurnParts7And12(entity);
            GiantBeetle_TurnParts23To32(entity);
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void GiantBeetle_OnHit(Entity *entity) {

    EntityAi_LowerWeightsAfterHit(entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
}

/* The think handler: as EntityAi_Think, with GiantBeetle_ThinkCombat for state 2 and GiantBeetle_ThinkWander
   for the other states. */
static void GiantBeetle_Think(Entity *entity, s32 run_now) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
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
    case ENTITY_STATE_SEARCHING:
        break;
    case ENTITY_STATE_NOTICED:
        GiantBeetle_ThinkCombat(entity, run_now);
        return;
    }
    GiantBeetle_ThinkWander(entity, run_now);
}

/* The think function of every state but 2 (GiantBeetle_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 79 times in 100, for 120 to
   179 frames) or no target (60 to 89 frames), takes the command of EntityAi_MoveCommand towards it (or the
   turn away from a push, EntityAi_TurnFromPush) and starts the move action. */
static void GiantBeetle_ThinkWander(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 *point;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (entity->action_timer == 0) {
        if (rand() % 100 > 20) {
            point = ai_state->target_point;
            EntityPoint_PickNear(entity->spawn_info->wander_radius, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->action_timer = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->action_timer = rand() % 30 + 60;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (turn < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but attack index 0 has the type's own
   actions (GiantBeetle_ShotAttackShakingCamera for the ranged one, GiantBeetle_MeleeAttack0 for the melee one); closing
   in it takes the command of EntityAi_MoveCommand (range: the radius plus 101) with no turn away from a
   push, and it leaves flag bits 9 and 10 as they are. */
static void GiantBeetle_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;
    s32 side;

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
                if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                    EntityAi_StartAction(entity, GiantBeetle_ShotAttackShakingCamera, run_now);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
                }
            } else if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                EntityAi_StartAction(entity, GiantBeetle_MeleeAttack0, run_now);
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
            entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 1.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* Helper of GiantBeetle_Animate: eases ai_state.part_angle_x towards 20 degrees on the side the command
   turns to (0 with no turn bit) and puts half of it in angle part_angles[1] of model parts 7 and 12. */
static void GiantBeetle_TurnParts7And12(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *part;
    f32 angle;
    f32 limit;
    s32 i;

    limit = DEG_TO_RAD(20);
    if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        angle = limit;
    } else if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        angle = -limit;
    } else {
        angle = 0.0f;
    }
    limit = DEG_TO_RAD(20);
    if (angle > limit) {
        angle = limit;
    } else if (angle < -limit) {
        angle = -limit;
    }
    EntityMath_EaseAngleToward(&ai_state->part_angle_x, angle, 10.0f);
    part = (EntityModelPart80 *)entity->model->parts;
    for (i = 0; i < ENTITY_MODEL(entity)->part_count; i++) {
        if (i == 7 || i == 12) {
            part->part_angles[1] = ai_state->part_angle_x * 0.5f;
        }
        part++;
    }
}

/* Helper of GiantBeetle_Animate: eases ai_state.part_angle_y towards 20 degrees on the side the command
   turns to (0 with no turn bit) and puts minus half of it in angle rotation_y of model parts 23, 28, 31
   and 32 (the name gives the first and the last of the four). */
static void GiantBeetle_TurnParts23To32(Entity *entity) {
    f32 limit;
    f32 turn;
    EntityAiState *ai_state;
    ModelInstance *model;
    ModelPart *part;
    u16 command;
    u16 count;
    s32 i;

    limit = DEG_TO_RAD(20);
    command = entity->move_command;
    ai_state = &entity->ai_state;
    if (command & MOVE_COMMAND_TURN_RIGHT) {
        turn = limit;
    } else {
        turn = (command & MOVE_COMMAND_TURN_LEFT) ? -limit : 0.0f;
    }
    limit = DEG_TO_RAD(20);
    if (limit < turn || (limit = -limit, turn < limit)) {
        turn = limit;
    }
    EntityMath_EaseAngleToward(&ai_state->part_angle_y, turn, 20.0f);
    model = ENTITY_MODEL(entity);
    count = model->part_count;
    part = model->parts;
    for (i = 0; i < count; i++) {
        if (i == 0x17 || i == 0x1C || i == 0x1F || i == 0x20) {
            part->rotation_y = -ai_state->part_angle_y * 0.5f;
        }
        part++;
    }
}

/* The melee attack with record 0 (busy bit 1): animation 20; it checks part 0 only, with the frames
   1 to 2 whatever the animation's frame (EntityAi_CheckMeleeHit), and ends when flag bit 45 or 49 is set,
   not when the motion ends. */
static void GiantBeetle_MeleeAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[0];

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
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, 1, 2);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (entity->flags & (ENTITY_FLAG_WALL_PUSH | ENTITY_FLAG_PLAYER_PUSH)) {
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

/* The ranged attack with record 0 (busy bit 1): animation 23, launching the record's shots as
   EntityAi_RangedAttack does, and also checking the hits of melee record 1; at frame 19 it shakes the camera
   (EntityAi_ShakeCamera, reach 10 for 26 frames) and calls Input_StartRumble(0x80, 13, 0). */
static void GiantBeetle_ShotAttackShakingCamera(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[1];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        EntityAi_LaunchShots(entity, unk3D8, elapsed, entity->model->level_tables->frame);
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_1_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_2_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, entity->model->level_tables->frame);
        }
        if (entity->model->level_tables->frame == 0x13) {
            EntityAi_ShakeCamera(10, 0x1A);
            Input_StartRumble(0x80, 0xD, 0);
        }
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death without that one's setting of Entity.collision_radius;
   flag bits 26 and 27 are set when the fade starts, not at the start. */
static void GiantBeetle_Death(Entity *entity) {
    EntityParams *params;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x10, 0, 5, 1);
        params = entity->params;
        if (params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->action_timer = 20;
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case DEATH_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = 0;
        }
        break;
    }
}
