/* StoneGuardian: the behaviour of entity type 23 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Stone Guardian. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "map_object/map_object.h"
#include "entity/entity_world.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "math/vu0.h"
#include "entity/entity_math.h"
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"
#include "player/player.h"

/* Entity.spawn_info->spawn_mode: picks the think handler (set-up function) of the spawn */
#define STONE_GUARDIAN_SPAWN_MODE_HOLD_ANIM      1
#define STONE_GUARDIAN_SPAWN_MODE_ZONE_CENTRE    2
#define STONE_GUARDIAN_SPAWN_MODE_ZONE_NORTH     3
#define STONE_GUARDIAN_SPAWN_MODE_ZONE_SOUTH     4
#define STONE_GUARDIAN_SPAWN_MODE_ZONE_FAR_WEST  5
#define STONE_GUARDIAN_SPAWN_MODE_HIDE_SEQUENCE  6

/* Entity.state in the mode think handlers */
#define STONE_GUARDIAN_STATE_HOLD              0 /* stands at its post in animation 1 */
#define STONE_GUARDIAN_STATE_WALK_TO_POST      1 /* the player is outside the zone: walks back to its post */
#define STONE_GUARDIAN_STATE_CHASE             2 /* the player is in the zone: chases in bounds */
#define STONE_GUARDIAN_STATE_RETURN_TO_SPAWN   3 /* the map object is open: returns to its spawn point */

// Local: callers pass Entity/PlayerState positions, so these take void *; entity_math.h says float *.

static void StoneGuardian_Animate(Entity *entity);
static void StoneGuardian_OnHit(Entity *entity);
static void StoneGuardian_Move(Entity *entity);
static void StoneGuardian_TurnToSpawnHeading(Entity *entity);
static void StoneGuardian_HoldAnim1(Entity *entity);
static void StoneGuardian_ThinkHoldAnim17(Entity *entity, s32 run_now);
static void StoneGuardian_ThinkMode2StayInBounds(Entity *entity, s32 run_now);
static void StoneGuardian_Mode2WalkToPost(Entity *entity);
static void StoneGuardian_Mode2ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode2ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode3StayInBounds(Entity *entity, s32 run_now);
static void StoneGuardian_Mode3WalkToPost(Entity *entity);
static void StoneGuardian_Mode3ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode3ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode4StayInBounds(Entity *entity, s32 run_now);
static void StoneGuardian_Mode4WalkToPost(Entity *entity);
static void StoneGuardian_Mode4ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode4ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode5StayInBounds(Entity *entity, s32 run_now);
static void StoneGuardian_Mode5WalkToPost(Entity *entity);
static void StoneGuardian_Mode5ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode5ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode6HideSequence(Entity *entity, s32 run_now);
static void StoneGuardian_Mode6StayHidden(Entity *entity, s32 run_now);
static void StoneGuardian_Mode6WaitTurnHidden(Entity *entity, s32 run_now);
static void StoneGuardian_Mode6LeaveHiding(Entity *entity, s32 run_now);
static void StoneGuardian_PlayAnim9(Entity *entity);
static void StoneGuardian_Mode6WaitTurnShown(Entity *entity, s32 run_now);
static void StoneGuardian_Mode6ReturnToHiding(Entity *entity, s32 run_now);
static void StoneGuardian_PlayAnim8(Entity *entity);
static void StoneGuardian_Mode6WalkToHidePoint(Entity *entity, s32 run_now);
static s32 StoneGuardian_Mode6IsStepFree(Entity *entity, s32 value);
static s32 StoneGuardian_Mode6IsNearestWaiting(Entity *entity, f32 limit);

