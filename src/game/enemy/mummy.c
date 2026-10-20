/* Mummy: the behaviour of entity type 39 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Mummy. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
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
extern PlayerData player_state;
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionHeavy(Entity *);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_DistanceXZ(void *, void *);

static void Mummy_Think(Entity *entity, s32 unknown);
static void Mummy_ThinkCombat(Entity *entity, s32 unknown);
static void Mummy_OnHit(Entity *entity);
static s32 Mummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach);

/* Set-up of a type 39 entity: clears its work area and installs the type's handlers. */
void Mummy_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Mummy_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = Mummy_OnHit;
    ai_state->move = EntityAi_Move;
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
}

/* The think handler: as EntityAi_Think, with Mummy_ThinkCombat for state 2. Sets work byte
   unk7A once Entity.hit_points is below half of EntityParams.hit_points. */
static void Mummy_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->hit_points < entity->params->hit_points >> 1 && ai_state->step_index == 0) {
        ai_state->step_index = 1;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                Mummy_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but closing in runs once work byte unk7A
   is set. When the player is near, the entity is within 40 degrees of the player's facing and the
   player is 40 to 90 degrees off its heading, it goes backwards for 90 to 149 frames instead (work
   byte unk78 is 1), with the command Mummy_BackwardsCommand gives. */
static void Mummy_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;
    s32 mode;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
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
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
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
    if (ai_state->move_choice != 1) {
        angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
        if (entity->distance_to_player_3d - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                ai_state->move_choice = 1;
                entity->target = &player_state;
                entity->combat_choice = 1;
                entity->action_timer = rand() % 60 + 90;
            }
        }
    }
    mode = entity->combat_choice;
    if (mode == 1) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->turn_in_place == 0) {
                ai_state->turn_in_place = ABS(angle) > 0.5235988f;
                if (ai_state->turn_in_place == 0) {
                    goto done;
                }
            }
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                ai_state->turn_in_place = 0;
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            switch (ai_state->move_choice) {
            case 0:
                if (ai_state->step_index != 0) {
                    if (turn > 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
                    } else if (turn < 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
                    } else {
                        entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                    }
                } else {
                    if (turn > 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                    } else if (turn < 0) {
                        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                    } else {
                        entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                    }
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_LEFT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_RIGHT);
                } else {
                    // match: the caller passes an integer argument the callee does not take
                    entity->move_command = Mummy_BackwardsCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit forgets the target and starts a
   reaction (EntityAi_ReactionHeavy). */
static void Mummy_OnHit(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_LowerWeightsAfterHit((char *)entity);
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->target = NULL;
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The movement command for going backwards: a turn bit (0x100 / 0x200) while the target is off the heading by
   more than the turn limit, and command bit 2 (backwards) while it is farther than reach; 0 without a target.
   The second parameter is not used. */
static s32 Mummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach) {
    f32 turn;
    f32 limit;
    f32 heading;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    heading = EntityMath_HeadingTo(entity->model->position, entity->target);
    turn = EntityMath_AngleDifference(entity->model->rotation[1], heading);
    limit = entity->params->turn_limit;
    if (0.0f <= turn ? limit < turn : limit < -turn) {
        command = 0x100;
        if (0.0f <= turn) {
            command = 0x200;
        }
    }
    if (reach < EntityMath_DistanceXZ(entity->model->position, entity->target)) {
        command |= 2;
    }
    return command;
}
