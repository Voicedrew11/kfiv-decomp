/* Critter: the behaviour of entity type 25 (Entity.params->unk02, the index into D_003E6E40): its set-up
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
extern s32 D_003E6E40[][2];
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
extern PlayerData D_00413F80;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
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
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Critter_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))Critter_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The dormant action (busy bit 0x20): holds animation 2 at its start, the motion stopped, until the
   player is within EntitySpawnInfo.unk54 or flag bit 43 is set; then plays it to its end, goes to
   state 2 and thinks. */
static void Critter_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 2, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
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
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
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
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
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
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (EntityCollision_DistanceToGround(entity->model->position) != D_004467FC[0]) {
        ENTITY_FLAG_BITS(entity)->bits60 = 5;
    }
    if (entity->unkC6 != 0) {
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
   the pause to EntityParams.unk434. */
static void Critter_ThinkIdle(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 turn;

    if (unkE0->unk7A == 0) {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->unkC8 = 0x41;
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn != 0 && unkE0->unk7B <= 0) {
            if (turn > 0) {
                direction[0] += 1.5707964f;
            } else if (turn < 0) {
                direction[0] -= 1.5707964f;
            }
            direction[0] += (f32)(rand() % 140 - 70) * 0.017453292f;
            unkE0->unk48 = EntityMath_WrapAngle(direction[0]);
            unkE0->unk7A = 1;
        }
        if (unkE0->unk7B > 0) {
            unkE0->unk7B--;
        }
        if (unkE0->unk7C == 0) {
            unkE0->unk7C = EntityAi_CheckMeleeHit(entity, &entity->params->unk240[1], 0, 1, 1);
        }
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], unkE0->unk48);
        if (ABS(angle) > entity->params->turn_limit) {
            if (angle >= 0.0f) {
                entity->unkC8 |= 0x600;
            } else {
                entity->unkC8 |= 0x500;
            }
        } else {
            unkE0->unk7A = 0;
            unkE0->unk7C = 0;
            unkE0->unk7B = entity->params->unk434;
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
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        func_00196D48(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (unkE0->unk79 == 0) {
                unkE0->unk79 = ABS(angle) > 0.5235988f;
                if (unkE0->unk79 == 0) {
                    goto done;
                }
            }
            entity->unkC8 = EntityAi_TurnCommand(entity);
            if (entity->unkC8 == 0) {
                unkE0->unk79 = 0;
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity->radius + 100.0f + 30.0f, entity, 0);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}