/* Set-up of a type 23 entity: clears its work area and installs the type's handlers. */
void StoneGuardian_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->animate = StoneGuardian_Animate;
    ai_state->on_hit = StoneGuardian_OnHit;
    ai_state->move = StoneGuardian_Move;
    ai_state->death = EntityAi_Death;
    ai_state->dormant = NULL;
    if (entity->spawn_info->spawn_mode == STONE_GUARDIAN_SPAWN_MODE_HOLD_ANIM) {
        ai_state->move = NULL;
        ai_state->think = StoneGuardian_ThinkHoldAnim17;
    } else if (entity->spawn_info->spawn_mode == STONE_GUARDIAN_SPAWN_MODE_ZONE_CENTRE) {
        ai_state->think = StoneGuardian_ThinkMode2StayInBounds;
    } else if (entity->spawn_info->spawn_mode == STONE_GUARDIAN_SPAWN_MODE_ZONE_NORTH) {
        ai_state->think = StoneGuardian_ThinkMode3StayInBounds;
    } else if (entity->spawn_info->spawn_mode == STONE_GUARDIAN_SPAWN_MODE_ZONE_SOUTH) {
        ai_state->think = StoneGuardian_ThinkMode4StayInBounds;
    } else if (entity->spawn_info->spawn_mode == STONE_GUARDIAN_SPAWN_MODE_ZONE_FAR_WEST) {
        ai_state->think = StoneGuardian_ThinkMode5StayInBounds;
    } else if (entity->spawn_info->spawn_mode == STONE_GUARDIAN_SPAWN_MODE_HIDE_SEQUENCE) {
        ai_state->think = StoneGuardian_ThinkMode6HideSequence;
        ai_state->move = EntityAi_Move;
        entity->model->position[0] = -16200.0f;
        entity->model->position[1] = 3030.0f;
        entity->model->position[2] = 4900.0f;
        entity->model_alpha = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ai_state->step_index = 1;
    } else {
        ai_state->think = EntityAi_Think;
        ai_state->move = EntityAi_Move;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    EntityAi_StartAction(entity, ai_state->move, 0);
    ai_state->pose_blend_weight = 0.5f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
    ai_state->part_angle_x = entity->spawn_info->spawn_x;
    ai_state->part_angle_y = entity->spawn_info->spawn_y;
    ai_state->part_angle_z = entity->spawn_info->spawn_z;
}

/* The animate handler: as EntityAi_Animate without the head turn. */
static void StoneGuardian_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void StoneGuardian_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
}

/* The move handler: turns only while the command also walks or runs; picks animation 3 or 4
   walking, 11 or 12 for a turn on the spot, else 0; then clears the busy bits and the command. */
static void StoneGuardian_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 turn = 0.0f;

    if (entity->move_command & (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN)) {
        if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
            ai_state->turn_rate -= entity->params->turn_step_per_frame;
            if (ai_state->turn_rate < -entity->params->turn_limit) {
                ai_state->turn_rate = -entity->params->turn_limit;
            }
        } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
            ai_state->turn_rate += entity->params->turn_step_per_frame;
            if (ai_state->turn_rate > entity->params->turn_limit) {
                ai_state->turn_rate = entity->params->turn_limit;
            }
        } else {
            ai_state->turn_rate = turn;
        }
    } else {
        ai_state->turn_rate = turn;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->animation_index != ENTITY_MOTION_WALK && entity->animation_index != ENTITY_MOTION_RUN) {
            if (entity->move_command & MOVE_COMMAND_RUN) {
                Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 1);
            } else {
                Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 1);
            }
        } else if (entity->move_command & MOVE_COMMAND_RUN) {
            entity->animation_index = ENTITY_MOTION_RUN;
            entity->model->level_tables->frame_step = entity->params->motion_speeds[entity->animation_index];
        } else {
            entity->animation_index = ENTITY_MOTION_WALK;
            entity->model->level_tables->frame_step = entity->params->motion_speeds[entity->animation_index];
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
    entity->busy_flags = 0;
    entity->move_command = 0;
    entity->action_timer = 0;
}

/* An action (busy bit 0x1000): turns on the spot (animation 12 or 11) to the heading the entity
   spawned with, EntitySpawnInfo.spawn_rotation_y, then goes to state 0 and thinks. */
static void StoneGuardian_TurnToSpawnHeading(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 frames;

    entity->busy_flags = ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        if (EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y) >= 0.0f) {
            Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 1);
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            frames = EntityAi_AnimationFrameCount(entity, entity->animation_index);
            entity->action_timer = frames;
            if (entity->ailment_3_frames != 0) {
                entity->action_timer = frames * 2;
            }
            entity->action_step++;
        }
        break;
    case 2:
        EntityMath_EaseAngleToward(&entity->model->rotation[1], entity->spawn_info->spawn_rotation_y, entity->action_timer);
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->part_angle_x = entity->model->position[0];
            ai_state->part_angle_y = entity->model->position[1];
            ai_state->part_angle_z = entity->model->position[2];
            entity->prev_state = entity->state;
            entity->state = STONE_GUARDIAN_STATE_HOLD;
            ai_state->think(entity, 1);
        }
        break;
    }
}

/* The action of state 0 in modes 2 to 5: stands in animation 1 with flag bit 18 set. */
static void StoneGuardian_HoldAnim1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    Motion_Start(entity, 1, 0, entity->params->motion_blend_frames, 0);
    ai_state->turn_rate = 0.0f;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
}

/* The think handler of mode 1 (EntitySpawnInfo.spawn_mode is 1): holds animation 17 with flag bits 16,
   18, 21 to 24 and 28 set and starts no action. */
static void StoneGuardian_ThinkHoldAnim17(Entity *entity, s32 run_now) {
    s64 flags;

    Motion_Start(entity, 0x11, 0, 0, run_now);
    flags = entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    flags |= ENTITY_FLAG_400000;
    flags |= ENTITY_FLAG_NO_WALL_COLLISION;
    flags |= ENTITY_FLAG_NO_SECOND_WALL_COLLISION;
    flags |= ENTITY_FLAG_40000;
    flags |= ENTITY_FLAG_200000;
    flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
    flags |= ENTITY_FLAG_NOT_TARGETABLE;
    entity->flags = flags;
}

