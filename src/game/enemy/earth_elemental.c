/* EarthElemental: the behaviour of entity type 49 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Earth Elemental. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
extern PlayerData player_state;
int rand(void);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
float EntityMath_WrapAngle(float);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
extern u8 player_weapon_step[];
extern EntityModel *held_item_model_ptr;
float EntityMath_HeadingTo(float *, float *);
float EntityMath_DistanceXZ(float *a, float *b);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
/* "<< !!!??? >>", printed when unkCD is not 0 again after EntityAi_LowerChoiceWeight */
extern char earth_elemental_unexpected_message[];
int printf(const char *, ...);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void EntityAi_SetAttackDelay(Entity *, s32);

static void EarthElemental_Think(Entity *entity, s32 unknown);
static void EarthElemental_ThinkCombat(Entity *entity, s32 unknown);
static void EarthElemental_PlayTurnAnim(Entity *entity);
static s32 EarthElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle);
static s32 EarthElemental_ShouldGuard(Entity *entity, f32 max_distance, f32 max_angle);
static void EarthElemental_Move(Entity *entity);
static s32 EarthElemental_BackwardsCommand(Entity *entity, s32 unused, f32 reach);
static void EarthElemental_AlertSameKind(Entity *entity);
static void EarthElemental_OnHit(Entity *entity);
static void EarthElemental_BackAwayOrRunAtPlayer(Entity *entity);
static void EarthElemental_RangedAttack0(Entity *entity);

/* Set-up of a type 49 entity: clears its work area and installs the type's handlers. */
void EarthElemental_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = EarthElemental_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = (void (*)(void))EarthElemental_OnHit;
    ai_state->move = (void (*)(void))EarthElemental_Move;
    ai_state->dormant = (void (*)(void))EntityAi_Dormant;
    ai_state->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    ai_state->mode_timer = entity->params->guard_chance;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with EarthElemental_ThinkCombat for state 2; walking back to
   the spawn point (flag bit 53) gets command bits 0x440 added. Work byte unk7B is put to 25 to 34
   while player_state.unk259 is set and counts down; work word unk6C is EntityParams.far_distance while work
   word unk6E counts down and EntityParams.guard_chance otherwise. While work word unk68 is 1
   (EarthElemental_AlertSameKind of another entity sets it) it only starts EarthElemental_BackAwayOrRunAtPlayer. */
