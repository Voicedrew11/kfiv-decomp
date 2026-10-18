/* StoneGuardian: the behaviour of entity type 23 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Stone Guardian. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "map_object/map_object.h"
#include "entity/entity_world.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_Think(Entity *, s32);
void EntityAi_Move(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionHeavy(Entity *);
int Motion_Start(void *, int, int, u32, int);
float EntityMath_AngleDifference(float, float);
int EntityAi_AnimationFrameCount(void *, int);
float EntityMath_EaseAngleToward(float *, float, float);
extern u32 frame_counter;
extern PlayerData player_state;
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
float EntityMath_DistanceXZ(void *, void *);
void Vector_Copy(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
float EntityMath_HeadingTo(float *, float *);
extern EntityIdTable stone_guardian_spawn_ids;

static void StoneGuardian_Animate(Entity *entity);
static void StoneGuardian_OnHit(Entity *entity);
static void StoneGuardian_Move(Entity *entity);
static void StoneGuardian_TurnToSpawnHeading(Entity *entity);
static void StoneGuardian_HoldAnim1(Entity *entity);
static void StoneGuardian_ThinkHoldAnim17(Entity *entity, s32 unknown);
static void StoneGuardian_ThinkMode2StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode2WalkToPost(Entity *entity);
static void StoneGuardian_Mode2ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode2ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode3StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode3WalkToPost(Entity *entity);
static void StoneGuardian_Mode3ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode3ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode4StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode4WalkToPost(Entity *entity);
static void StoneGuardian_Mode4ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode4ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode5StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode5WalkToPost(Entity *entity);
static void StoneGuardian_Mode5ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode5ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode6HideSequence(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6StayHidden(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6WaitTurnHidden(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6LeaveHiding(Entity *entity, s32 unknown);
static void StoneGuardian_PlayAnim9(Entity *entity);
static void StoneGuardian_Mode6WaitTurnShown(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6ReturnToHiding(Entity *entity, s32 unknown);
static void StoneGuardian_PlayAnim8(Entity *entity);
static void StoneGuardian_Mode6WalkToHidePoint(Entity *entity, s32 unknown);
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
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->animate = (void (*)(void))StoneGuardian_Animate;
    ai_state->on_hit = (void (*)(void))StoneGuardian_OnHit;
    ai_state->move = (void (*)(void))StoneGuardian_Move;
    ai_state->death = (void (*)(void))EntityAi_Death;
    ai_state->dormant = NULL;
    if (entity->spawn_info->spawn_mode == 1) {
        ai_state->move = NULL;
        ai_state->think = StoneGuardian_ThinkHoldAnim17;
    } else if (entity->spawn_info->spawn_mode == 2) {
        ai_state->think = StoneGuardian_ThinkMode2StayInBounds;
    } else if (entity->spawn_info->spawn_mode == 3) {
        ai_state->think = StoneGuardian_ThinkMode3StayInBounds;
    } else if (entity->spawn_info->spawn_mode == 4) {
        ai_state->think = StoneGuardian_ThinkMode4StayInBounds;
    } else if (entity->spawn_info->spawn_mode == 5) {
        ai_state->think = StoneGuardian_ThinkMode5StayInBounds;
    } else if (entity->spawn_info->spawn_mode == 6) {
        ai_state->think = StoneGuardian_ThinkMode6HideSequence;
        ai_state->move = (void (*)(void))EntityAi_Move;
        entity->model->position[0] = -16200.0f;
        entity->model->position[1] = 3030.0f;
        entity->model->position[2] = 4900.0f;
        entity->model_alpha = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ai_state->step_index = 1;
    } else {
        ai_state->think = EntityAi_Think;
        ai_state->move = (void (*)(void))EntityAi_Move;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    ai_state->pose_blend_weight = 0.5f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
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
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void StoneGuardian_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (((entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
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
        if (entity->animation_index != 3 && entity->animation_index != 4) {
            if (entity->move_command & MOVE_COMMAND_RUN) {
                Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 1);
            } else {
                Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 1);
            }
        } else if (entity->move_command & MOVE_COMMAND_RUN) {
            entity->animation_index = 4;
            entity->model->level_tables->frame_step = entity->params->motion_speeds[entity->animation_index];
        } else {
            entity->animation_index = 3;
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
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
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
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->part_angle_x = entity->model->position[0];
            ai_state->part_angle_y = entity->model->position[1];
            ai_state->part_angle_z = entity->model->position[2];
            entity->prev_state = entity->state;
            entity->state = 0;
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
static void StoneGuardian_ThinkHoldAnim17(Entity *entity, s32 unknown) {
    s64 flags;

    Motion_Start(entity, 0x11, 0, 0, unknown);
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
static void StoneGuardian_ThinkMode2StayInBounds(Entity *entity, s32 unknown) {
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
        if (player_state.position[0] > -9700.0f && player_state.position[0] < -700.0f &&
            player_state.position[2] > -4800.0f && player_state.position[2] < -3600.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            ai_state->pause_timer = entity->params->pitch_limit_deg;
            break;
        case 1:
            if (ai_state->pause_timer == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2WalkToPost, unknown);
                ai_state->sub_timer = entity->params->yaw_limit_deg;
            }
            break;
        case 2:
            if (ai_state->pause_timer == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2ChaseInBounds, unknown);
                ai_state->sub_timer = entity->params->yaw_limit_deg;
            }
            break;
        case 3:
            if (ai_state->pause_timer == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2ReturnToSpawn, unknown);
            }
            break;
        default:
            entity->state = 0;
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

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (player_state.position[0] < -6000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = 1.5707964f;
    } else if (player_state.position[0] > -4350.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = -1.5707964f;
    } else if (player_state.position[2] < -4000.0f) {
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
        facing = 3.1415927f;
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
        if (ABS(angle) > 0.5235988f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 2 in mode 2: attacks when EntityAi_CanAttack says so; else goes for the player,
   stopping at the edge of its bounds and turning to the player there. */