/* The think handler of mode 2: sets Entity.state itself and starts the action of the state
   (StoneGuardian_HoldAnim1, StoneGuardian_Mode2WalkToPost, StoneGuardian_Mode2ChaseInBounds,
   StoneGuardian_Mode2ReturnToSpawn). While bit 17 of the map object EntitySpawnInfo.variant names is
   clear the state is 2 with the player inside a fixed rectangle, else 1; once the bit is set it is
   3 until the entity is back at its place, then 0. Every frame it clamps the entity's position to
   fixed bounds. */
static void StoneGuardian_ThinkMode2StayInBounds(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->busy_flags == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->state_flags >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (player_state.position.x > -9700.0f && player_state.position.x < -700.0f &&
            player_state.position.z > -4800.0f && player_state.position.z < -3600.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == STONE_GUARDIAN_STATE_HOLD) {
                entity->state = STONE_GUARDIAN_STATE_HOLD;
            } else {
                entity->state = STONE_GUARDIAN_STATE_RETURN_TO_SPAWN;
            }
        } else if (player_in_zone) {
            entity->state = STONE_GUARDIAN_STATE_CHASE;
        } else {
            entity->state = STONE_GUARDIAN_STATE_WALK_TO_POST;
        }
        switch (entity->state) {
        case STONE_GUARDIAN_STATE_HOLD:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, run_now);
            ai_state->pause_timer = entity->params->pitch_limit_deg;
            break;
        case STONE_GUARDIAN_STATE_WALK_TO_POST:
            if (ai_state->pause_timer == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2WalkToPost, run_now);
                ai_state->sub_timer = entity->params->yaw_limit_deg;
            }
            break;
        case STONE_GUARDIAN_STATE_CHASE:
            if (ai_state->pause_timer == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2ChaseInBounds, run_now);
                ai_state->sub_timer = entity->params->yaw_limit_deg;
            }
            break;
        case STONE_GUARDIAN_STATE_RETURN_TO_SPAWN:
            if (ai_state->pause_timer == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2ReturnToSpawn, run_now);
            }
            break;
        default:
            entity->state = STONE_GUARDIAN_STATE_HOLD;
            break;
        }
        if (ai_state->pause_timer > 0) {
            ai_state->pause_timer--;
        }
        if (ai_state->sub_timer > 0) {
            ai_state->sub_timer--;
        }
    }
    if (entity->model->position[0] < -8400.0f) {
        entity->model->position[0] = -8400.0f;
    }
    if (entity->model->position[0] > -1950.0f) {
        entity->model->position[0] = -1950.0f;
    }
    if (entity->model->position[2] > -4000.0f - entity->collision_radius) {
        entity->model->position[2] = -4000.0f - entity->collision_radius;
    }
    if (entity->model->position[0] < -3150.0f) {
        if (entity->model->position[2] < entity->collision_radius + -4800.0f) {
            entity->model->position[2] = entity->collision_radius + -4800.0f;
        }
    }
}

/* The action of state 1 in mode 2: attacks when EntityAi_CanAttack says so; else goes to a post picked
   by where the player is and turns to a fixed heading there. */
static void StoneGuardian_Mode2WalkToPost(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 post[4];
    f32 facing;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (!(s32)(entity->flags >> 63) && EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (player_state.position.x < -6000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = HALF_PI;
    } else if (player_state.position.x > -4350.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = -HALF_PI;
    } else if (player_state.position.z < -4000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = entity->collision_radius + -4800.0f;
        post[3] = 0.0f;
        facing = 0.0f;
    } else {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4000.0f - entity->collision_radius;
        post[3] = 0.0f;
        facing = PI;
    }
    if (entity->model->position[2] < -4800.0f) {
        ai_state->target_point[0] = entity->spawn_info->spawn_x;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4200.0f;
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, post) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], facing);
        if (ABS(angle) > DEG_TO_RAD(30)) {
            if (angle >= 0.0f) {
                entity->move_command = MOVE_COMMAND_TURN_RIGHT;
            } else {
                entity->move_command = MOVE_COMMAND_TURN_LEFT;
            }
        }
    } else {
        Vector_Copy(ai_state->target_point, post);
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f) | MOVE_COMMAND_TURN_FAST;
    }
    (ai_state->move)(entity);
}

/* The action of state 2 in mode 2: attacks when EntityAi_CanAttack says so; else goes for the player,
   stopping at the edge of its bounds and turning to the player there. */
