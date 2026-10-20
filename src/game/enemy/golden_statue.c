/* GoldenStatue: the behaviour of entity type 29 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Golden Statue. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "entity/entity_world.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Think(Entity *, s32);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
extern void EntityAi_ReactionHeavy(Entity *);
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_DistanceXZ(void *, void *);
s32 EntityAi_CanAttack(Entity *);
s32 EntitySight_EntityInWay(Entity *);
int EntityAi_PickAttack(char *);
float EntityMath_AngleDifference(float, float);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int EntityAi_ThinkReturn(void *, int);
void EntityAi_StepState(void *);
void EntityAi_CheckFarFromSpawn(void *);
extern PlayerData player_state;
extern char golden_statue_arrived_message[];
int printf(const char *, ...);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_CanShoot(Entity *);
s32 EntityAi_CanMelee(Entity *);
s32 EntityAi_PickRangedAttack(Entity *);
s32 EntityAi_PickMeleeAttack(Entity *);
void EntityAi_ClearTurnInPlace(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
extern int entity_under_player[];
int Motion_Start(void *, int, int, u32, int);
double atan2(double, double);
float EntityMath_WrapAngle(float);
float EntityMath_EaseAngleToward(float *, float, float);
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);

static void GoldenStatue_OnHit(Entity *entity);
static void GoldenStatue_ThinkWaitAtPost(Entity *entity, s32 unknown);
static void GoldenStatue_WalkToPost(Entity *entity, s32 unknown);
static void GoldenStatue_TurnAtPost(Entity *entity, s32 unknown);
static void GoldenStatue_StandAtPost(Entity *entity, s32 unknown);
static void GoldenStatue_ThinkNoStates(Entity *entity, s32 unknown);
static void GoldenStatue_ThinkDecideHoldPostOrCloseIn(Entity *entity, s32 unknown);
static void GoldenStatue_Move(Entity *entity);
static void GoldenStatue_AimPart15AbovePlayer(Entity *entity);
static void GoldenStatue_Animate(Entity *entity);

/* Set-up of a type 29 entity: clears its work area and installs the type's handlers. */
void GoldenStatue_Init(Entity *entity) {
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
    ai_state->animate = GoldenStatue_Animate;
    ai_state->on_hit = GoldenStatue_OnHit;
    ai_state->move = GoldenStatue_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    if (entity->spawn_info->spawn_mode == 1) {
        ai_state->think = GoldenStatue_ThinkWaitAtPost;
    } else if (entity->spawn_info->spawn_mode == 2) {
        ai_state->think = GoldenStatue_ThinkNoStates;
    } else {
        ai_state->think = EntityAi_Think;
    }
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

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void GoldenStatue_OnHit(Entity *entity) {
    u64 flags;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    flags = entity->flags;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler of mode 1 (EntitySpawnInfo.spawn_mode is 1): no states. Once the entity
   Entity_GetBySpawnIdForMap finds for spawn 0x29 has flag bit 0 set and its flags & 0xE are not 2,
   it puts GoldenStatue_ThinkNoStates in the think slot and calls it. Until then, while it is not busy,
   it calls GoldenStatue_WalkToPost, GoldenStatue_TurnAtPost or GoldenStatue_StandAtPost, picked by work
   bytes unk78 and unk79. */
static void GoldenStatue_ThinkWaitAtPost(Entity *entity, s32 unknown) {
    Entity *other;
    EntityAiState *ai_state = &entity->ai_state;
    u32 phase;

    other = Entity_GetBySpawnIdForMap(entity->group_id, 0x29);
    if (other != NULL && (s32)other->flags & ENTITY_FLAG_ACTIVE_LIST) {
        if ((other->flags & (ENTITY_FLAG_LIVE | ENTITY_FLAG_DYING_MASK)) != 2) {
            void (*handler)(Entity *, s32) = GoldenStatue_ThinkNoStates;

            ai_state->think = handler;
            handler(entity, unknown);
            return;
        }
    }
    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        if (ai_state->move_choice == 0) {
            GoldenStatue_WalkToPost(entity, unknown);
        } else {
            if (ai_state->turn_in_place != 0) {
                GoldenStatue_TurnAtPost(entity, unknown);
            } else {
                GoldenStatue_StandAtPost(entity, unknown);
            }
        }
    }
}

/* Helper of GoldenStatue_ThinkWaitAtPost, called while work byte unk78 is 0: walks to the point (-7900,
   spawn y, 3050), sets unk78 within 50 of it, and starts the move action. */
static void GoldenStatue_WalkToPost(Entity *entity, s32 unknown) {
    f32 point[4];
    f32 limit;
    EntityAiState *ai_state;
    f32 *target;

    limit = 50.0f;
    target = entity->ai_state.target_point;
    ai_state = &entity->ai_state;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    point[0] = -7900.0f;
    point[1] = entity->spawn_info->spawn_y;
    point[2] = 3050.0f;
    point[3] = 0;
    Vector_Copy(target, point);
    entity->target = target;
    entity->move_command = EntityAi_MoveCommand(entity, 0, 25.0f);
    if (EntityMath_DistanceXZ(entity->model->position, point) < limit) {
        ai_state->move_choice = 1;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* Helper of GoldenStatue_ThinkWaitAtPost, called while work bytes unk78 and unk79 are set: clears unk78
   when the entity is more than 100 from the point (-7900, spawn y, 3050); starts the attack
   EntityAi_PickAttack picks when EntityAi_CanAttack says so and EntitySight_EntityInWay does not; else turns on the spot
   to the heading in work float unk48, clears unk79 once it is within the turn limit of it, and
   starts the move action. */
static void GoldenStatue_TurnAtPost(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 home[4];
    f32 angle;
    f32 range = 100.0f;

    home[0] = -7900.0f;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = 3050.0f;
    home[3] = 0.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) > range) {
        ai_state->move_choice = 0;
    }
    if (EntityAi_CanAttack(entity) && !EntitySight_EntityInWay(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    angle = EntityMath_AngleDifference(entity->model->rotation[1], ai_state->part_angle_x);
    if (ABS(angle) > entity->params->turn_limit) {
        if (angle >= 0.0f) {
            entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
        } else {
            entity->move_command |= MOVE_COMMAND_TURN_LEFT;
        }
    } else {
        ai_state->turn_in_place = 0;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* Helper of GoldenStatue_ThinkWaitAtPost, called while work byte unk78 is set and unk79 is 0: attacks as
   GoldenStatue_TurnAtPost does; else, when Entity.heading_to_player is more than 30 degrees off its
   heading, keeps it in work float unk48 and sets unk79; starts the move action. */
static void GoldenStatue_StandAtPost(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    if (EntityAi_CanAttack(entity) && !EntitySight_EntityInWay(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (ABS(angle) > 0.5235988f) {
        ai_state->part_angle_x = entity->heading_to_player;
        ai_state->turn_in_place = 1;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think handler of mode 2 (EntitySpawnInfo.spawn_mode is 2), which GoldenStatue_ThinkWaitAtPost also
   installs: as EntityAi_Think without the states; while flag bit 53 is clear it calls
   GoldenStatue_ThinkDecideHoldPostOrCloseIn. */
static void GoldenStatue_ThinkNoStates(Entity *entity, s32 unknown) {
    u32 phase;

    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) != ENTITY_PARAMS_MOVEMENT_FIXED) {
        EntityAi_CheckFarFromSpawn(entity);
    }
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    GoldenStatue_ThinkDecideHoldPostOrCloseIn(entity, unknown);
}

/* Decides for GoldenStatue_ThinkNoStates: guards or dodges the player's swing; while flag bit 32 is
   set starts EntityAi_RangedAttack (EntityAi_CanShoot, EntityAi_PickRangedAttack) with the player farther from the
   spawn point than EntitySpawnInfo.wander_radius and EntityAi_MeleeAttack (EntityAi_CanMelee, EntityAi_PickMeleeAttack)
   with the player nearer. While the player's x is below -7900 it goes to one of three posts at x
   -8000, picked by the player's z, and from there turns to the player and starts the ranged attack
   only (work byte unk78 says it is at the post; on arriving it prints golden_statue_arrived_message, "tuitayo",
   Japanese for "arrived"); else it closes in on the player as EntityAi_ThinkCombat does. */
static void GoldenStatue_ThinkDecideHoldPostOrCloseIn(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 post[4];
    f32 direction[4];
    f32 angle;
    f32 chase_angle;
    f32 margin = 0.0f;
    Entity *other;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (EntityMath_DistanceXZ(entity->spawn_info, &player_state) > entity->spawn_info->wander_radius) {
        if (ENTITY_FLAG_BITS(entity)->player_in_reach) {
            if (EntityAi_CanShoot(entity) && !EntitySight_EntityInWay(entity)) {
                EntityAi_PickRangedAttack(entity);
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                return;
            }
        }
    } else if (ENTITY_FLAG_BITS(entity)->player_in_reach) {
        if (EntityAi_CanMelee(entity) && !EntitySight_EntityInWay(entity)) {
            EntityAi_PickMeleeAttack(entity);
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            return;
        }
    }
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (player_state.position[0] < -7900.0f) {
        Vector_Copy(post, &player_state);
        post[0] = -8000.0f;
        if (post[2] > 3350.0f) {
            post[2] = 3550.0f;
        } else if (post[2] < 2750.0f) {
            post[2] = 2550.0f;
        } else {
            post[2] = 3050.0f;
            other = Entity_GetBySpawnId(0x28);
            if (ENTITY_FLAG_BITS(other)->dying == 0) {
                other = Entity_GetBySpawnId(0x28);
                if (ENTITY_FLAG_BITS(other)->live) {
                    margin = entity->radius + entity->radius;
                }
            }
        }
        if (ai_state->move_choice == 0) {
            if (EntityMath_DistanceXZ(entity->model->position, post) < margin + 60.0f) {
                ai_state->move_choice = 1;
                printf(golden_statue_arrived_message);
            } else {
                entity->target = post;
                turn = EntityAi_TurnFromPush(entity, direction);
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
                }
                EntityAi_StartAction(entity, ai_state->move, unknown);
                return;
            }
        } else {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (EntityMath_DistanceXZ(entity->model->position, post) > margin + 150.0f) {
                entity->target = post;
                ai_state->move_choice = 0;
                return;
            }
            if (ENTITY_FLAG_BITS(entity)->player_in_reach && EntityAi_CanShoot(entity) && !EntitySight_EntityInWay(entity)) {
                EntityAi_PickRangedAttack(entity);
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                return;
            }
            if (ABS(angle) > 0.5235988f) {
                if (angle >= 0.0f) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                }
                EntityAi_StartAction(entity, ai_state->move, unknown);
                return;
            }
        }
    } else {
        entity->target = &player_state;
    }
    EntityAi_ClearTurnInPlace(entity);
    if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
        chase_angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (ai_state->turn_in_place == 0) {
            ai_state->turn_in_place = ABS(chase_angle) > 0.5235988f;
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
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: turns as the command says and does not move the entity itself; picks animation
   4 or 3 going forward, 11 or 12 for a turn on the spot, else 0. */
static void GoldenStatue_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 turn;

    if (entity == (Entity *)entity_under_player[0] && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
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
        ai_state->turn_rate = 0.0f;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* Helper of GoldenStatue_Animate: adds work float unk4C to the x rotation of model part 15. Before
   frame 43 unk4C goes towards the pitch to a point 400 above the player (EntityMath_EaseAngleToward, rate 10);
   from frame 54 it is scaled by (67 - frame) / 14. */
static void GoldenStatue_AimPart15AbovePlayer(Entity *entity) {
    EntityModel *model;
    EntityAiState *ai_state;
    EntityModelPart80 *part;
    s16 frame;
    f32 point[2];
    f32 pitch;

    model = entity->model;
    part = &((EntityModelPart80 *)model->parts)[15];
    ai_state = &entity->ai_state;
    if (part != NULL) {
        frame = model->level_tables->frame;
        if (frame < 43) {
            Vector_Copy(point, player_state.position);
            point[1] += 400.0f;
            pitch = EntityMath_WrapAngle(atan2(point[1] - model->position[1], entity->distance_to_player));
            EntityMath_EaseAngleToward(&ai_state->part_angle_y, pitch, 10.0f);
        } else if (frame >= 54) {
            ai_state->part_angle_y *= (f32)(67 - frame) / 14.0f;
        }
        part->part_angles[0] += ai_state->part_angle_y;
    }
}

/* The animate handler: as EntityAi_Animate; during animation 23 of an attack (busy bit 1) it also
   calls GoldenStatue_AimPart15AbovePlayer, and otherwise clears work float unk4C. */
static void GoldenStatue_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FIXED) {
            entity->flags = flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        }
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->look_at_target;
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
        if ((entity->busy_flags & ENTITY_BUSY_ATTACK) && entity->animation_index == 0x17) {
            GoldenStatue_AimPart15AbovePlayer(entity);
        } else {
            ai_state->part_angle_y = 0.0f;
        }
    }
}
