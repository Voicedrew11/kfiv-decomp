/* DarkFlower: the behaviour of entity type 71 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Flower. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
int EntityAi_ThinkReturn(void *, int);
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
int rand(void);
void EntityPoint_PickNear(float, void *, void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
extern f32 D_00431AF0[16];
extern PlayerData D_00413F80;
void sceVu0RotMatrixY(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Normalize(void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);
float EntityMath_HeadingTo(float *, float *);
int Motion_Start(void *, int, int, u32, int);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void DarkFlower_Think(Entity *entity, s32 unknown);
static void DarkFlower_ThinkWander(Entity *entity, s32 unknown);
static void DarkFlower_ThinkCombat(Entity *entity, s32 unknown);
static void DarkFlower_Move(Entity *entity);
static void DarkFlower_RangedAttack0(Entity *entity);

/* Set-up of a type 71 entity: clears its work area and installs the type's handlers. */
void DarkFlower_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = DarkFlower_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))DarkFlower_Move;
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
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with DarkFlower_ThinkWander for every state but 2 and
   DarkFlower_ThinkCombat for state 2. */
static void DarkFlower_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        DarkFlower_ThinkCombat(entity, unknown);
        return;
    }
    DarkFlower_ThinkWander(entity, unknown);
}

/* The think function of every state but 2 (DarkFlower_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 79 times in 100, for 240 to
   329 frames) or no target (90 to 149 frames), takes the command of EntityAi_MoveCommand towards it and
   starts the move action. It does not go for the player. */
static void DarkFlower_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 >= 21) {
            f32 *point = unkE0->unk30;

            EntityPoint_PickNear(entity->spawn_info->unk4C, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->unkA0 = rand() % 90 + 240;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 60 + 90;
        }
    }
    entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat without the dodge; ranged record 0 is
   DarkFlower_RangedAttack0, and instead of closing in on the player it goes to a point
   EntityPoint_PickNear picks round the spawn point. */
static void DarkFlower_ThinkCombat(Entity *entity, s32 unknown) {
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                    EntityAi_StartAction(entity, DarkFlower_RangedAttack0, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            EntityPoint_PickNear(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 210;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 60 + 90;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: as EntityAi_Move, but the entity moves along the bearing to its target instead
   of along its heading, not up or down, and the animation is always 0. */
static void DarkFlower_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 heading;
    f32 amount;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
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
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    vector = ENTITY_SCRATCH_VECTOR;
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
    if (entity->unkC8 & 0xF) {
        unkE0->unk44 = EntityAi_Accelerate(entity, unkE0->unk44);
        amount = unkE0->unk44;
        if (entity->unkC8 & 0x40) {
            amount += amount;
        }
        if (entity->target != NULL) {
            heading = EntityMath_HeadingTo(entity->model->position, entity->target);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, D_00431AF0, heading);
            Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
            entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
            entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
        }
    } else {
        unkE0->unk44 = 0.0f;
    }
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
}

/* The ranged attack with record 0 (busy bit 1): animation 23, no turning. From frame
   EntityParams.unk430 to unk432 it launches shots (EntityAi_LaunchShots) every unk434 frames, taking
   records 1 and 0 in turn (work byte unk7A), or record 0 every frame when unk434 is 0.
   DarkFlower_ThinkCombat starts it for record 0 only. */
static void DarkFlower_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 0);
    EntityParamsUnk3D8 *second_unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= entity->params->unk430 && entity->model->unkFC->unk82 <= entity->params->unk432) {
            if ((s16)entity->params->unk434 == 0) {
                EntityAi_LaunchShots(entity, unk3D8, 30, 50);
            } else if (D_00446ED4 % (s16)entity->params->unk434 == 0) {
                if (unkE0->unk7A != 0) {
                    EntityAi_LaunchShots(entity, unk3D8, 30, 50);
                } else {
                    EntityAi_LaunchShots(entity, second_unk3D8, 30, 50);
                }
                unkE0->unk7A ^= 1;
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