static void StoneGuardian_Mode2ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (!(s32)(entity->flags >> 63) && EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position.x < -8400.0f) {
        ai_state->target_point[0] = entity->collision_radius + -8400.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position.x > -1950.0f) {
        ai_state->target_point[0] = -1950.0f - entity->collision_radius;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position.z > -4000.0f) {
        ai_state->target_point[0] = -5200.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = -4000.0f - entity->collision_radius;
        ai_state->target_point[3] = 0.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position.z < -4800.0f && player_state.position.x > -5600.0f &&
               player_state.position.x < -4800.0f) {
        ai_state->target_point[0] = -5200.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = entity->collision_radius + -4800.0f;
        ai_state->target_point[3] = 0.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else {
        entity->target = &player_state;
        entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
    }
    (ai_state->move)(entity);
}

/* The action of state 3 in mode 2: after the pause in work word unk6A goes back to its spawn point,
   turns to its spawn heading (StoneGuardian_TurnToSpawnHeading) and goes to state 0; still attacks when
   EntityAi_CanAttack says so. */
static void StoneGuardian_Mode2ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        (ai_state->move)(entity);
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (!(s32)(entity->flags >> 63) && EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < -3150.0f) {
        ai_state->target_point[0] = entity->spawn_info->spawn_x;
        entity->target = ai_state->target_point;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4200.0f;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > DEG_TO_RAD(10)) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = STONE_GUARDIAN_STATE_HOLD;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    (ai_state->move)(entity);
}

/* The think handler of mode 3: as StoneGuardian_ThinkMode2StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode3WalkToPost, StoneGuardian_Mode3ChaseInBounds,
   StoneGuardian_Mode3ReturnToSpawn). */
static void StoneGuardian_ThinkMode3StayInBounds(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->busy_flags == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->state_flags >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (player_state.position.z > -6000.0f && player_state.position.z < -2800.0f &&
            entity->model->position[2] >= -4800.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == STONE_GUARDIAN_STATE_HOLD) {
                entity->state = STONE_GUARDIAN_STATE_HOLD;
            } else {
                entity->state = STONE_GUARDIAN_STATE_RETURN_TO_SPAWN;
            }
        } else if (player_in_zone) {
            entity->state = STONE_GUARDIAN_STATE_CHASE;
        } else {
            entity->state = STONE_GUARDIAN_STATE_WALK_TO_POST;
        }
        switch (entity->state) {
        case STONE_GUARDIAN_STATE_HOLD:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, run_now);
            break;
        case STONE_GUARDIAN_STATE_WALK_TO_POST:
            EntityAi_StartAction(entity, StoneGuardian_Mode3WalkToPost, run_now);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case STONE_GUARDIAN_STATE_CHASE:
            EntityAi_StartAction(entity, StoneGuardian_Mode3ChaseInBounds, run_now);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case STONE_GUARDIAN_STATE_RETURN_TO_SPAWN:
            EntityAi_StartAction(entity, StoneGuardian_Mode3ReturnToSpawn, run_now);
            break;
        default:
            entity->state = STONE_GUARDIAN_STATE_HOLD;
            break;
        }
        if (ai_state->sub_timer > 0) {
            ai_state->sub_timer--;
        }
    }
    if (entity->model->position[0] < 4800.0f) {
        entity->model->position[0] = 4800.0f;
    }
    if (entity->model->position[0] > 6650.0f - entity->collision_radius) {
        entity->model->position[0] = 6650.0f - entity->collision_radius;
    }
    if (entity->model->position[2] > -4000.0f - entity->collision_radius) {
        entity->model->position[2] = -4000.0f - entity->collision_radius;
    }
}

/* The action of state 1 in mode 3: attacks when EntityAi_CanAttack says so; else goes to a fixed post
   and turns to a fixed heading there. */
static void StoneGuardian_Mode3WalkToPost(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = -4400.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) < range) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], PI);
        if (ABS(angle) > HALF_PI) {
            if (angle >= 0.0f) {
                entity->move_command = MOVE_COMMAND_TURN_RIGHT;
            } else {
                entity->move_command = MOVE_COMMAND_TURN_LEFT;
            }
        }
    } else {
        Vector_Copy(ai_state->target_point, home);
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f) | MOVE_COMMAND_TURN_FAST;
    }
    (ai_state->move)(entity);
}

/* The action of state 2 in mode 3: attacks when EntityAi_CanAttack says so; else goes for the player,
   stopping at the edge of its bounds and turning to the player there. */
static void StoneGuardian_Mode3ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position.x < 4800.0f) {
        ai_state->target_point[0] = 4800.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position.x > 6650.0f) {
        ai_state->target_point[0] = 6650.0f - entity->collision_radius;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else {
        entity->target = &player_state;
        entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + range + 30.0f);
    }
    (ai_state->move)(entity);
}