static void EarthElemental_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (player_state.magic_shot_launched != 0) {
        ai_state->action_flags = rand() % 10 + 25;
    }
    if (ai_state->action_flags > 0) {
        ai_state->action_flags--;
    }
    if (ai_state->reload_timer > 0) {
        ai_state->mode_timer = entity->params->far_distance;
        ai_state->reload_timer--;
    } else {
        ai_state->mode_timer = entity->params->guard_chance;
    }
    if (ai_state->pause_timer == 1) {
        if (entity->busy_flags == 0) {
            EntityAi_StartAction(entity, EarthElemental_BackAwayOrRunAtPlayer, 1);
        }
        return;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if ((s32)(entity->flags >> 53) & 1) {
            // match: called as returning nothing, so $2 is free for the next statement
            ((void (*)(void *, int))EntityAi_ThinkReturn)(entity, unknown);
            entity->move_command |= (MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_FAST);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                EarthElemental_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but it first starts EntityAi_Dodge when
   EarthElemental_ShouldDodgeMagic says so and guards when EarthElemental_ShouldGuard says so; an attack sets work word unk6E to
   EntityParams.near_distance. Closing in is one of three things: with the entity more than 35 degrees off
   the player's facing it goes backwards for 60 to 89 frames (work byte unk78 is 1, command from
   EarthElemental_BackwardsCommand); else it runs at the player for 60 to 119 frames (unk78 is 0); with the player
   more than 90 degrees off its heading it starts EarthElemental_PlayTurnAnim. Work word unk6A counts
   the frames it is not going for the player; above EntityParams.type_param_0, with the player within 30
   degrees ahead and nearer than 1000, it starts EarthElemental_RangedAttack0. */
static void EarthElemental_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    f32 player_angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    } else {
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
    }
    if (EarthElemental_ShouldDodgeMagic(entity, 200.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (EarthElemental_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    if (entity->action_timer == 0 || mode == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = mode;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->combat_choice) {
        case 0:
            ai_state->reload_timer = entity->params->near_distance;
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            player_angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
            if (ABS(player_angle) > 0.61086524f) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 60;
                ai_state->move_choice = 1;
            } else {
                entity->target = &player_state;
                if (1.5707964f < ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]))) {
                    ai_state->step_index = (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, EarthElemental_PlayTurnAnim, unknown);
                    return;
                }
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 60;
                ai_state->move_choice = 0;
            }
            break;
        case 2:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = 2;
            break;
        default:
            ai_state->move_choice = 2;
            break;
        }
    }
    if (ai_state->sub_timer > entity->params->type_param_0
        && ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < 0.5235988f
        && entity->distance_to_player < 1000.0f) {
        ai_state->sub_timer = 0;
        ENTITY_FLAG_BITS(entity)->bits58 = 0;
        EntityAi_StartAction(entity, EarthElemental_RangedAttack0, 1);
        return;
    }
    if (ai_state->sub_timer <= entity->params->type_param_0) {
        ai_state->sub_timer++;
    }
    if (entity->combat_choice == 1) {
        EntityAi_ClearTurnInPlace(entity);
        if ((s32)(entity->flags >> 31) & 1) {
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
            side = EntityAi_TurnFromPush(entity, direction);
            switch (ai_state->move_choice) {
            case 0:
                if (side > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
                } else if (side < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
                } else {
                    if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > 1.5707964f
                        && frame_counter % 15U == 0 && !(rand() & 1)) {
                        ai_state->step_index = (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f) ? 1 : -1;
                        EntityAi_StartAction(entity, EarthElemental_PlayTurnAnim, unknown);
                        return;
                    }
                    entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                ai_state->sub_timer = 0;
                break;
            case 1:
                if (side > 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_RIGHT);
                } else if (side < 0) {
                    entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_LEFT);
                } else {
                    if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > 1.5707964f
                        && frame_counter % 15U == 0 && !(rand() & 1)) {
                        ai_state->step_index = (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f) ? 1 : -1;
                        EntityAi_StartAction(entity, EarthElemental_PlayTurnAnim, unknown);
                        return;
                    }
                    // match: called like EntityAi_MoveCommand, with a mode the callee ignores
                    entity->move_command = EarthElemental_BackwardsCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                ai_state->sub_timer = 0;
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* An action (busy bit 0x2000): plays animation 12 or, with work byte unk7A below 1, animation 11 to
   its end. EarthElemental_ThinkCombat starts it when the player is more than 90 degrees off the
   heading. */
static void EarthElemental_PlayTurnAnim(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case 0:
        if (ai_state->step_index > 0) {
            Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 1);
        }
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* Whether to dodge a magic: only while work byte unk7B is 1 to 15 (EarthElemental_Think puts it to 25 to 34
   when the player casts one, player_state.unk259), with the player at least min_distance away (less the radius
   and 100), Entity.heading_to_player within max_angle of the heading and the entity within 30 degrees of the player's
   facing; then EntityParams.dodge_chance / 10 times in 100. */
static s32 EarthElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle) {
    f32 distance;
    f32 angle;
    f32 player_angle;

    if ((u8)(entity->ai_state.action_flags - 1) >= 15) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    player_angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (ABS(player_angle) > 0.5235988f) {
        return FALSE;
    }
    return rand() % 100 < entity->params->dodge_chance / 10;
}

/* As EntityAi_ShouldGuard, but by a chance of work word unk6C / 5 percent (EntityParams.far_distance for a while
   after an attack, EntityParams.guard_chance otherwise; EarthElemental_Think sets it). */
static s32 EarthElemental_ShouldGuard(Entity *entity, f32 max_distance, f32 max_angle) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 distance;
    f32 angle;

    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (entity->busy_flags != 0) {
        return FALSE;
    }
    if (distance > max_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (player_weapon_step[0] == 0 || held_item_model_ptr->level_tables->frame >= 6) {
        return FALSE;
    }
    return rand() % 100 < ai_state->mode_timer / 5;
}

/* The move handler: as EntityAi_Move, but it only turns the entity and picks the animation, it does
   not move it; going backwards is animation 18. */
