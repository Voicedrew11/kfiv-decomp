/* The talking characters: entity types 15 and 81. Every character a Recollection page describes has a chr.prm
   record of type 15 (from the game data). Type 15 turns to the player and watches it while scripts put it in
   states 4 to 6; type 81 only stands and walks back to its spawn point. */
#include "entity/npc.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_events.h"
#include "model/motion.h"
#include "entity/entity_math.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void Vector_Copy(void *, void *);
extern s32 entity_type_handlers[][2];
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern EntityGroupRecord entity_group_state[];
int EntityAi_SetReaction(Entity *entity, void (*reaction)(void), int run_now);
void EntityAi_SetState(char *, u8);
extern PlayerData player_state;
extern u8 map_script_flags[0x80][0x40];
int EntityAi_AnimationFrameCount(void *, int);
void EntityAi_SetAttackDelay(Entity *, s32);
extern char story_flags[0x40];
extern f32 identity_matrix[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Normalize(void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);
extern u32 frame_counter;
void EntityAi_SensePlayer(Entity *, s32);
void EntityAi_CheckFarFromSpawn(char *);

/* Motion_BlendToPose as if it returned a value.
   match: the callee is void, but retail keeps what it leaves in $2 */
#define MOTION_BLEND_TO_POSE ((int (*)(EntityModel *, f32))Motion_BlendToPose)

static void Npc_Animate(Entity *entity);
static void Npc_OnHit(Entity *entity);
static void Npc_ThinkStand(Entity *entity, int unknown);
static void Npc_ThinkReactToPlayer(Entity *entity, s32 run_now);
static void Npc_ThinkTurnToPlayer(Entity *entity, s32 run_now);
static void Npc_ThinkWatchPlayer(Entity *entity, int unknown);
static void Npc_ThinkStopWatching(Entity *entity, int unknown);
static void Npc_ThinkTurnBack(Entity *entity, s32 run_now);
static void Npc_TurnToPlayer(Entity *entity);
static void Npc_TurnToSpawnHeading(Entity *entity);
static void Npc_ReactionHeavy(Entity *entity);
static void Npc_Death(Entity *entity);
static void Npc_Move(Entity *entity);
static void Npc_OnHitType81(Entity *entity);
static void Npc_ThinkType81(Entity *entity, s32 run_now);
static int Npc_ThinkType81Stand(void *entity_ptr, int unknown);

/* The set-up function of entity type 15, the talking characters (chr.prm): as EntityAi_InitDefaults, with the
   Npc_ handlers, its look part from EntityParams.unk43C, and flag bit 16 set. */
void Npc_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    EntityModelPart80 *part;
    s32 i;

    part = (EntityModelPart80 *)entity->model->parts + entity->params->unk43C;
    entity->model->look_at_part = part;
    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = Npc_Think;
    unkE0->animate = (void (*)(void))Npc_Animate;
    unkE0->on_hit = (void (*)(void))Npc_OnHit;
    unkE0->move = (void (*)(void))Npc_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))Npc_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26) {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    if (part != NULL) {
        Vector_Copy(unkE0->look_part_angles, part->unk50);
    }
    unkE0->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    // match: called as returning an int, which keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))Motion_StepModel)(entity->model, 0, 0);
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
}

/* The animate handler of types 15 and 81: as EntityAi_Animate, but the blend runs at unkE0->unk40 only in a
   busy-bit-2 reaction, and the head turns at the rates EntityParams.action_frames (towards the target) and unk436. */
static void Npc_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->level_tables->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = MOTION_BLEND_TO_POSE(entity->model, (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? unkE0->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->look_part_angles, look_at_target, limits, ENTITY_CONTROLLER(entity)->action_frames);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->look_part_angles, 0, NULL, entity->params->near_distance);
        }
    }
}

/* The on_hit handler of type 15: lowers the weights of the last choice (EntityAi_LowerWeightsAfterHit),
   forgets the target, stops the head following it, starts Npc_ReactionHeavy and marks its group record
   (entity_group_state[EntityParams.unk43A].unk02). */