/* The action of state 3 in mode 3: after the pause in work word unk6A goes back to its spawn point,
   turns to its spawn heading (StoneGuardian_TurnToSpawnHeading) and goes to state 0; still attacks when
   EntityAi_CanAttack says so. */
static void StoneGuardian_Mode3ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        (ai_state->move)(entity);
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < 5450.0f || entity->model->position[0] > 6150.0f) {
        ai_state->target_point[0] = entity->spawn_info->spawn_x;
        entity->target = ai_state->target_point;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4200.0f;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > DEG_TO_RAD(10)) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = STONE_GUARDIAN_STATE_HOLD;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    (ai_state->move)(entity);
}

/* The think handler of mode 4: as StoneGuardian_ThinkMode3StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode4WalkToPost, StoneGuardian_Mode4ChaseInBounds,
   StoneGuardian_Mode4ReturnToSpawn). */
static void StoneGuardian_ThinkMode4StayInBounds(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->busy_flags == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->state_flags >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (player_state.position.z > -6000.0f && player_state.position.z < -2800.0f &&
            entity->model->position[2] <= -4000.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == STONE_GUARDIAN_STATE_HOLD) {
                entity->state = STONE_GUARDIAN_STATE_HOLD;
            } else {
                entity->state = STONE_GUARDIAN_STATE_RETURN_TO_SPAWN;
            }
        } else if (player_in_zone) {
            entity->state = STONE_GUARDIAN_STATE_CHASE;
        } else {
            entity->state = STONE_GUARDIAN_STATE_WALK_TO_POST;
        }
        switch (entity->state) {
        case STONE_GUARDIAN_STATE_HOLD:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, run_now);
            break;
        case STONE_GUARDIAN_STATE_WALK_TO_POST:
            EntityAi_StartAction(entity, StoneGuardian_Mode4WalkToPost, run_now);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case STONE_GUARDIAN_STATE_CHASE:
            EntityAi_StartAction(entity, StoneGuardian_Mode4ChaseInBounds, run_now);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case STONE_GUARDIAN_STATE_RETURN_TO_SPAWN:
            EntityAi_StartAction(entity, StoneGuardian_Mode4ReturnToSpawn, run_now);
            break;
        default:
            entity->state = STONE_GUARDIAN_STATE_HOLD;
            break;
        }
        if (ai_state->sub_timer > 0) {
            ai_state->sub_timer--;
        }
    }
    if (entity->model->position[0] < entity->collision_radius + 6150.0f) {
        entity->model->position[0] = entity->collision_radius + 6150.0f;
    }
    if (entity->model->position[0] > 8000.0f) {
        entity->model->position[0] = 8000.0f;
    }
    if (entity->model->position[2] < entity->collision_radius + -4800.0f) {
        entity->model->position[2] = entity->collision_radius + -4800.0f;
    }
}

/* The action of state 1 in mode 4: the same as StoneGuardian_Mode3WalkToPost. */
static void StoneGuardian_Mode4WalkToPost(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = -4400.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) < range) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], PI);
        if (ABS(angle) > HALF_PI) {
            if (angle >= 0.0f) {
                entity->move_command = MOVE_COMMAND_TURN_RIGHT;
            } else {
                entity->move_command = MOVE_COMMAND_TURN_LEFT;
            }
        }
    } else {
        Vector_Copy(ai_state->target_point, home);
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f) | MOVE_COMMAND_TURN_FAST;
    }
    (ai_state->move)(entity);
}

/* The action of state 2 in mode 4: as StoneGuardian_Mode3ChaseInBounds with its own bounds. */
static void StoneGuardian_Mode4ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position.x < 6150.0f) {
        ai_state->target_point[0] = entity->collision_radius + 6150.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position.x > 8000.0f) {
        ai_state->target_point[0] = 8000.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else {
        entity->target = &player_state;
        entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + range + 30.0f);
    }
    (ai_state->move)(entity);
}

/* The action of state 3 in mode 4: as StoneGuardian_Mode3ReturnToSpawn with its own bounds. */
static void StoneGuardian_Mode4ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        (ai_state->move)(entity);
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < 6650.0f || entity->model->position[0] > 7350.0f) {
        ai_state->target_point[0] = entity->spawn_info->spawn_x;
        entity->target = ai_state->target_point;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4200.0f;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > DEG_TO_RAD(10)) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = STONE_GUARDIAN_STATE_HOLD;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    (ai_state->move)(entity);
}

/* The think handler of mode 5: as StoneGuardian_ThinkMode3StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode5WalkToPost, StoneGuardian_Mode5ChaseInBounds,
   StoneGuardian_Mode5ReturnToSpawn). */