static void StoneGuardian_Mode2ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position[0] < -8400.0f) {
        ai_state->target_point[0] = entity->collision_radius + -8400.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position[0] > -1950.0f) {
        ai_state->target_point[0] = -1950.0f - entity->collision_radius;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position[2] > -4000.0f) {
        ai_state->target_point[0] = -5200.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = -4000.0f - entity->collision_radius;
        ai_state->target_point[3] = 0.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position[2] < -4800.0f && player_state.position[0] > -5600.0f &&
               player_state.position[0] < -4800.0f) {
        ai_state->target_point[0] = -5200.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[2] = entity->collision_radius + -4800.0f;
        ai_state->target_point[3] = 0.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 3 in mode 2: after the pause in work word unk6A goes back to its spawn point,
   turns to its spawn heading (StoneGuardian_TurnToSpawnHeading) and goes to state 0; still attacks when
   EntityAi_CanAttack says so. */
static void StoneGuardian_Mode2ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        ai_state->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
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
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The think handler of mode 3: as StoneGuardian_ThinkMode2StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode3WalkToPost, StoneGuardian_Mode3ChaseInBounds,
   StoneGuardian_Mode3ReturnToSpawn). */
static void StoneGuardian_ThinkMode3StayInBounds(Entity *entity, s32 unknown) {
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
        if (player_state.position[2] > -6000.0f && player_state.position[2] < -2800.0f &&
            entity->model->position[2] >= -4800.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            break;
        case 1:
            EntityAi_StartAction(entity, StoneGuardian_Mode3WalkToPost, unknown);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case 2:
            EntityAi_StartAction(entity, StoneGuardian_Mode3ChaseInBounds, unknown);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case 3:
            EntityAi_StartAction(entity, StoneGuardian_Mode3ReturnToSpawn, unknown);
            break;
        default:
            entity->state = 0;
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

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
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
        angle = EntityMath_AngleDifference(entity->model->rotation[1], 3.1415927f);
        if (ABS(angle) > 1.5707964f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 2 in mode 3: attacks when EntityAi_CanAttack says so; else goes for the player,
   stopping at the edge of its bounds and turning to the player there. */
static void StoneGuardian_Mode3ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position[0] < 4800.0f) {
        ai_state->target_point[0] = 4800.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position[0] > 6650.0f) {
        ai_state->target_point[0] = 6650.0f - entity->collision_radius;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 3 in mode 3: after the pause in work word unk6A goes back to its spawn point,
   turns to its spawn heading (StoneGuardian_TurnToSpawnHeading) and goes to state 0; still attacks when
   EntityAi_CanAttack says so. */
static void StoneGuardian_Mode3ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        ai_state->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
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
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The think handler of mode 4: as StoneGuardian_ThinkMode3StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode4WalkToPost, StoneGuardian_Mode4ChaseInBounds,
   StoneGuardian_Mode4ReturnToSpawn). */
static void StoneGuardian_ThinkMode4StayInBounds(Entity *entity, s32 unknown) {
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
        if (player_state.position[2] > -6000.0f && player_state.position[2] < -2800.0f &&
            entity->model->position[2] <= -4000.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            break;
        case 1:
            EntityAi_StartAction(entity, StoneGuardian_Mode4WalkToPost, unknown);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case 2:
            EntityAi_StartAction(entity, StoneGuardian_Mode4ChaseInBounds, unknown);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case 3:
            EntityAi_StartAction(entity, StoneGuardian_Mode4ReturnToSpawn, unknown);
            break;
        default:
            entity->state = 0;
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

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
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
        angle = EntityMath_AngleDifference(entity->model->rotation[1], 3.1415927f);
        if (ABS(angle) > 1.5707964f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 2 in mode 4: as StoneGuardian_Mode3ChaseInBounds with its own bounds. */
static void StoneGuardian_Mode4ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position[0] < 6150.0f) {
        ai_state->target_point[0] = entity->collision_radius + 6150.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position[0] > 8000.0f) {
        ai_state->target_point[0] = 8000.0f;
        ai_state->target_point[1] = entity->spawn_info->spawn_y;
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 3 in mode 4: as StoneGuardian_Mode3ReturnToSpawn with its own bounds. */
static void StoneGuardian_Mode4ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        ai_state->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
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
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The think handler of mode 5: as StoneGuardian_ThinkMode3StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode5WalkToPost, StoneGuardian_Mode5ChaseInBounds,
   StoneGuardian_Mode5ReturnToSpawn). */
static void StoneGuardian_ThinkMode5StayInBounds(Entity *entity, s32 unknown) {
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
        if (player_state.position[0] > -20300.0f && player_state.position[0] < -13350.0f &&
            player_state.position[2] > -250.0f && player_state.position[2] < 1800.0f &&
            entity->model->position[2] <= 950.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            break;
        case 1:
            EntityAi_StartAction(entity, StoneGuardian_Mode5WalkToPost, unknown);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case 2:
            EntityAi_StartAction(entity, StoneGuardian_Mode5ChaseInBounds, unknown);
            ai_state->sub_timer = entity->params->yaw_limit_deg;
            break;
        case 3:
            EntityAi_StartAction(entity, StoneGuardian_Mode5ReturnToSpawn, unknown);
            break;
        default:
            entity->state = 0;
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

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
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
        if (ABS(angle) > 1.5707964f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 2 in mode 5: as StoneGuardian_Mode3ChaseInBounds with its own bounds. */
static void StoneGuardian_Mode5ChaseInBounds(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (player_state.position[0] < -18300.0f) {
        ai_state->target_point[0] = -18300.0f;
        ai_state->target_point[1] = entity->model->position[1];
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = 600.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
            }
        }
    } else if (player_state.position[0] > -16050.0f) {
        ai_state->target_point[0] = -16050.0f;
        ai_state->target_point[1] = entity->model->position[1];
        ai_state->target_point[3] = 0.0f;
        ai_state->target_point[2] = 600.0f;
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > range) {
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &player_state;
            direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
            angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
            if (ABS(angle) > 1.5707964f) {
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
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The action of state 3 in mode 5: as StoneGuardian_Mode3ReturnToSpawn with its own bounds. */
static void StoneGuardian_Mode5ReturnToSpawn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (ai_state->sub_timer > 0) {
        ai_state->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
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
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))ai_state->move)(entity);
}

/* The think handler of mode 6: a sequence kept in work byte unk7A (1 to 7; at 0 it is
   EntityAi_Think), driven by whether the two map objects EntitySpawnInfo.variant and unk42 name
   both have bit 17 set. While both are set the entity is kept unseen at a fixed point (steps 1 and
   2); when one clears it comes out, one entity of the five in stone_guardian_spawn_ids at a time (step 3,
   StoneGuardian_PlayAnim9, then 0); when both are set again it goes back the same way (steps 4 to 7,
   StoneGuardian_PlayAnim8). The functions named Mode6... do the work of a step. */
static void StoneGuardian_ThinkMode6HideSequence(Entity *entity, s32 unknown) {
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
        if (EntityMath_DistanceXZ(entity->spawn_info, &player_state) > 5400.0f) {
            Vector_Copy(entity->model->position, entity->spawn_info);
            ai_state->step_index = 0;
        }
    }
    switch (ai_state->step_index) {
    case 1:
        if (!both_open) {
            ai_state->step_index++;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        } else {
            StoneGuardian_Mode6StayHidden(entity, unknown);
        }
        break;
    case 2:
        if (!both_open) {
            if (!StoneGuardian_Mode6IsStepFree(entity, 3) || !StoneGuardian_Mode6IsStepFree(entity, 6) || !StoneGuardian_Mode6IsStepFree(entity, 7)) {
                StoneGuardian_Mode6WaitTurnHidden(entity, unknown);
            } else {
                ai_state->step_index++;
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
            }
        } else {
            ai_state->step_index = 1;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        }
        break;
    case 3:
        if (!both_open) {
            StoneGuardian_Mode6LeaveHiding(entity, unknown);
        } else {
            ai_state->step_index = 7;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        }
        break;
    case 4:
        post[0] = -18450.0f;
        post[1] = 3190.0f;
        post[2] = 4900.0f;
        ai_state->pitch_speed = EntityMath_DistanceXZ(entity->model->position, post);
        ai_state->step_index++;
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        break;
    case 5:
        if (!both_open) {
            ai_state->step_index = 0;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        } else if (StoneGuardian_Mode6IsStepFree(entity, 3) && StoneGuardian_Mode6IsStepFree(entity, 6) && StoneGuardian_Mode6IsStepFree(entity, 7) &&
                   StoneGuardian_Mode6IsNearestWaiting(entity, ai_state->pitch_speed)) {
            ai_state->step_index++;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        } else {
            StoneGuardian_Mode6WaitTurnShown(entity, unknown);
        }
        break;
    case 6:
        if (!both_open) {
            ai_state->step_index = 0;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        } else {
            StoneGuardian_Mode6ReturnToHiding(entity, unknown);
        }
        break;
    case 7:
        if (!both_open) {
            ai_state->step_index = 3;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        } else {
            StoneGuardian_Mode6WalkToHidePoint(entity, unknown);
        }
        break;
    default:
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        entity->model_alpha = 0x80;
        if (both_open) {
            ai_state->step_index = 4;
            EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
        } else {
            EntityAi_Think(entity, unknown);
        }
        break;
    }
}

/* Step 1 of StoneGuardian_ThinkMode6HideSequence: keeps the entity unseen (Entity.model_alpha 0; flag bits 18, 20, 21,
   26, 27) at a fixed point. */
static void StoneGuardian_Mode6StayHidden(Entity *entity, s32 unknown) {
    EntityModel *model;

    model = entity->model;
    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->model_alpha = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->rotation[1] = 1.5707964f;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->ai_state.move, unknown);
}

/* Step 2 of StoneGuardian_ThinkMode6HideSequence: the same as StoneGuardian_Mode6StayHidden. What
   differs is when the think handler calls it: one of the two map objects has its bit clear, and
   another of the five entities is still at step 3, 6 or 7 (StoneGuardian_Mode6IsStepFree). */
static void StoneGuardian_Mode6WaitTurnHidden(Entity *entity, s32 unknown) {
    EntityModel *model = entity->model;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->model_alpha = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->rotation[1] = 1.5707964f;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->ai_state.move, unknown);
}

/* Step 3 of StoneGuardian_ThinkMode6HideSequence: shows the entity, walks it to a second fixed point, turns it
   to heading 90 degrees and starts StoneGuardian_PlayAnim9. */
static void StoneGuardian_Mode6LeaveHiding(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    entity->model_alpha = 0x80;
    ai_state->target_point[0] = -18050.0f;
    ai_state->target_point[1] = 3030.0f;
    ai_state->target_point[2] = 4900.0f;
    if (entity->model->position[1] > 3031.0f) {
        ai_state->step_index = 0;
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    } else {
        point = ai_state->target_point;
        if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
            ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
            angle = EntityMath_AngleDifference(entity->model->rotation[1], 1.5707964f);
            if (ABS(angle) > 0.17453292f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                EntityAi_StartAction(entity, StoneGuardian_PlayAnim9, unknown);
                return;
            }
        } else {
            entity->target = point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, range);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* An action (busy bit 0x1000) of mode 6, which StoneGuardian_Mode6LeaveHiding starts: plays
   animation 9 to its end with Entity.model_alpha at 0x80, then puts the sequence at 0 (from there the
   think handler is EntityAi_Think) and thinks. */
static void StoneGuardian_PlayAnim9(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    entity->model_alpha = 0x80;
    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ai_state->step_index = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
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

/* Step 5 of StoneGuardian_ThinkMode6HideSequence: the entity is seen (Entity.model_alpha 0x80, the flag
   bits of the other steps cleared) and stands, the move action with no command. The think handler
   calls it while both map objects have their bit set and it is not yet this entity's turn to go
   back: another one is at step 3, 6 or 7 (StoneGuardian_Mode6IsStepFree) or waits nearer
   (StoneGuardian_Mode6IsNearestWaiting). */
static void StoneGuardian_Mode6WaitTurnShown(Entity *entity, s32 unknown) {
    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->model_alpha = 0x80;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->ai_state.move, unknown);
}

/* Step 6 of StoneGuardian_ThinkMode6HideSequence: walks the entity to a third fixed point, turns it to heading
   -90 degrees and starts StoneGuardian_PlayAnim8. */
static void StoneGuardian_Mode6ReturnToHiding(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    entity->model_alpha = 0x80;
    ai_state->target_point[0] = -18450.0f;
    ai_state->target_point[1] = 3190.0f;
    ai_state->target_point[2] = 4900.0f;
    if (entity->model->position[1] <= 3030.0f) {
        ai_state->step_index = 7;
    } else {
        point = ai_state->target_point;
        if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], -1.5707964f);
            if (ABS(angle) > 0.17453292f) {
                if (angle >= 0.0f) {
                    entity->move_command = MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command = MOVE_COMMAND_TURN_LEFT;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                EntityAi_StartAction(entity, StoneGuardian_PlayAnim8, unknown);
                return;
            }
        } else {
            entity->target = point;
            entity->move_command = EntityAi_MoveCommand(entity, 0, range);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* An action (busy bit 0x1000) of mode 6, which StoneGuardian_Mode6ReturnToHiding starts: plays
   animation 8 to its end, then puts the sequence at 7 (StoneGuardian_Mode6WalkToHidePoint) and
   thinks. */
static void StoneGuardian_PlayAnim8(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    entity->model_alpha = 0x80;
    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ai_state->step_index = 7;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
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

/* Step 7 of StoneGuardian_ThinkMode6HideSequence: walks the entity back to the first fixed point, then puts the
   sequence at 1. */
static void StoneGuardian_Mode6WalkToHidePoint(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *point = ai_state->target_point;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
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
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
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
