/* BloodBat: the behaviour of entity type 10 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Blood Bat. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_OnHit(char *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
extern u32 D_00446ED4;
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn3D(char *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand3D(Entity *, s32, f32);
void Vector_Normalize(void *, void *);
void Vector_Cross(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
int EntityAi_PushTurnCommand3D(Entity *, int);
void EntityPoint_PickNearWithHeight(f32 *, f32, f32, void *, void *);
int rand(void);
extern PlayerData D_00413F80;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_AngleDifference(float, float);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);

static void BloodBat_Animate(Entity *entity);
static void BloodBat_Think(Entity *entity, s32 unknown);
static int BloodBat_ThinkReturn(void *entity_arg, int unknown);
static void BloodBat_ThinkWander(Entity *entity, s32 unknown);
static int BloodBat_ThinkCombat(void *entity_arg, int unknown);
static void BloodBat_Move(Entity *entity);
static void BloodBat_Dormant(Entity *entity);

/* Set-up of a type 10 entity: clears its work area and installs the type's handlers. */
void BloodBat_Init(Entity *entity) {
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
    unkE0->think = (void (*)(Entity *, s32))BloodBat_Think;
    unkE0->animate = (void (*)(void))BloodBat_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))BloodBat_Move;
    unkE0->dormant = (void (*)(void))BloodBat_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the head turn. */
static void BloodBat_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x6000000) {
            entity->flags = flags & ~0x3800L;
        }
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
    }
}

/* The think handler: as EntityAi_Think, with EntityAi_CheckFarFromSpawn3D where that one calls EntityAi_CheckFarFromSpawn,
   BloodBat_ThinkReturn while flag bit 53 is set, BloodBat_ThinkCombat for state 2 and
   BloodBat_ThinkWander for the other states. */
static void BloodBat_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn3D((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        BloodBat_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        BloodBat_ThinkCombat(entity, unknown);
        return;
    }
    BloodBat_ThinkWander(entity, unknown);
}

/* The think function while flag bit 53 is set: walks back to the spawn point, turning away from
   whatever is pushing the entity, as EntityAi_ThinkReturn; with flag bits 11 to 13 at 0, the
   command of EntityAi_MoveCommand3D, and, while it is pushed, command bit 0x20 or 0x10 by the y of the push.
   */
static int BloodBat_ThinkReturn(void *entity_arg, int unknown) {
    Entity *entity = entity_arg;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (0xC000L << 31)) {
            push[0] += entity->unkA4[0];
            push[1] += entity->unkA4[1];
            push[2] += entity->unkA4[2];
        }
        if ((s32)(entity->flags >> 48) & 1) {
            push[0] += entity->unkB0[0];
            push[1] += entity->unkB0[1];
            push[2] += entity->unkB0[2];
        }
        Vector_Normalize(push, push);
        Vector_Subtract(to_spawn, entity->spawn_info, entity->model->position);
        Vector_Normalize(to_spawn, to_spawn);
        Vector_Cross(to_spawn, push, to_spawn);
        if (to_spawn[1] <= 0.0f) {
            if (turn == -1) {
                turn = 1;
            }
        } else if (turn == 1) {
            turn = -1;
        }
        entity->target = entity->spawn_info;
        if (turn > 0) {
            entity->unkC8 = 0x201;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x221;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x211;
            }
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x121;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x111;
            }
        } else {
            entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, 50.0f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of every state but 2 (BloodBat_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNearWithHeight) for 120 to 179 frames, takes
   the command of EntityAi_MoveCommand3D towards it (or of EntityAi_PushTurnCommand3D for the turn away from a push) and
   starts the move action. */
static void BloodBat_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[2];
    s32 turn;
    f32 *point;
    EntitySpawnInfo *spawn_info;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        spawn_info = entity->spawn_info;
        point = entity->unkE0.unk30;
        EntityPoint_PickNearWithHeight(point, (f32)spawn_info->unk4C, (f32)spawn_info->unk56, spawn_info, entity->model->position);
        entity->target = point;
        entity->unkA0 = rand() % 60 + 120;
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->unkC8 = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 2: guard, dodge and the choice among attacking, closing in and
   waiting as in EntityAi_ThinkCombat, with flag bits 11 to 13 at 0; the head does not follow.
   Closing in, it goes forward turning to Entity.unk68, with command bit 0x20 or 0x10 to bring its y
   to the player's less work float unk4C (EntityParams.unk432 give or take unk434), and it chooses
   anew once the player is within the radius plus 130. */
static int BloodBat_ThinkCombat(void *entity_arg, int unknown) {
    Entity *entity = entity_arg;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 delta; /* angle to the wanted heading, then height to the wanted level */
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
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
            unkE0->unk4C = entity->params->unk432;
            unkE0->unk4C += rand() % (ENTITY_CONTROLLER(entity)->unk434 * 2) - ENTITY_CONTROLLER(entity)->unk434;
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
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn != 0) {
            entity->unkC8 = EntityAi_PushTurnCommand3D(entity, turn);
        } else {
            delta = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - unkE0->unk4C - entity->model->position[1];
            if (ABS(delta) > entity->params->unk54) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x20;
                } else {
                    entity->unkC8 |= 0x10;
                }
            }
            entity->unkC8 |= 1;
            if (entity->distance_to_player < entity->radius + 100.0f + 30.0f) {
                entity->unkA0 = 0;
            }
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: as EntityAi_Move, with the y part of the movement eased in work float unk48,
   and with animations 4, 3 (command bit 1) and 0 only. */
static void BloodBat_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 amount;
    f32 climb_step;

    turn = 0.0f;
    climb_step = 0.02f;
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
        unkE0->unk1C = turn;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (ENTITY_FLAG_BITS(entity)->bits11 == 0) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
        if (entity->unkC8 & 3) {
            amount = 0.0f;
            if (entity->unkC8 & 1) {
                amount = -1.0f;
            } else if (entity->unkC8 & 2) {
                amount = 1.0f;
            }
            vector[2] += amount;
        }
        if (entity->unkC8 & 0xC) {
            amount = 0.0f;
            if (entity->unkC8 & 4) {
                amount = 1.0f;
            } else if (entity->unkC8 & 8) {
                amount = -1.0f;
            }
            vector[0] += amount;
        }
        if (entity->unkC8 & 0x30) {
            amount = 0.0f;
            if (entity->unkC8 & 0x10) {
                unkE0->unk48 -= climb_step;
                if (unkE0->unk48 < -1.0f) {
                    unkE0->unk48 = -1.0f;
                }
                amount = unkE0->unk48;
            } else if (entity->unkC8 & 0x20) {
                unkE0->unk48 += climb_step;
                if (1.0f < unkE0->unk48) {
                    unkE0->unk48 = 1.0f;
                }
                amount = unkE0->unk48;
            }
            vector[1] += amount;
        }
        if (entity->unkC8 & 0x3F) {
            unkE0->unk44 = EntityAi_Accelerate(entity, unkE0->unk44);
            amount = unkE0->unk44;
            if (entity->unkC8 & 0x40) {
                amount += amount;
            }
            sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
            sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[0]);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[1]);
            Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
            entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
            entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
        } else {
            unkE0->unk44 = 0.0f;
            unkE0->unk48 = 0.0f;
        }
    } else {
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}

/* The dormant action (busy bit 0x20): holds animation 1 until the player is within
   EntitySpawnInfo.unk54 or flag bit 43 is set, then goes to state 2 and thinks at once; it plays no
   animation 2. */
static void BloodBat_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
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