static void Npc_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->flags &= ~0x400;
    entity->target = NULL;
    entity->flags &= ~0x200;
    EntityAi_SetReaction(entity, (void (*)(void))Npc_ReactionHeavy, 0);
    entity_group_state[ENTITY_CONTROLLER(entity)->entity_group_index].unk02 = 1;
}

/* The think handler of type 15, while not busy: state 4 turns to the player, state 5 watches the player until
   it is farther than EntityParams.attack_range (then state 6), state 6 turns back; any other state stands. */
void Npc_Think(Entity *entity, s32 unknown) {
    if (entity->busy_flags == 0) {
        switch (entity->state) {
        case 4:
            if (entity->params->flags & 0x80) {
                Npc_ThinkTurnToPlayer(entity, unknown);
            } else {
                Npc_ThinkReactToPlayer(entity, unknown);
            }
            break;
        case 5:
            Npc_ThinkWatchPlayer(entity, unknown);
            if (entity->params->attack_range < entity->distance_to_player) {
                EntityAi_SetState((char *)entity, 6);
            }
            break;
        case 6:
            if (entity->params->flags & 0x80) {
                Npc_ThinkTurnBack(entity, unknown);
            } else {
                Npc_ThinkStopWatching(entity, unknown);
            }
            break;
        case 0:
        case 1:
        case 2:
        case 3:
        default:
            Npc_ThinkStand(entity, unknown);
            break;
        }
    }
}

/* The think function of states 0-3: the head stops following the target and the move action runs with no
   command (standing). */
static void Npc_ThinkStand(Entity *entity, int unknown) {
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->unkE0.move, unknown);
}

/* The think function of state 4 when EntityParams.flags has no 0x80: picks the animation by where the player
   is (26 within EntityParams.far_distance degrees of the heading, else 28 or 27 for the side), then state 5. */
static void Npc_ThinkReactToPlayer(Entity *entity, s32 run_now) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;
    f32 limit;
    f32 direction;

    direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
    limit = (f32)ENTITY_CONTROLLER(entity)->far_distance * 0.017453292f;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        if (0.0f <= angle) {
            unkE0->pause_timer = 0x4000;
        } else {
            unkE0->pause_timer = 0x2000;
        }
    } else {
        unkE0->pause_timer = 0x1000;
    }
    entity->move_command = unkE0->pause_timer;
    EntityAi_SetState((char *)entity, 5);
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, run_now);
}

/* The think function of state 4 when EntityParams.flags has 0x80: turns to the player (Npc_TurnToPlayer) when
   it is more than EntityParams.far_distance degrees off the heading, else animation 26 and state 5. */
static void Npc_ThinkTurnToPlayer(Entity *entity, s32 run_now) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;
    f32 limit;
    f32 direction;

    direction = EntityMath_HeadingTo(entity->model->position, player_state.position);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], direction);
    limit = (f32)ENTITY_CONTROLLER(entity)->far_distance * 0.017453292f;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        EntityAi_StartAction(entity, Npc_TurnToPlayer, run_now);
    } else {
        unkE0->pause_timer = 0x1000;
        entity->move_command = 0x1000;
        EntityAi_SetState((char *)entity, 5);
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, run_now);
    }
}

/* The think function of state 5: the head follows the target (flag bit 9) and the animation picked in state 4
   goes on. */
static void Npc_ThinkWatchPlayer(Entity *entity, int unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u16 timer = unkE0->pause_timer;

    ENTITY_FLAGS(entity) |= 0x200;
    entity->move_command = timer;
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 6 when EntityParams.flags has no 0x80: the head stops following the target,
   state 0. */
static void Npc_ThinkStopWatching(Entity *entity, int unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    unkE0->pause_timer = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_SetState((char *)entity, 0);
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 6 when EntityParams.flags has 0x80: turns back to the spawn heading
   (Npc_TurnToSpawnHeading) when more than EntityParams.far_distance degrees off it, else as Npc_ThinkStopWatching. */
static void Npc_ThinkTurnBack(Entity *entity, s32 run_now) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;
    f32 limit;

    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
    limit = (f32)ENTITY_CONTROLLER(entity)->far_distance * 0.017453292f;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        EntityAi_StartAction(entity, Npc_TurnToSpawnHeading, run_now);
    } else {
        unkE0->pause_timer = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        EntityAi_SetState((char *)entity, 0);
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, run_now);
    }
}

