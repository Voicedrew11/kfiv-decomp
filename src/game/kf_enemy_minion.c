/* Minion: the behaviour of entity type 60 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Minion. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "kf_shot.h"
#include "common.h"

int rand(void);
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
void Motion_SetPartSlots(EntityModel *, s32, s32, s32);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
int EntityAi_StartSlotMotion(void *, void *, int, int, int, int, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern PlayerData D_00413F80;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
int EntityAi_StepSlotMotion(Entity *, EntityMotionSlot *, int, f32);
s32 Motion_StepSlot(Entity *, EntityUnkE0SlotBits *, s32, s32);
void EntityEvent_StartFrameEffects(Entity *, s32, s32, s32);
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
int EntityAi_LookAtPoint(void *, int, int, int, int, float);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *entity, void (*callback)(void), int run);
void EntityAi_SetState(char *, u8);
void EntityEvent_OnDeath(Entity *);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float EntityMath_WrapAngle(float);
float EntityMath_DistanceXZ(void *, void *);
f32 EntityMath_Approach(f32, f32, f32);
int func_001B4F30(u16, void *, void *, void *, void *);
void *memset(void *, int, u32);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);

static void Minion_Think(Entity *entity, s32 unknown);
static void Minion_ThinkCombat(Entity *entity, s32 unknown);
static void Minion_Animate(Entity *entity);
static void Minion_Guard(Entity *entity);
static void Minion_MeleeAttack(Entity *entity);
static void Minion_RangedAttack(Entity *entity);
static void Minion_Move(Entity *entity);
static void Minion_OnHit(Entity *entity);
static void Minion_ReactionLight(Entity *entity);
static void Minion_ReactionHeavy(Entity *entity);
static void Minion_Death(Entity *entity);
static void Minion_Dodge(Entity *entity);
static s32 Minion_LaunchShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static void func_00153F60(Entity *entity, f32 *result, f32 *point);

/* Set-up of a type 60 entity: clears its work area and installs the type's handlers. */
void Minion_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Minion_Think;
    unkE0->animate = (void (*)(void))Minion_Animate;
    unkE0->on_hit = (void (*)(void))Minion_OnHit;
    unkE0->move = (void (*)(void))Minion_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))Minion_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
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
    if (!(rand() & 1)) {
        Motion_SetPartSlots(entity->model, 4, 5, 1);
        unkE0->unk7B = 0;
    } else {
        Motion_SetPartSlots(entity->model, 6, 7, 1);
        unkE0->unk7B = 1;
    }
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit5 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit6 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk88.bit6 = 1;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with Minion_ThinkCombat for state 2. Once
   Entity.unk2C is below half of EntityParams.unk0A it sets work byte unk79 to 1 and keeps animation
   18 on the second motion slot. */
