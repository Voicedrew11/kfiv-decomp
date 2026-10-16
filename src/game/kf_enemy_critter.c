/* Critter: the behaviour of entity type 25 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Critter. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);
extern u32 D_00446ED4;
extern f32 D_004467FC[];
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
f32 EntityCollision_DistanceToGround(f32 *);
int EntityAi_ThinkReturn(void *, int);
int rand(void);
int EntityAi_TurnFromPush(void *, void *);
float EntityMath_WrapAngle(float);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
float EntityMath_AngleDifference(float, float);
extern PlayerData player_state;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
int EntityAi_TurnCommand(Entity *);
s16 EntityAi_MoveCommand(float, void *, int); // match: this file only matches with this type; EntityAi_MoveCommand.c defines it differently
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);

static void Critter_Dormant(Entity *entity);
static void Critter_Think(Entity *entity, s32 unknown);
static void Critter_ThinkIdle(Entity *entity, s32 unknown);
static void Critter_ThinkCombat(Entity *entity, s32 unknown);

/* Set-up of a type 25 entity: clears its work area and installs the type's handlers. */
void Critter_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    unkE0->think = Critter_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))Critter_Dormant;
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
    unkE0->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The dormant action (busy bit 0x20): holds animation 2 at its start, the motion stopped, until the
   player is within EntitySpawnInfo.wake_distance or flag bit 43 is set; then plays it to its end, goes to
   state 2 and thinks. */
static void Critter_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->busy_flags |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 2, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->bit43) {
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            EntityAi_SetState((char *)entity, 2);
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

/* The think handler: as EntityAi_Think, with Critter_ThinkIdle for state 0 and
   Critter_ThinkCombat for states 1 and 2. Sets the flag field bits60 to 5 whenever
   EntityCollision_DistanceToGround gives a reading for the entity's position. */
static void Critter_Think(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (EntityCollision_DistanceToGround(entity->model->position) != D_004467FC[0]) {
        ENTITY_FLAG_BITS(entity)->bits60 = 5;
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
        break;
    case 1:
    case 2:
        Critter_ThinkCombat(entity, unknown);
        return;
    }
    Critter_ThinkIdle(entity, unknown);
}

/* The think function of state 0 (called after the switch, which `case 0:` leaves): gives command
   0x41 and, once in each such run, checks part 0 of melee record 1 for a hit (EntityAi_CheckMeleeHit, work
   byte unk7C). When the entity is pushed and the pause in work byte unk7B is over, it picks a
   heading 90 degrees to the side of the one EntityAi_TurnFromPush gives, give or take 70 degrees (work
   float unk48), turns to it with command bit 0x400 and no movement (work byte unk7A set), then sets
   the pause to EntityParams.action_frames. */
static void Critter_ThinkIdle(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 turn;

    if (unkE0->step_index == 0) {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->move_command = 0x41;
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn != 0 && unkE0->action_flags <= 0) {
            if (turn > 0) {
                direction[0] += 1.5707964f;
            } else if (turn < 0) {
                direction[0] -= 1.5707964f;
            }
            direction[0] += (f32)(rand() % 140 - 70) * 0.017453292f;
            unkE0->part_angle_x = EntityMath_WrapAngle(direction[0]);
            unkE0->step_index = 1;
        }
        if (unkE0->action_flags > 0) {
            unkE0->action_flags--;
        }
        if (unkE0->action_counter == 0) {
            unkE0->action_counter = EntityAi_CheckMeleeHit(entity, &entity->params->melee_attacks[1], 0, 1, 1);
        }
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        angle = EntityMath_AngleDifference(entity->model->rotation[1], unkE0->part_angle_x);
        if (ABS(angle) > entity->params->turn_limit) {
            if (angle >= 0.0f) {
                entity->move_command |= 0x600;
            } else {
                entity->move_command |= 0x500;
            }
        } else {
            unkE0->step_index = 0;
            unkE0->action_counter = 0;
            unkE0->action_flags = entity->params->action_frames;
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of states 1 and 2: as EntityAi_ThinkCombat; it leaves flag bits 9 and 10 as
   they are and never adds command bit 0x40. */
static void Critter_ThinkCombat(Entity *entity, s32 unknown) {
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
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = D_00446ED4;
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
    if (unkE0->move_choice == 0) {
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
            if (turn > 0) {
                entity->move_command = 0x201;
            } else if (turn < 0) {
                entity->move_command = 0x101;
            } else {
                entity->move_command = EntityAi_MoveCommand(entity->radius + 100.0f + 30.0f, entity, 0);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}