/* An action (busy bit 0x1000): plays the turning animation for the side the player is on (27 or 28; 30 or 31
   in spawn mode 1 when its map_script_flags entry is set) and turns the body to the heading towards the player
   (Entity.heading_to_player). */
static void Npc_TurnToPlayer(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 yaw;
    f32 angle;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->variant][entity->spawn_info->map_object_id] == entity->spawn_info->second_map_object_id) {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1F, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1E, 0, entity->params->motion_blend_frames, 0);
            }
        } else {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1C, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1B, 0, entity->params->motion_blend_frames, 0);
            }
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_timer = EntityAi_AnimationFrameCount(entity, entity->animation_index);
            entity->action_step++;
        }
        break;
    case 2:
        yaw = entity->model->rotation[1];
        EntityMath_EaseAngleToward(&yaw, entity->heading_to_player, entity->action_timer);
        entity->turn_delta_yaw += yaw - entity->model->rotation[1];
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000): as Npc_TurnToPlayer, towards the heading in the spawn record
   (EntitySpawnInfo.spawn_rotation_y). */
static void Npc_TurnToSpawnHeading(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 yaw;
    f32 angle;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->spawn_info->spawn_rotation_y);
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->variant][entity->spawn_info->map_object_id] == entity->spawn_info->second_map_object_id) {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1F, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1E, 0, entity->params->motion_blend_frames, 0);
            }
        } else {
            if (0.0f <= angle) {
                Motion_Start(entity, 0x1C, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 0x1B, 0, entity->params->motion_blend_frames, 0);
            }
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_timer = EntityAi_AnimationFrameCount(entity, entity->animation_index);
            entity->action_step++;
        }
        break;
    case 2:
        yaw = entity->model->rotation[1];
        EntityMath_EaseAngleToward(&yaw, entity->spawn_info->spawn_rotation_y, entity->action_timer);
        entity->turn_delta_yaw += yaw - entity->model->rotation[1];
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The reaction to every hit (busy bit 4): as EntityAi_ReactionHeavy, with animation 18 instead of 15 in spawn
   mode 1 when its map_script_flags entry is set. */
