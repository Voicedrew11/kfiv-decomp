/* Slime: the behaviour of entity type 1 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Green Slime, Poison Slime. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_player.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionHeavy(Entity *);
extern u32 D_00446ED4;
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
int rand(void);
void EntityPoint_PickNear(float, void *, void *, void *);
float EntityMath_DistanceXZ(void *, void *);
float EntityAi_RandomAngle(void);
float EntityMath_AngleDifference(float, float);
extern PlayerState D_00413F80;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_Guard(Entity *);
void EntityAi_RangedAttack(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixY(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Normalize(void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);
float EntityMath_HeadingTo(void *, void *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void Slime_Animate(Entity *entity);
static void Slime_OnHit(Entity *entity);
static void Slime_Think(Entity *entity, s32 unknown);
static void Slime_ThinkReturn(Entity *entity, s32 unknown);
static void Slime_ThinkWander(Entity *entity, s32 unknown);
static void Slime_ThinkCombat(Entity *entity, s32 unknown);
static void Slime_Move(Entity *entity);
static void Slime_MeleeAttack(Entity *entity);

/* Set-up of a type 1 entity: clears its work area and installs the type's handlers. */
void Slime_Init(Entity *entity) {
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
    unkE0->think = (void (*)(Entity *, s32))Slime_Think;
    unkE0->animate = (void (*)(void))Slime_Animate;
    unkE0->on_hit = (void (*)(void))Slime_OnHit;
    unkE0->move = (void (*)(void))Slime_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
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

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void Slime_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
    }
}

/* The on_hit handler: every hit starts EntityAi_ReactionHeavy, whatever its strength; the target
   and flag bits 9 and 10 are left as they are. */
static void Slime_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler: as EntityAi_Think, with an if in place of the switch: Slime_ThinkReturn
   while flag bit 53 is set, Slime_ThinkCombat in state 2, Slime_ThinkWander in the other states.
   */
static void Slime_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        Slime_ThinkReturn(entity, unknown);
        return;
    }
    if (entity->state == 2) {
        Slime_ThinkCombat(entity, unknown);
        return;
    }
    Slime_ThinkWander(entity, unknown);
}

/* The think function while flag bit 53 is set: makes the spawn point the target, gives command 1
   and starts the move action (Slime_Move goes straight for the target). */
static void Slime_ThinkReturn(Entity *entity, s32 unknown) {
    f32 unused[8]; // match: retail reserves 0x20 bytes, which also keeps the call from being a jump

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    entity->target = entity->spawn_info;
    entity->unkC8 = 1;
    entity->unkA0 = 0;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->unkE0.move, unknown);
}

/* The think function of every state but 2 (Slime_Think calls it): wanders. Each time the
   timer runs out it draws: walk to a point near the spawn point (EntityPoint_PickNear; forward command
   while farther than 50), turn on the spot to a random heading (EntityAi_RandomAngle), or stand; then
   starts the move action. */
static void Slime_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 roll;
    f32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        roll = rand() % 100;
        if (roll > 60) {
            EntityPoint_PickNear(entity->spawn_info->unk4C, entity->unkE0.unk30, entity->spawn_info, entity->model->position);
            entity->target = entity->unkE0.unk30;
            entity->unkA0 = rand() % 120 + 240;
            unkE0->unk78 = 0;
        } else if (roll > 40) {
            unkE0->unk48 = EntityAi_RandomAngle();
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 1;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 2;
        }
    }
    if (unkE0->unk78 == 0) {
        if (entity->target != NULL) {
            if (EntityMath_DistanceXZ(entity->model->position, entity->target) > 50.0f) {
                entity->unkC8 = 1;
            }
        }
    } else if (unkE0->unk78 == 1) {
        turn = EntityMath_AngleDifference(entity->model->unkD0[1], unkE0->unk48);
        if (ABS(turn) > entity->params->turn_limit) {
            entity->unkC8 = (turn >= 0.0f) ? 0x200 : 0x100;
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 2 (called under `entity->state == 2`, not from a switch): guards the
   player's swing and picks among attacking, closing in and waiting as EntityAi_ThinkCombat does; it
   has no dodge, and its melee attack is Slime_MeleeAttack. Closing in is for 480 to 599
   frames, on the player (EntityParams.unk430 times in 100) or on a point near the spawn point, with
   command 1 while the target is farther than the range. */
static void Slime_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
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
                return;
            }
            EntityAi_StartAction(entity, Slime_MeleeAttack, unknown);
            return;
        case 1:
            if (rand() % 100 < entity->params->unk430) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 120 + 480;
                unkE0->unk78 = 0;
            } else {
                EntityPoint_PickNear(entity->spawn_info->unk4C, entity->unkE0.unk30, entity->spawn_info, entity->model->position);
                entity->target = entity->unkE0.unk30;
                entity->unkA0 = rand() % 120 + 480;
                unkE0->unk78 = 1;
            }
            break;
        case 2:
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        if (entity->target != NULL) {
            if (EntityMath_DistanceXZ(entity->model->position, entity->target) > entity->radius + 100.0f + 30.0f) {
                entity->unkC8 = 1;
            }
        }
        break;
    case 1:
        if (entity->target != NULL) {
            if (EntityMath_DistanceXZ(entity->model->position, entity->target) > 50.0f) {
                entity->unkC8 = 1;
            }
        }
        break;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: turns as the command bits in Entity.unkC8 say and, with command bit 1, moves
   straight towards the target, whatever the heading; picks animation 4 or 3 for that, else 0. */
static void Slime_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn = 0.0f;
    f32 amount;
    f32 heading;

    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if ((entity->unkC8 & 1) && entity->target != NULL) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[3] = 0.0f;
        amount = -1.0f;
        vector[2] = amount;
        unkE0->unk44 = EntityAi_Accelerate(entity, unkE0->unk44);
        amount = unkE0->unk44;
        if (entity->unkC8 & 0x40) {
            amount += amount;
        }
        heading = EntityMath_HeadingTo(entity->model->position, entity->target);
        sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, D_00431AF0, heading);
        Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
        Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
        entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
        entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
        entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
    } else {
        unkE0->unk44 = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x300) {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack; a hit of any of the three parts also
   clears D_00413F80.timed_flags[0], EntityParams.unk432 times in 100. */
static void Slime_MeleeAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->unkFC->unk82);
            if (ENTITY_FLAG_BITS(entity)->bit54 && rand() % 100 < entity->params->unk432) {
                D_00413F80.timed_flags[0] = 0;
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->unkFC->unk82);
            if (ENTITY_FLAG_BITS(entity)->bit55 && rand() % 100 < entity->params->unk432) {
                D_00413F80.timed_flags[0] = 0;
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->unkFC->unk82);
            if (ENTITY_FLAG_BITS(entity)->bit56 && rand() % 100 < entity->params->unk432) {
                D_00413F80.timed_flags[0] = 0;
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