static void Minion_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (unkE0->unk79 == 0 && entity->unk2C < entity->params->unk0A >> 1) {
        unkE0->unk79 = 1;
    }
    if (unkE0->unk79 == 1 || unkE0->unk79 == 2) {
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)unkE0)->unk88, 1, 0x12, 0, entity->params->unkC4 >> 1, 0);
    }
    if (entity->unkC6 == 0) {
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
                Minion_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat with the type's own guard, dodge and
   attacks; it closes in at the run. */
static void Minion_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, Minion_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, Minion_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
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
                EntityAi_StartAction(entity, Minion_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, Minion_MeleeAttack, unknown);
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
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x241;
        } else if (turn < 0) {
            entity->unkC8 = 0x141;
        } else {
            entity->unkC8 = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The animate handler: plays the two motion slots of the work area (unk80 and unk88) where
   EntityAi_Animate plays the model's one motion, then turns the head. */
static void Minion_Animate(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    EntityModelUnkF8 *part;
    f32 limits[4];
    s32 previous;
    f32 rate;

    if (!ENTITY_FLAG_BITS(entity)->bit14) {
        return;
    }
    if (slots->unk80.unk01 != 0) {
        if (entity->unkC6 & 0x12) {
            rate = unkE0->unk40;
        } else {
            rate = 1.0f / slots->unk80.unk01;
        }
        // match: the caller passes a float the callee does not take
        ENTITY_FLAG_BITS(entity)->bit34 = ((int (*)(void *, void *, int, float))EntityAi_StepSlotMotion)(entity, &slots->unk80, 0, rate);
    } else {
        previous = slots->unk80.unk04;
        ENTITY_FLAG_BITS(entity)->bit34 = Motion_StepSlot(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk80.index != 3) {
            EntityEvent_StartFrameEffects(entity, slots->unk80.index, previous, slots->unk80.unk04);
            EntityEvent_StartFrameCues(entity, slots->unk80.index, previous, slots->unk80.unk04);
        }
    }
    if (slots->unk88.unk01 != 0) {
        if (entity->unkC6 & 0x12) {
            rate = unkE0->unk40;
        } else {
            rate = 1.0f / slots->unk88.unk01;
        }
        // match: the caller passes a float the callee does not take
        unkE0->unk7A = ((int (*)(void *, void *, int, float))EntityAi_StepSlotMotion)(entity, &slots->unk88, 1, rate);
    } else {
        if (unkE0->unk79 == 1 && slots->unk88.index == 0x12 && unkE0->unk7A != 0) {
            part = &entity->model->unkF8[2];
            if (unkE0->unk7B) {
                part = &entity->model->unkF8[3];
            }
            unkE0->unk54 = part->unk50[0];
            unkE0->unk58 = part->unk50[1];
            unkE0->unk5C = part->unk50[2];
            unkE0->unk79 = 2;
        }
        if (unkE0->unk79 != 2) {
            previous = slots->unk88.unk04;
            unkE0->unk7A = Motion_StepSlot(entity, &slots->unk88, 1, ENTITY_FLAG_BITS(entity)->bits11);
            if (slots->unk88.index == 3) {
                EntityEvent_StartFrameEffects(entity, slots->unk88.index, previous, slots->unk88.unk04);
                EntityEvent_StartFrameCues(entity, slots->unk88.index, previous, slots->unk88.unk04);
            }
        }
    }
    limits[1] = entity->params->unk430 * 0.017453292f;
    limits[0] = -entity->params->unk430 * 0.017453292f;
    limits[3] = entity->params->unk432 * 0.017453292f;
    limits[2] = -entity->params->unk432 * 0.017453292f;
    if (ENTITY_FLAG_BITS(entity)->bit9 && entity->target != NULL) {
        EntityAi_LookAtPoint(entity, (int)unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, (int)entity->target, (int)limits, 12.0f);
    } else {
        EntityAi_LookAtPlayer(entity, unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, NULL, 8.0f);
    }
}

/* The guard action (busy bit 0x10): as EntityAi_Guard, on the motion slots. */
static void Minion_Guard(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 0xD, 0, 0xA, 1);
        if (unkE0->unk79 == 1) {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 0xD, 0, 0xA, 1);
        }
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->unkA0 = entity->params->unkCE;
            ENTITY_FLAG_BITS(entity)->bit35 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->unkA0 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, on the motion slots. */
static void Minion_MeleeAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (unkE0->unk79 == 0) {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        }
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
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (slots->unk80.unk02 <= unk240->unk01) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (slots->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, slots->unk80.unk02);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, slots->unk80.unk02);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, slots->unk80.unk02);
        }
        if (slots->unk80.unk02 > 0 && unk240->unk03 >= slots->unk80.unk02) {
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, on the motion slots;
   Minion_LaunchShots launches the shots. */
static void Minion_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->bits58);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (unkE0->unk79 == 0) {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        }
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (slots->unk80.unk02 <= unk3D8->unk01) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        Minion_LaunchShots(entity, unk3D8, (slots->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8, slots->unk80.unk02, NULL, 0.0f, 0.0f);
        if (slots->unk80.unk02 > 0 && unk3D8->unk03 >= slots->unk80.unk02) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The move handler: turns as the command says and picks animation 0, 3 or 4 on the motion slots. */
static void Minion_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 turn;

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
        if (unkE0->unk1C > entity->params->turn_limit) {
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
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            EntityAi_StartSlotMotion(entity, &slots->unk80, 0, 4, 0, entity->params->unkC4 >> 1, 0);
            if (unkE0->unk79 == 0) {
                EntityAi_StartSlotMotion(entity, &slots->unk88, 1, 4, 0, entity->params->unkC4 >> 1, 0);
            }
        } else {
            EntityAi_StartSlotMotion(entity, &slots->unk80, 0, 3, 0, entity->params->unkC4 >> 1, 0);
            if (unkE0->unk79 == 0) {
                EntityAi_StartSlotMotion(entity, &slots->unk88, 1, 3, 0, entity->params->unkC4 >> 1, 0);
            }
        }
    } else if (entity->unkC8 & 0x300) {
        EntityAi_StartSlotMotion(entity, &slots->unk80, 0, 3, 0, entity->params->unkC4 >> 1, 0);
        if (unkE0->unk79 == 0) {
            EntityAi_StartSlotMotion(entity, &slots->unk88, 1, 3, 0, entity->params->unkC4 >> 1, 0);
        }
    } else {
        EntityAi_StartSlotMotion(entity, &slots->unk80, 0, 0, 0, entity->params->unkC4 >> 1, 0);
        if (unkE0->unk79 == 0) {
            EntityAi_StartSlotMotion(entity, &slots->unk88, 1, 0, 0, entity->params->unkC4 >> 1, 0);
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit with the type's own two reactions. */
static void Minion_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))Minion_ReactionLight, 0);
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))Minion_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the motion slots. */
static void Minion_ReactionLight(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 0xE, 0, 5, 1);
        if (unkE0->unk79 == 0) {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 0xE, 0, 5, 1);
        }
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        EntityAi_SetAttackDelay(entity, entity->params->unkC8);
        EntityAi_SetReaction((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            entity->unkE0.think(entity, 1);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the motion slots. */
static void Minion_ReactionHeavy(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 0xF, 0, 5, 1);
        if (unkE0->unk79 == 0) {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 0xF, 0, 5, 1);
        }
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetAttackDelay(entity, entity->params->unkC8);
            EntityAi_SetReaction((char *)entity, NULL, 0);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots. */
static void Minion_Death(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 0x10, 0, 5, 1);
        if (unkE0->unk79 == 0) {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 0x10, 0, 5, 1);
        }
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkA0 = 20;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}

