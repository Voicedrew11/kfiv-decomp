/* AncientMummy: the behaviour of entity type 42 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Ancient Mummy. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
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
float EntityMath_HeadingTo(float *origin, float *target);
float EntityMath_DistanceXZ(float *a, float *b);
extern f32 D_00446810[];
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityPoint_PickNearFromDistance(f32 *, f32 *, f32, f32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
float EntityMath_WrapAngle(float);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetState(char *, u8);

static void AncientMummy_Think(Entity *entity, s32 unknown);
static void AncientMummy_ThinkCombat(Entity *entity, s32 unknown);
static s32 AncientMummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach);
static void AncientMummy_ShootAndRelocate(Entity *entity);
static void AncientMummy_FadeBackIn(Entity *entity);

/* Set-up of a type 42 entity: clears its work area and installs the type's handlers. */
void AncientMummy_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = AncientMummy_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(unkE0->look_part_angles, unk138 + 0x50);
    }
    unkE0->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with AncientMummy_ThinkCombat for state 2. Counts down work
   word unk68, the pause before the next AncientMummy_ShootAndRelocate. */
static void AncientMummy_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (unkE0->pause_timer > 0) {
        unkE0->pause_timer--;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
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
                AncientMummy_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but with the entity within 40 degrees of
   the player's facing it first starts AncientMummy_ShootAndRelocate when the player is nearer than 300
   and the pause is over, or, with the player nearer than 800 and less than 90 degrees off its
   heading, goes backwards for 90 to 149 frames (work byte unk78 is 1) with the command
   AncientMummy_BackwardsCommand gives. Closing in never runs. */
static void AncientMummy_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;
    s32 mode;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    if (entity->distance_to_player_3d - entity->radius - 100.0f < 300.0f && ABS(angle) < 0.6981317f && unkE0->pause_timer == 0) {
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        EntityAi_StartAction(entity, AncientMummy_ShootAndRelocate, 1);
        return;
    }
    if (entity->distance_to_player_3d - entity->radius - 100.0f < 800.0f && ABS(angle) < 0.6981317f) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (ABS(angle) < 1.5707964f) {
            unkE0->move_choice = 1;
            entity->target = &player_state;
            entity->combat_choice = 1;
            entity->action_timer = rand() % 60 + 90;
        }
    }
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
            unkE0->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 30;
            unkE0->move_choice = -1;
            break;
        default:
            unkE0->move_choice = -1;
            break;
        }
    }
    mode = entity->combat_choice;
    if (mode == 1) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (unkE0->turn_in_place == 0) {
                unkE0->turn_in_place = ABS(angle) > 0.5235988f;
                if (unkE0->turn_in_place == 0) {
                    goto done;
                }
            }
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                unkE0->turn_in_place = 0;
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            switch (unkE0->move_choice) {
            case 0:
                if (turn > 0) {
                    entity->move_command = 0x201;
                } else if (turn < 0) {
                    entity->move_command = 0x101;
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->move_command = 0x102;
                } else if (turn < 0) {
                    entity->move_command = 0x202;
                } else {
                    // match: the caller passes an integer argument the callee does not take
                    entity->move_command = AncientMummy_BackwardsCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The movement command for going backwards: a turn bit (0x100 / 0x200) while the target is off the heading by
   more than the turn limit, and command bit 2 (backwards) while it is farther than reach; 0 without a target.
   The second parameter is not used. */
static s32 AncientMummy_BackwardsCommand(Entity *entity, s32 unused, f32 reach) {
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

/* An action (busy bit 0x1000): plays animation 23 and launches the shots of ranged record 0 on
   their frames. Meanwhile it looks for a point (EntityPoint_PickNearFromDistance, EntityCollision_DistanceToPropFloor) that is more than 90
   degrees off the player's facing; from frame 35 it shrinks the model in x and z, stretches it in y
   and fades it out over EntityParams.far_distance frames, then puts the entity at the point if it found
   one and starts AncientMummy_FadeBackIn. */
static void AncientMummy_ShootAndRelocate(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;
    f32 *position;
    f32 *destination;
    f32 height;
    f32 yaw;
    f32 step;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->turn_rate = 0.0f;
        unkE0->mode_timer = entity->params->far_distance;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->unk88 == 0) {
            unkE0->step_index = 0;
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->level_tables->unk82) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.turn_rate, entity->heading_to_player);
        } else {
            unkE0->turn_rate = 0.0f;
        }
        if (unkE0->step_index == 0) {
            position = entity->model->position;
            destination = unkE0->target_point;
            EntityPoint_PickNearFromDistance(destination, position, EntityMath_DistanceXZ(position, (float *)entity->spawn_info), entity->spawn_info->wander_radius);
            height = EntityCollision_DistanceToPropFloor(entity->map_record, destination);
            if (height != D_00446810[0]) {
                yaw = EntityMath_AngleXZ(player_state.position, (char *)destination);
                if (EntityMath_WrapAngle(yaw - player_state.heading) < -1.5707964f || EntityMath_WrapAngle(yaw - player_state.heading) > 1.5707964f) {
                    unkE0->step_index = 1;
                    unkE0->target_point[1] += height;
                }
            }
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->level_tables->unk84 - entity->params->motion_speeds[entity->animation_index]) >> 8, entity->model->level_tables->unk82);
        if (entity->model->level_tables->unk82 >= 0x23) {
            unkE0->mode_timer--;
            ENTITY_FLAG_BITS(entity)->bit20 = 1;
            ENTITY_FLAG_BITS(entity)->bit18 = 1;
            ENTITY_FLAG_BITS(entity)->bit21 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            step = 1.3f / entity->params->far_distance;
            entity->model->scale[0] -= step;
            entity->model->scale[1] += step;
            entity->model->scale[2] -= step;
            entity->model_alpha -= 0x80 / entity->params->far_distance;
        }
        if (unkE0->mode_timer < 0) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            unkE0->mode_timer = (u16)entity->params->unk43A + (u16)entity->params->unk43C;
            entity->model_alpha = 0;
            if (unkE0->step_index == 1) {
                Vector_Copy(entity->model->position, unkE0->target_point);
            }
            EntityAi_StartAction(entity, AncientMummy_FadeBackIn, 1);
        }
        break;
    }
}