static void Npc_ReactionHeavy(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParams *params;
    s32 kind;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case 0:
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->variant][entity->spawn_info->map_object_id] == entity->spawn_info->second_map_object_id) {
            Motion_Start(entity, 0x12, 0, 5, 1);
        } else {
            Motion_Start(entity, 0xF, 0, 5, 1);
        }
        params = entity->params;
        if (params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            kind = 1;
            if (ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_1) {
                kind = 2;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        unkE0->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_WALK) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->reaction_step++;
        break;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->reaction_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
            EntityAi_SetReaction(entity, NULL, 0);
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The death action of types 15 and 81 (busy bit 0x40): as EntityAi_Death (animation 19 instead of 16 in spawn
   mode 1 when its map_script_flags entry is set); also writes into its group record whether the player killed it
   (1) or not (2), and sets story_flags[13] when the player did. */
static void Npc_Death(Entity *entity) {
    EntityParams *params;
    s32 kind;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        if (entity->spawn_info->spawn_mode == 1
            && map_script_flags[entity->spawn_info->variant][entity->spawn_info->map_object_id] == entity->spawn_info->second_map_object_id) {
            Motion_Start(entity, 0x13, 0, 5, 1);
        } else {
            Motion_Start(entity, 0x10, 0, 5, 1);
        }
        params = entity->params;
        if (params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            kind = 1;
            if (ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_1) {
                kind = 2;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->action_timer = 20;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            EntityEvent_OnDeath(entity);
            entity_group_state[ENTITY_CONTROLLER(entity)->entity_group_index].unk00 = ((s32)(entity->flags >> 38) & 3) ? 1 : 2;
            if ((s32)(entity->flags >> 38) & 3) {
                story_flags[0xD] = 1;
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}

/* The move action of types 15 and 81: as EntityAi_Move for forward and back only; plays animation 17 in spawn
   mode 1 when its map_script_flags entry is set, and animations 26-28 for the commands 0x1000, 0x2000 and 0x4000. */
static void Npc_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 amount;

    turn = 0.0f;
    if (entity->move_command & 0x100) {
        unkE0->turn_rate -= entity->params->turn_step_per_frame;
        if (unkE0->turn_rate < -entity->params->turn_limit) {
            unkE0->turn_rate = -entity->params->turn_limit;
        }
    } else if (entity->move_command & 0x200) {
        unkE0->turn_rate += entity->params->turn_step_per_frame;
        if (entity->params->turn_limit < unkE0->turn_rate) {
            unkE0->turn_rate = entity->params->turn_limit;
        }
    } else {
        unkE0->turn_rate = turn;
    }
    turn = unkE0->turn_rate;
    if (entity->move_command & 0x400) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (ENTITY_FLAG_BITS(entity)->bits11 == 0) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
        if (entity->move_command & 3) {
            amount = 0.0f;
            if (entity->move_command & 1) {
                amount = -1.0f;
            } else if (entity->move_command & 2) {
                amount = 1.0f;
            }
            vector[2] += amount;
        }
        if (entity->move_command & 3) {
            unkE0->move_speed = EntityAi_Accelerate(entity, unkE0->move_speed);
            amount = unkE0->move_speed;
            if (entity->move_command & 0x40) {
                amount += amount;
            }
            sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, identity_matrix, entity->model->rotation[2]);
            sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[0]);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[1]);
            Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
            entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
            entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
        } else {
            unkE0->move_speed = 0.0f;
        }
    } else {
        unkE0->move_speed = 0.0f;
    }
    if (entity->spawn_info->spawn_mode == 1
        && map_script_flags[entity->spawn_info->variant][entity->spawn_info->map_object_id] == entity->spawn_info->second_map_object_id) {
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 1) {
        if (entity->move_command & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & 2) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 0x1000) {
        Motion_Start(entity, 0x1A, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 0x2000) {
        Motion_Start(entity, 0x1B, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 0x4000) {
        Motion_Start(entity, 0x1C, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 0x300) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The set-up function of entity type 81: as Npc_Init, with Npc_ThinkType81 and Npc_OnHitType81. */
void Npc_InitType81(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    EntityModelPart80 *part;
    s32 i;

    part = (EntityModelPart80 *)entity->model->parts + entity->params->unk43C;
    entity->model->look_at_part = part;
    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = (void (*)(Entity *, s32))Npc_ThinkType81;
    unkE0->animate = (void (*)(void))Npc_Animate;
    unkE0->on_hit = (void (*)(void))Npc_OnHitType81;
    unkE0->move = (void (*)(void))Npc_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))Npc_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26) {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    if (part != NULL) {
        Vector_Copy(unkE0->look_part_angles, part->unk50);
    }
    unkE0->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    // match: called as returning an int, which keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))Motion_StepModel)(entity->model, 0, 0);
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
}

/* The on_hit handler of type 81: as Npc_OnHit, and sets flag bit 29 when the player dealt the hit (flag bits
   38-39). */
static void Npc_OnHitType81(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    EntityAi_SetReaction(entity, (void (*)(void))Npc_ReactionHeavy, 0);
    entity_group_state[ENTITY_CONTROLLER(entity)->entity_group_index].unk02 = 1;
    if (((u64)entity->flags >> 38) & 3) {
        entity->flags |= 0x20000000;
    }
}

/* The think handler of type 81: clears flag bit 29, updates what it knows of the player every tenth frame,
   walks back while flag bit 53 is set, else stands. */
static void Npc_ThinkType81(Entity *entity, s32 run_now) {
    s32 mask = ~0x20000000;

    entity->flags &= mask;
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    if ((s32)(entity->flags >> 53) & 1) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    Npc_ThinkType81Stand(entity, run_now);
}

/* The think function of type 81 that stands: as Npc_ThinkStand. */
static int Npc_ThinkType81Stand(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;

    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->unkE0.move, unknown);
}
