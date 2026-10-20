/* Seafang: the behaviour of entity type 28 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Seafang. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
void EntityAi_OnHit(Entity *);
void EntityAi_Dormant(Entity *);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
// match: const, so that the store to unk48 can follow the load of it
extern const f32 seafang_no_floor_height_think[];
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn3D(char *);
void EntityAi_StepState(void *);
f32 EntityCollision_DistanceToGround(f32 *);
int EntityAi_ThinkReturn(void *, int);
int rand(void);
void EntityPoint_PickNearWithHeight(f32 *, f32, f32, void *, void *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand3D(Entity *, s32, f32);
extern float player_state[];
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void Vector_Copy(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
// match: const, so that the store to unk48 can follow the load of it
extern const f32 seafang_no_floor_height_lower[];
void EntityPoint_PickNear(float, void *, void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
int Motion_Start(void *, int, int, u32, int);
float EntityMath_EaseAngleToward(float *, float, float);
// match: const, so that the store to unk48 can follow the load of it
extern const f32 seafang_no_floor_height_death[];
void EntityEvent_OnDeath(Entity *);

static void Seafang_Think(Entity *entity, s32 unknown);
static void Seafang_ThinkIdle(Entity *entity, s32 unknown);
static void Seafang_ThinkCombat(Entity *entity, s32 unknown);
static void Seafang_ThinkLowerGradually(Entity *entity, s32 unknown);
static void Seafang_ThinkWander(Entity *entity, s32 unknown);
static void Seafang_ThinkCombatRunAtPlayer(Entity *entity, s32 unknown);
static void Seafang_Move(Entity *entity);
static void Seafang_Death(Entity *entity);
static void Seafang_DeathWithoutDrop(Entity *entity);

/* Set-up of a type 28 entity: clears its work area and installs the type's handlers. */
void Seafang_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    if (entity->spawn_info->spawn_mode != 0) {
        ai_state->think = Seafang_ThinkLowerGradually;
    } else {
        ai_state->think = Seafang_Think;
    }
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = Seafang_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = Seafang_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
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
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think with the type's own functions for the states. Before that it
   reads EntityCollision_DistanceToGround at the entity's position into work float unk48, moves the entity in y by it,
   and starts Seafang_DeathWithoutDrop when it is above 0. */