static void EarthElemental_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 turn;

    if (entity == player_state.stood_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
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
    ai_state->move_speed = 0.0f;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_DOWN) {
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The movement command for going backwards: a turn bit (0x100 / 0x200) while the target is off the heading by
   more than the turn limit, and command bit 2 (backwards) while it is farther than reach; 0 without a target.
   The second parameter is not used. */
static s32 EarthElemental_BackwardsCommand(Entity *entity, s32 unused, f32 reach) {
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

/* Helper of EarthElemental_OnHit: sets work word unk68 to 1 in every other live entity with the same
   EntityParams that is within EntityParams.action_frames of this one, has unk68 at 0 and has the player
   within EntitySpawnInfo.wander_radius of its spawn point. */
static void EarthElemental_AlertSameKind(Entity *entity) {
    Entity *other;

    for (other = active_entities_head; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & ENTITY_FLAG_ACTIVE_LIST) && ((s32)(other->flags >> 1) & 1)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            EntityAiState *ai_state = &other->ai_state;

            if (ai_state->pause_timer == 0
                && EntityMath_DistanceXZ(entity->model->position, other->model->position)
                       < (s16)entity->params->action_frames
                && EntityMath_DistanceXZ((float *)other->spawn_info, player_state.position) < other->spawn_info->wander_radius) {
                ai_state->pause_timer = 1;
            }
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, then EarthElemental_AlertSameKind. */
static void EarthElemental_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
    EarthElemental_AlertSameKind(entity);
}

/* An action (busy bit 0x3000) that EarthElemental_Think starts while work word unk68 is 1
   (EarthElemental_AlertSameKind of another entity sets it). It starts no animation of its own. Each
   frame it dodges or guards (EarthElemental_ShouldDodgeMagic, EarthElemental_ShouldGuard, EntityAi_ShouldDodge), starts the attack
   EntityAi_PickAttack picks once EntityAi_PickCombatChoice chooses attacking (unk68 becomes 2), and else gives a
   command with the player as target and calls the move handler itself: backwards (bit 2) while the
   entity is more than 45 degrees off the player's facing, forwards at the run (0x41) while it is
   within. Flag bit 53 ends it with unk68 at 2. */
static void EarthElemental_BackAwayOrRunAtPlayer(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;

    entity->busy_flags |= (ENTITY_BUSY_1000 | ENTITY_BUSY_2000);
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (EarthElemental_ShouldDodgeMagic(entity, 200.0f, 1.0471976f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if (EarthElemental_ShouldGuard(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Guard, 1);
            break;
        }
        if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->pause_timer = 2;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        side = EntityAi_PickCombatChoice(entity, 0);
        ENTITY_FLAGS(entity) |= 0x200;
        if (side == 0) {
            entity->prev_combat_choice = entity->combat_choice;
            entity->combat_choice = 0;
            if (entity->prev_combat_choice != 0) {
                entity->choice_change_frame = frame_counter;
            }
            EntityAi_LowerChoiceWeight(entity, 0);
            if (entity->combat_choice == 0) {
                ai_state->pause_timer = 2;
                if (EntityAi_PickAttack((char *)entity)) {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
                }
            } else {
                printf(earth_elemental_unexpected_message);
            }
            break;
        }
        entity->target = &player_state;
        angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
        side = EntityAi_TurnFromPush(entity, direction);
        if (ABS(angle) > 0.7853982f) {
            entity->move_command = MOVE_COMMAND_BACK;
        } else {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN);
        }
        if (side > 0) {
            entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
        } else if (side < 0) {
            entity->move_command |= MOVE_COMMAND_TURN_LEFT;
        } else if (ABS(angle) > 0.7853982f) {
            // match: the caller passes a second integer argument the callee does not take
            entity->move_command = EarthElemental_BackwardsCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        } else {
            entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
        ((void (*)(Entity *))ai_state->move)(entity);
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23, as EntityAi_RangedAttack, but the
   shots are launched with EntityShot_Launch along the reverse of its heading. EarthElemental_ThinkCombat
   starts it. */
static void EarthElemental_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;
    f32 direction[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(direction, entity->model->rotation);
        direction[1] = EntityMath_WrapAngle(direction[1] - 3.1415927f);
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction);
        if (entity->model->level_tables->frame > 0 && unk3D8->unk03 >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}