static void StoneGuardian_ThinkMode5StayInBounds(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->busy_flags == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->state_flags >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (player_state.position.x > -20300.0f && player_state.position.x < -13350.0f &&
            player_state.position.z > -250.0f && player_state.position.z < 1800.0f &&
            entity->model->position[2] <= 950.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == STONE_GUARDIAN_STATE_HOLD) {
                entity->state = STONE_GUARDIAN_STATE_HOLD;
            } else {
                entity->state = STONE_GUARDIAN_STATE_RETURN_TO_SPAWN;
            }
        } else if (player_in_zone) {
            entity->state = STONE_GUARDIAN_STATE_CHASE;
        } else {
            entity->state = STONE_GUARDIAN_STATE_WALK_TO_POST;
        }
        switch (entity->state) {
        case STONE_GUARDIAN_STATE_HOLD:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, run_now);
            break;
        case STONE_GUARDIAN_STATE_WALK_TO_POST:
            EntityAi_StartAction(entity, StoneGuardian_Mode5WalkToPost, run_now);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case STONE_GUARDIAN_STATE_CHASE:
            EntityAi_StartAction(entity, StoneGuardian_Mode5ChaseInBounds, run_now);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case STONE_GUARDIAN_STATE_RETURN_TO_SPAWN:
            EntityAi_StartAction(entity, StoneGuardian_Mode5ReturnToSpawn, run_now);
            break;
        default:
            entity->state = STONE_GUARDIAN_STATE_HOLD;
            break;
        }
        if (ai_state->sub_timer > 0) {
            ai_state->sub_timer--;
        }
    }
    if (entity->model->position[0] < -18300.0f) {
        entity->model->position[0] = -18300.0f;
    }
    if (entity->model->position[0] > -16050.0f) {
        entity->model->position[0] = -16050.0f;
    }
    if (entity->model->position[2] < 600.0f) {
        entity->model->position[2] = 600.0f;
    }
}

/* The action of state 1 in mode 5: as StoneGuardian_Mode3WalkToPost with its own post and heading. */
static void StoneGuardian_Mode5WalkToPost(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->model->position[1];
    home[2] = 600.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) < range) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], 0.0f);
        if (ABS(angle) > HALF_PI) {
            if (angle >= 0.0f) {
                entity->move_command = MOVE_COMMAND_TURN_RIGHT;
            } else {
                entity->move_command = MOVE_COMMAND_TURN_LEFT;
            }
        }
    } else {
        Vector_Copy(ai_state->target_point, home);
        entity->target = ai_state->target_point;
        entity->move_command = EntityAi_MoveCommand(entity, 1, 50.0f) | MOVE_COMMAND_TURN_FAST;
    }
    (ai_state->move)(entity);
}

/* The action of state 2 in mode 5: as StoneGuardian_Mode3ChaseInBounds with its own bounds. */
static void StoneGuardian_Mode5ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position.x < -18300.0f) {
        ai_state->target_point[0] = -18300.0f;
        ai_state->target_point[1] = entity->model->position[1];
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = 600.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position.x > -16050.0f) {
        ai_state->target_point[0] = -16050.0f;
        ai_state->target_point[1] = entity->model->position[1];
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = 600.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, &player_state.position.x);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > HALF_PI) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else {
        entity->target = &player_state;
        entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + range + 30.0f);
    }
    (ai_state->move)(entity);
}

/* The action of state 3 in mode 5: as StoneGuardian_Mode3ReturnToSpawn with its own bounds. */
static void StoneGuardian_Mode5ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        (ai_state->move)(entity);
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    if (EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack(entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < -17900.0f || entity->model->position[0] > -17200.0f) {
        ai_state->target_point[0] = entity->spawn_info->spawn_x;
        entity->target = ai_state->target_point;
        ai_state->target_point[1] = entity->model->position[1];
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = 600.0f;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > DEG_TO_RAD(10)) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = STONE_GUARDIAN_STATE_HOLD;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    (ai_state->move)(entity);
}

/* The think handler of mode 6: a sequence kept in work byte unk7A (1 to 7; at 0 it is
   EntityAi_Think), driven by whether the two map objects EntitySpawnInfo.variant and unk42 name
   both have bit 17 set. While both are set the entity is kept unseen at a fixed point (steps 1 and
   2); when one clears it comes out, one entity of the five in stone_guardian_spawn_ids at a time (step 3,
   StoneGuardian_PlayAnim9, then 0); when both are set again it goes back the same way (steps 4 to 7,
   StoneGuardian_PlayAnim8). The functions named Mode6... do the work of a step. */
