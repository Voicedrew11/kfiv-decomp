/* Widda: the behaviour of entity type 19 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Widda Large, Widda. The game names
   no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records
   by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern PlayerData player_state;
float atan2f(float, float);
float EntityMath_EaseAngleToward(float *, float, float);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
float EntityMath_AngleXZ(float *, char *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
extern int entity_under_player[];
int Motion_Start(void *, int, int, u32, int);

static void Widda_Animate(Entity *entity);
static void Widda_AimPartsAtTarget(Entity *entity);
static void Widda_Think(Entity *entity, s32 unknown);
static void Widda_ThinkCombat(Entity *entity, s32 unknown);
static void Widda_Move(Entity *entity);

/* Set-up of a type 19 entity: clears its work area and installs the type's handlers. */
void Widda_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = Widda_Think;
    unkE0->animate = (void (*)(void))Widda_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))Widda_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
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

/* The animate handler: as EntityAi_Animate, and Widda_AimPartsAtTarget turns two model parts.
   */
static void Widda_Animate(Entity *entity) {
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
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? unkE0->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        Widda_AimPartsAtTarget(entity);
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* Helper of Widda_Animate: turns the two model parts EntityParams.near_distance and unk438 name
   towards the target, pitch from the player's height (at most 30 degrees) and yaw from the bearing
   (at most 40), each part by half of it; the angles are work floats unk48 to unk54. Without a
   target or while the entity is busy it only reads the parts' angles into them. */
static void Widda_AimPartsAtTarget(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->parts;
    s32 first = entity->params->near_distance;
    s32 second = entity->params->far_distance;
    f32 angle;
    f32 bearing;
    f32 limit;

    if (entity->target == NULL || entity->busy_flags != 0) {
        unkE0->part_angle_x = parts[first].unk50[0];
        unkE0->part_angle_y = parts[first].unk50[1];
        unkE0->part_angle_z = parts[second].unk50[0];
        unkE0->pitch_speed = parts[second].unk50[1];
    } else {
        angle = atan2f(player_state.position[1] - entity->model->position[1], entity->distance_to_player);
        limit = 0.5235988f;
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        EntityMath_EaseAngleToward(&unkE0->part_angle_x, angle * 0.5f, 10.0f);
        EntityMath_EaseAngleToward(&unkE0->part_angle_z, angle * 0.5f, 10.0f);
        parts[first].unk50[0] = unkE0->part_angle_x;
        parts[second].unk50[0] = unkE0->part_angle_z;
        bearing = EntityMath_HeadingTo(entity->model->position, entity->target);
        angle = EntityMath_AngleDifference(entity->model->rotation[1], bearing);
        limit = 0.6981317f;
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        EntityMath_EaseAngleToward(&unkE0->part_angle_y, angle * 0.5f, 10.0f);
        EntityMath_EaseAngleToward(&unkE0->pitch_speed, angle * 0.5f, 10.0f);
        parts[first].unk50[1] = unkE0->part_angle_y;
        parts[second].unk50[1] = unkE0->pitch_speed;
    }
}

/* The think handler: as EntityAi_Think, with Widda_ThinkCombat for state 2. Sets work byte
   unk7A when flag bit 45, 47 or 48 is set during a dodge (busy bit 8). */
static void Widda_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if ((entity->busy_flags & ENTITY_BUSY_DODGE) && (entity->flags & (0xD000L << 33))) {
        unkE0->step_index = 1;
    }
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        Widda_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but closing in runs (work byte unk78 is
   1) once the player is farther than 900 and within 40 degrees ahead. Every tenth frame, with the
   player near and the entity within 40 degrees of the player's facing, a chance of
   EntityParams.action_frames in 100 makes it start EntityAi_Dodge when the player is 40 to 90 degrees off
   its heading, or close in at the run when the player is farther off or work byte unk7A is set. */
static void Widda_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.6981317f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    side = EntityAi_PickCombatChoice(entity, 0);
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
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (entity->distance_to_player > 900.0f && ABS(angle) < 0.6981317f) {
                entity->action_timer = 300;
                unkE0->move_choice = 1;
            } else {
                entity->action_timer = rand() % 60 + 120;
                unkE0->move_choice = 0;
            }
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
    angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    if (entity->distance_to_player_3d - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f
        && frame_counter % 10U == entity->spawn_index % 10U && rand() % 100 < ENTITY_CONTROLLER(entity)->action_frames) {
        angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        if (unkE0->step_index == 0) {
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                EntityAi_StartAction(entity, EntityAi_Dodge, 1);
                unkE0->step_index = 0;
                return;
            }
            if (!(ABS(angle) >= 1.5707964f)) {
                goto skip;
            }
        }
        entity->prev_combat_choice = entity->combat_choice;
        entity->target = &player_state;
        entity->action_timer = 300;
        unkE0->move_choice = entity->combat_choice = 1;
        unkE0->step_index = 0;
    }
skip:
    if (entity->combat_choice == 1) {
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
                angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
                if (entity->distance_to_player > 900.0f && ABS(angle) < 0.6981317f) {
                    entity->action_timer = 300;
                    unkE0->move_choice = 1;
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->move_command = 0x241;
                } else if (turn < 0) {
                    entity->move_command = 0x141;
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: as EntityAi_Move, but it only turns the entity (twice as fast for command bits
   0x40 and 0x400) and picks the animation; it does not move it. */
static void Widda_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    if (entity == (Entity *)entity_under_player[0] && (entity->params->flags & 0x800)) {
        entity->move_command = 0;
    }
    if (entity->move_command & 0x100) {
        unkE0->turn_rate -= entity->params->turn_step_per_frame;
        if (unkE0->turn_rate < -entity->params->turn_limit) {
            unkE0->turn_rate = -entity->params->turn_limit;
        }
    } else if (entity->move_command & 0x200) {
        unkE0->turn_rate += entity->params->turn_step_per_frame;
        if (unkE0->turn_rate > entity->params->turn_limit) {
            unkE0->turn_rate = entity->params->turn_limit;
        }
    } else {
        unkE0->turn_rate = 0.0f;
    }
    turn = unkE0->turn_rate;
    if (entity->move_command & 0x440) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    if (entity->move_command & 1) {
        if (entity->move_command & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & 2) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 0x10) {
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 0x20) {
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 4) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 8) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & 0x300) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}