/* The dodge action (busy bit 8): plays animation 6 or 7, by the side of the player's facing the
   entity is on, turning to the player. */
static void Minion_Dodge(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        if (EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position)) * 57.29578f > 0.0f) {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 6, 0, entity->params->unkC4 >> 1, 1);
            if (unkE0->unk79 == 0) {
                EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 6, 0, entity->params->unkC4 >> 1, 1);
            }
        } else {
            EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 7, 0, entity->params->unkC4 >> 1, 1);
            if (unkE0->unk79 == 0) {
                EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk88, 1, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (((EntityUnkE0Kind151D30 *)unkE0)->unk80.unk01 == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* Helper of Minion_RangedAttack: launches the record's shots whose frame has come, as
   EntityShot_LaunchAimed does, from both launch points while work byte unk79 is 0 and afterwards only from
   the one work byte unk7B picks. */
static s32 Minion_LaunchShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;
    s32 i;
    s32 end;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    if (unkE0->unk79 == 0) {
        i = 0;
        end = 2;
    } else {
        i = unkE0->unk7B;
        end = i + 1;
    }
    for (; i < end; i++) {
        if (record->unk10[i].unk02 != 0 && frame_from < record->unk10[i].unk02 && record->unk10[i].unk02 <= frame_to) {
            shot = Shot_Alloc();
            shot->unk22 = shot->unk20 = 0;
            shot->unk00 = 1;
            shot->unk28 = entity->params->unk238;
            from = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
            tip = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01 + 1);
            if (from != NULL && tip != NULL) {
                if (record->unk10[i].unk01 != 0xFF) {
                    delta_x = tip[0] - from[0];
                    delta_y = tip[1] - from[1];
                    delta_z = tip[2] - from[2];
                    angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
                    angles[2] = angles[3] = 0.0f;
                } else {
                    Vector_Copy(angles, entity->model->unkD0);
                    angles[1] = EntityMath_WrapAngle(angles[1] + 3.1415927f);
                }
                if (target != NULL) {
                    pitch = EntityMath_WrapAngle(-atan2(target[1] - from[1], EntityMath_DistanceXZ(from, target)));
                    yaw = EntityMath_AngleXZ(from, (char *)target);
                    angles[0] = EntityMath_Approach(angles[0], pitch, pitch_rate);
                    angles[1] = EntityMath_Approach(angles[1], yaw, yaw_rate);
                }
                func_001B4F30(record->unk10[i].unk00, shot, from, angles, entity);
            }
        }
    }
    return 0;
}

/* Turns a point from world space into the entity's own space: undoes the model's position, then its rotation. */
static void func_00153F60(Entity *entity, f32 *result, f32 *point) {
    f32 matrix[16];
    f32 offset[4];

    memset(matrix, 0, 0x40);
    matrix[15] = matrix[10] = matrix[5] = matrix[0] = 1.0f;
    offset[0] = -entity->model->position[0];
    offset[1] = -entity->model->position[1];
    offset[2] = -entity->model->position[2];
    offset[3] = -entity->model->position[3];
    sceVu0TransMatrix(matrix, matrix, offset);
    sceVu0RotMatrixY(matrix, matrix, -entity->model->unkD0[1]);
    sceVu0RotMatrixZ(matrix, matrix, -entity->model->unkD0[2]);
    sceVu0RotMatrixX(matrix, matrix, -entity->model->unkD0[0]);
    sceVu0ApplyMatrix(result, matrix, point);
}