static void StoneGuardian_ThinkMode6HideSequence(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 post[4];
    MapObject *map_object;
    s32 first_open;
    s32 second_open;
    s32 both_open;

    first_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
    if (map_object != NULL) {
        first_open = (map_object->state_flags >> 17) & 1;
    }
    second_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->map_object_id);
    if (map_object != NULL) {
        second_open = (map_object->state_flags >> 17) & 1;
    }
    both_open = first_open && second_open;
    if (entity->busy_flags != 0) {
        return;
    }
    if (!both_open && ai_state->step_index != 0) {
        if (EntityMath_DistanceXZ(&entity->spawn_info->spawn_x, &player_state.position.x) > 5400.0f) {
            Vector_Copy(entity->model->position, entity->spawn_info);
            ai_state->step_index = 0;
        }
    }
    switch (ai_state->step_index) {
    case 1:
        if (!both_open) {
            ai_state->step_index++;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        } else {
            StoneGuardian_Mode6StayHidden(entity, run_now);
        }
        break;
    case 2:
        if (!both_open) {
            if (!StoneGuardian_Mode6IsStepFree(entity, 3) || !StoneGuardian_Mode6IsStepFree(entity, 6) || !StoneGuardian_Mode6IsStepFree(entity, 7)) {
                StoneGuardian_Mode6WaitTurnHidden(entity, run_now);
            } else {
                ai_state->step_index++;
                EntityAi_StartAction(entity, ai_state->move, run_now);
            }
        } else {
            ai_state->step_index = 1;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        }
        break;
    case 3:
        if (!both_open) {
            StoneGuardian_Mode6LeaveHiding(entity, run_now);
        } else {
            ai_state->step_index = 7;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        }
        break;
    case 4:
        post[0] = -18450.0f;
        post[1] = 3190.0f;
        post[2] = 4900.0f;
        ai_state->pitch_speed = EntityMath_DistanceXZ(entity->model->position, post);
        ai_state->step_index++;
        EntityAi_StartAction(entity, ai_state->move, run_now);
        break;
    case 5:
        if (!both_open) {
            ai_state->step_index = 0;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        } else if (StoneGuardian_Mode6IsStepFree(entity, 3) && StoneGuardian_Mode6IsStepFree(entity, 6) && StoneGuardian_Mode6IsStepFree(entity, 7) &&
                   StoneGuardian_Mode6IsNearestWaiting(entity, ai_state->pitch_speed)) {
            ai_state->step_index++;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        } else {
            StoneGuardian_Mode6WaitTurnShown(entity, run_now);
        }
        break;
    case 6:
        if (!both_open) {
            ai_state->step_index = 0;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        } else {
            StoneGuardian_Mode6ReturnToHiding(entity, run_now);
        }
        break;
    case 7:
        if (!both_open) {
            ai_state->step_index = 3;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        } else {
            StoneGuardian_Mode6WalkToHidePoint(entity, run_now);
        }
        break;
    default:
        ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        entity->model_alpha = 0x80;
        if (both_open) {
            ai_state->step_index = 4;
            EntityAi_StartAction(entity, ai_state->move, run_now);
        } else {
            EntityAi_Think(entity, run_now);
        }
        break;
    }
}

/* Step 1 of StoneGuardian_ThinkMode6HideSequence: keeps the entity unseen (Entity.model_alpha 0; flag bits 18, 20, 21,
   26, 27) at a fixed point. */
static void StoneGuardian_Mode6StayHidden(Entity *entity, s32 run_now) {
    EntityModel *model;

    model = entity->model;
    ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    entity->model_alpha = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->rotation[1] = HALF_PI;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    EntityAi_StartAction(entity, entity->ai_state.move, run_now);
}

/* Step 2 of StoneGuardian_ThinkMode6HideSequence: the same as StoneGuardian_Mode6StayHidden. What
   differs is when the think handler calls it: one of the two map objects has its bit clear, and
   another of the five entities is still at step 3, 6 or 7 (StoneGuardian_Mode6IsStepFree). */
static void StoneGuardian_Mode6WaitTurnHidden(Entity *entity, s32 run_now) {
    EntityModel *model = entity->model;

    ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    entity->model_alpha = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->rotation[1] = HALF_PI;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    EntityAi_StartAction(entity, entity->ai_state.move, run_now);
}

/* Step 3 of StoneGuardian_ThinkMode6HideSequence: shows the entity, walks it to a second fixed point, turns it
   to heading 90 degrees and starts StoneGuardian_PlayAnim9. */
static void StoneGuardian_Mode6LeaveHiding(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    entity->model_alpha = 0x80;
    ai_state->target_point[0] = -18050.0f;
    ai_state->target_point[1] = 3030.0f;
    ai_state->target_point[2] = 4900.0f;
    if (entity->model->position[1] > 3031.0f) {
        ai_state->step_index = 0;
        ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = FALSE;
    } else {
        point = ai_state->target_point;
        if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
            ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
            angle = EntityMath_AngleDifference(entity->model->rotation[1], HALF_PI);
            if (ABS(angle) > DEG_TO_RAD(10)) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
                ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            } else {
                EntityAi_StartAction(entity, StoneGuardian_PlayAnim9, run_now);
                return;
            }
        } else {
            entity->target = point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, range);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* An action (busy bit 0x1000) of mode 6, which StoneGuardian_Mode6LeaveHiding starts: plays
   animation 9 to its end with Entity.model_alpha at 0x80, then puts the sequence at 0 (from there the
   think handler is EntityAi_Think) and thinks. */