static void Seafang_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn3D((char *)entity);
        EntityAi_StepState(entity);
        ai_state->part_angle_x = EntityCollision_DistanceToGround(entity->model->position);
        if (ai_state->part_angle_x != seafang_no_floor_height_think[0] && -entity->collision_height < ai_state->part_angle_x) {
            entity->model->position[1] += entity->collision_height + ai_state->part_angle_x;
            if (ai_state->part_angle_x > 0.0f) {
                EntityAi_StartAction(entity, Seafang_DeathWithoutDrop, 1);
                return;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                Seafang_ThinkCombat(entity, unknown);
                break;
            case 0:
            default:
                Seafang_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 0: as EntityAi_ThinkIdle, with EntityPoint_PickNearWithHeight picking the point to go
   to and EntityAi_MoveCommand3D the command. */
static void Seafang_ThinkIdle(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (entity->action_timer == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNearWithHeight(ai_state->target_point, entity->spawn_info->wander_radius, entity->spawn_info->wander_height_range, entity->spawn_info, entity->model->position);
            entity->target = ai_state->target_point;
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
        entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of states 1 and 2: as EntityAi_ThinkCombat, but it closes in, at the run, on a
   point off the player in y (by work float unk4C), and its melee attack is always record 0. */
static void Seafang_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
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
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                ENTITY_FLAG_BITS(entity)->ranged_attack = 0;
                ENTITY_FLAG_BITS(entity)->attack_index = 0;
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = player_state;
            ai_state->part_angle_y = entity->collision_radius + rand() % 150;
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
    if (ai_state->move_choice == 0) {
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
        } else {
            Vector_Copy(ai_state->target_point, player_state);
            ai_state->target_point[1] -= ai_state->part_angle_y;
            entity->target = ai_state->target_point;
            entity->move_command = EntityAi_MoveCommand3D(entity, 1, entity->radius + 100.0f + 30.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think handler of a spawn whose EntitySpawnInfo.spawn_mode is set: as Seafang_Think, but where
   that one puts the entity at once at Entity.collision_height past the surface EntityCollision_DistanceToGround finds, this one
   moves it 5 a frame in y (y grows downward: EntityCollision_DistanceToPropFloor measures the floor below as a larger y);
   and it has its own functions for the states (Seafang_ThinkWander,
   Seafang_ThinkCombatRunAtPlayer). Starts Seafang_DeathWithoutDrop when EntityCollision_DistanceToGround
   reads above 0, as Seafang_Think does. */
static void Seafang_ThinkLowerGradually(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn3D((char *)entity);
        EntityAi_StepState(entity);
        ai_state->part_angle_x = EntityCollision_DistanceToGround(entity->model->position);
        if (ai_state->part_angle_x != seafang_no_floor_height_lower[0] && -entity->collision_height < ai_state->part_angle_x) {
            entity->model->position[1] += 5.0f;
            if (ai_state->part_angle_x > 0.0f) {
                EntityAi_StartAction(entity, Seafang_DeathWithoutDrop, 1);
                return;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                Seafang_ThinkCombatRunAtPlayer(entity, unknown);
                break;
            case 0:
            default:
                Seafang_ThinkWander(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 0 for Seafang_ThinkLowerGradually: wanders, as
   EntityAi_ThinkIdle. Each time the timer runs out it picks a point near the spawn point
   (EntityPoint_PickNear; 79 times in 100, for 120 to 179 frames) or no target (60 to 89 frames), takes the
   command of EntityAi_MoveCommand towards it (or the turn away from a push, EntityAi_TurnFromPush) and starts the
   move action. */
static void Seafang_ThinkWander(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (entity->action_timer == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNear(entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
            entity->target = ai_state->target_point;
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
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of states 1 and 2 for Seafang_ThinkLowerGradually: as
   EntityAi_ThinkCombat, closing in at the run (command bit 0x40) on the player's own position,
   where Seafang_ThinkCombat goes for a point off the player in y; the melee attack is record 0
   while work float unk48 is below -80. */
static void Seafang_ThinkCombatRunAtPlayer(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
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
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                if (ai_state->part_angle_x < -80.0f) {
                    ENTITY_FLAG_BITS(entity)->ranged_attack = 0;
                    ENTITY_FLAG_BITS(entity)->attack_index = 0;
                }
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
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
    if (ai_state->move_choice == 0) {
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: turns the entity and pitches it down or up as the command says,
   and picks the animation. */
static void Seafang_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 value;
    f32 pitch;
    f32 rate;

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
    value = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        value += value;
    }
    entity->turn_delta_yaw += value;
    if (entity->move_command & MOVE_COMMAND_DOWN) {
        pitch = -DEG_TO_RAD(30);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        pitch = DEG_TO_RAD(30);
    } else {
        pitch = 0.0f;
    }
    if (ABS(pitch - entity->model->rotation[0]) > DEG_TO_RAD(30)) {
        rate = 60.0f;
    } else {
        rate = 30.0f;
    }
    value = entity->model->rotation[0];
    EntityMath_EaseAngleToward(&value, pitch, rate);
    entity->turn_delta_pitch += value - entity->model->rotation[0];
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_DOWN) {
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death; until the fade starts it also moves the
   entity 2 a frame in y while EntityCollision_DistanceToGround reads below 0. */
static void Seafang_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 fall = 0.0f;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    ai_state->part_angle_x = EntityCollision_DistanceToGround(entity->model->position);
    if (ai_state->part_angle_x != seafang_no_floor_height_death[0] && ai_state->part_angle_x < fall) {
        fall = 2.0f;
    }
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags = (entity->flags & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_DYING_1;
        entity->flags |= ENTITY_FLAG_NO_PLAYER_COLLISION;
        entity->flags |= ENTITY_FLAG_NO_ENTITY_COLLISION;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = 0;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        fall = 0.0f;
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= ENTITY_FLAG_DYING_MASK;
            entity->flags &= ~ENTITY_FLAG_LIVE;
        }
        break;
    }
    entity->move_delta_y -= fall;
}

/* A second death action (busy bit 0x40), which the think handlers start when EntityCollision_DistanceToGround reads
   above 0: animation 16, the fade and the removal as in Seafang_Death, but with no movement in
   y and without EntityEvent_OnDeath, which is what drops the gold (Gold_DropFromEntity), frees the
   effects and starts the spawn's script when an entity dies. */
static void Seafang_DeathWithoutDrop(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FLY) {
            entity->collision_radius = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->animates = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->action_timer = 20;
            ENTITY_FLAG_BITS(entity)->animates = 0;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = 0;
        }
        break;
    }
}