/* An action (busy bit 0x2000) that AncientMummy_ShootAndRelocate starts: the second half of it.
   Waits EntityParams.near_distance frames, turns the entity straight to the player, then over
   EntityParams.unk43A frames brings the model's scale and Entity.model_alpha back (to 1.3 and 0x80),
   waits unk43C frames more, clears the flag bits the first half set, lets the motion end, goes to
   state 2 and thinks, with the pause in work word unk68 set to EntityParams.action_frames. */
static void AncientMummy_FadeBackIn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 step;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->sub_timer = 0;
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
        break;
    case 1:
        if (entity->params->near_distance < unkE0->sub_timer++) {
            entity->model->rotation[1] = EntityMath_HeadingTo(entity->model->position, player_state.position);
            entity->action_step++;
        }
        break;
    case 2:
        unkE0->mode_timer--;
        if (unkE0->mode_timer < 0) {
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit27 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->action_step++;
        } else if (entity->params->unk43C < unkE0->mode_timer) {
            step = 1.3f / entity->params->unk43A;
            entity->model->scale[0] += step;
            entity->model->scale[1] -= step;
            entity->model->scale[2] += step;
            entity->model_alpha += 0x80 / entity->params->unk43A;
        } else if (unkE0->mode_timer == entity->params->unk43C) {
            entity->model->scale[0] = 1.3f;
            entity->model->scale[1] = 1.3f;
            entity->model->scale[2] = 1.3f;
            entity->model_alpha = 0x80;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetState((char *)entity, 2);
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            unkE0->pause_timer = entity->params->action_frames;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