static void StoneGuardian_PlayAnim9(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    entity->model_alpha = 0x80;
    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ai_state->step_index = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            ENTITY_FLAG_BITS(entity)->draw_afterimage = TRUE;
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

/* Step 5 of StoneGuardian_ThinkMode6HideSequence: the entity is seen (Entity.model_alpha 0x80, the flag
   bits of the other steps cleared) and stands, the move action with no command. The think handler
   calls it while both map objects have their bit set and it is not yet this entity's turn to go
   back: another one is at step 3, 6 or 7 (StoneGuardian_Mode6IsStepFree) or waits nearer
   (StoneGuardian_Mode6IsNearestWaiting). */
static void StoneGuardian_Mode6WaitTurnShown(Entity *entity, s32 run_now) {
    ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    entity->model_alpha = 0x80;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    EntityAi_StartAction(entity, entity->ai_state.move, run_now);
}

/* Step 6 of StoneGuardian_ThinkMode6HideSequence: walks the entity to a third fixed point, turns it to heading
   -90 degrees and starts StoneGuardian_PlayAnim8. */
static void StoneGuardian_Mode6ReturnToHiding(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    entity->model_alpha = 0x80;
    ai_state->target_point[0] = -18450.0f;
    ai_state->target_point[1] = 3190.0f;
    ai_state->target_point[2] = 4900.0f;
    if (entity->model->position[1] <= 3030.0f) {
        ai_state->step_index = 7;
    } else {
        point = ai_state->target_point;
        if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], -HALF_PI);
            if (ABS(angle) > DEG_TO_RAD(10)) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
                ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            } else {
                EntityAi_StartAction(entity, StoneGuardian_PlayAnim8, run_now);
                return;
            }
        } else {
            entity->target = point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, range);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* An action (busy bit 0x1000) of mode 6, which StoneGuardian_Mode6ReturnToHiding starts: plays
   animation 8 to its end, then puts the sequence at 7 (StoneGuardian_Mode6WalkToHidePoint) and
   thinks. */
static void StoneGuardian_PlayAnim8(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    entity->model_alpha = 0x80;
    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ai_state->step_index = 7;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
            ENTITY_FLAG_BITS(entity)->draw_afterimage = TRUE;
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

/* Step 7 of StoneGuardian_ThinkMode6HideSequence: walks the entity back to the first fixed point, then puts the
   sequence at 1. */
static void StoneGuardian_Mode6WalkToHidePoint(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *point = ai_state->target_point;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    entity->model_alpha = 0x80;
    ai_state->target_point[0] = -16200.0f;
    ai_state->target_point[1] = 3030.0f;
    ai_state->target_point[2] = 4900.0f;
    if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
        ai_state->step_index = 1;
    } else {
        entity->target = point;
        entity->move_command = EntityAi_MoveCommand(entity, 0, range);
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* Helper of StoneGuardian_ThinkMode6HideSequence: whether none of the other live entities among the five in
   stone_guardian_spawn_ids is at step `value` of the sequence. */
static s32 StoneGuardian_Mode6IsStepFree(Entity *entity, s32 value) {
    EntityIdTable table = stone_guardian_spawn_ids;
    s32 i;
    Entity *other;

    for (i = 0; i < 5; i++) {
        other = Entity_GetBySpawnId(table.ids[i]);
        if (other != NULL && (other->flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_DYING_MASK)) == 1 && other != entity && other->ai_state.step_index == value) {
            return FALSE;
        }
    }
    return TRUE;
}

/* Helper of StoneGuardian_ThinkMode6HideSequence: whether none of the other live entities among the
   five in stone_guardian_spawn_ids is at step 5 with its work float unk54 below `limit`. unk54 is the distance to
   the post of step 6 that step 4 stored, and the caller passes its own, so TRUE means this entity
   is the nearest of those waiting. */
static s32 StoneGuardian_Mode6IsNearestWaiting(Entity *entity, f32 limit) {
    EntityIdTable table = stone_guardian_spawn_ids;
    s32 i;
    Entity *other;
    EntityAiState *other_unkE0;

    for (i = 0; i < 5; i++) {
        other = Entity_GetBySpawnId(table.ids[i]);
        if (other != NULL && (other->flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_DYING_MASK)) == 1 && other != entity) {
            other_unkE0 = &other->ai_state;
            if (other_unkE0->step_index == 5 && other_unkE0->pitch_speed < limit) {
                return FALSE;
            }
        }
    }
    return TRUE;
}
