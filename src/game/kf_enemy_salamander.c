/* Salamander: the behaviour of entity type 22 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Salamander. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void Motion_SetPartSlots(EntityModel *, s32, s32, s32);
extern s32 D_003E6E40[][2];
int EntityAi_StepSlotMotion(Entity *, EntityMotionSlot *, int, f32);
s32 Motion_StepSlot(Entity *, EntityUnkE0SlotBits *, s32, s32);
void EntityEvent_StartFrameEffects(Entity *, s32, s32, s32);
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
int EntityAi_LookAtPoint(void *, int, int, int, int, float);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *entity, void (*callback)(void), int run);
void EntityAi_SetState(char *, u8);
int EntityAi_StartSlotMotion(void *, void *, int, int, int, int, int);
void EntityAi_SetAttackDelay(Entity *, s32);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityEvent_OnDeath(Entity *);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
extern u32 D_00446ED4;
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn3D(char *);
void EntityAi_StepState(void *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand3D(Entity *, s32, f32);
void Vector_Normalize(void *, void *);
void Vector_Cross(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
extern float D_00413F80[];
int rand(void);
s32 EntityAi_IsPointInView(Entity *, void *, f32, f32, f32);
s32 EntityAi_PickRangedAttack(Entity *);
void EntityPoint_PickNearWithHeight(f32 *, f32, f32, void *, void *);
int EntityAi_PushTurnCommand3D(Entity *, int);
int EntityAi_PickAttack(char *);
float EntityMath_WrapAngle(float);
void EntityMath_FacingVector(Vector4 *, float);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);

static void Salamander_Animate(Entity *entity);
static void Salamander_OnHit(Entity *entity);
static void Salamander_ReactionRewind(Entity *entity);
static void Salamander_ReactionLight(Entity *entity);
static void Salamander_ReactionHeavy(Entity *entity);
static void Salamander_Death(Entity *entity);
static void Salamander_MeleeAttack(Entity *entity);
static void Salamander_RangedAttack(Entity *entity);
static void Salamander_Think(Entity *entity, s32 unknown);
static void Salamander_ThinkReturn(Entity *entity, s32 unknown);
static void func_00134DC8(Entity *entity, s32 unknown);
static void Salamander_ThinkCombat(Entity *entity, s32 unknown);
static void Salamander_Move(Entity *entity);

/* Set-up of a type 22 entity: clears its work area and installs the type's handlers. */
void Salamander_Init(Entity *entity) {
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
    unkE0->think = Salamander_Think;
    unkE0->animate = (void (*)(void))Salamander_Animate;
    unkE0->on_hit = (void (*)(void))Salamander_OnHit;
    unkE0->move = (void (*)(void))Salamander_Move;
    unkE0->death = (void (*)(void))Salamander_Death;
    unkE0->dormant = NULL;
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
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_SetPartSlots(entity->model, 10, 0x13, 1);
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit5 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit6 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk88.bit6 = 1;
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: plays the two motion slots of the work area (unk80 and unk88) where
   EntityAi_Animate plays the model's one motion, then turns the head (EntityAi_LookAtPoint with the target
   while flag bit 9 is set and there is one). */
static void Salamander_Animate(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 limits[4];
    f32 unused_vector[4]; // match: retail reserves 16 more bytes of stack than it uses
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
        ((int (*)(void *, void *, int, float))EntityAi_StepSlotMotion)(entity, &slots->unk88, 1, rate);
    } else {
        previous = slots->unk88.unk04;
        Motion_StepSlot(entity, &slots->unk88, 1, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk88.index == 3) {
            EntityEvent_StartFrameEffects(entity, slots->unk88.index, previous, slots->unk88.unk04);
            EntityEvent_StartFrameCues(entity, slots->unk88.index, previous, slots->unk88.unk04);
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

/* The on_hit handler: as EntityAi_OnHit with the type's own three reactions; it leaves flag bit 10
   as it is and sets work byte unk7A. */
static void Salamander_OnHit(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))Salamander_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))Salamander_ReactionLight, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))Salamander_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
    unkE0->unk7A = 1;
}

/* The reaction to a light hit while flag bit 19 is set (busy bit 2 while it blends in): as
   EntityAi_ReactionRewind, on the first motion slot. */
static void Salamander_ReactionRewind(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 frame;

    entity->unkC6 |= 2;
    switch (entity->unkCB) {
    case 0:
        frame = unkE0->unk80.unk02 - 3;
        if (frame < 0) {
            frame = 0;
        }
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, unkE0->unk80.unk00 & 0x1F, frame, 5, 1);
        entity->unkCB++;
        entity->flags = ((entity->flags & ~0x3800ULL) | 0x800) & ~0x200ULL;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->unkC6 &= ~2;
            EntityAi_SetReaction((char *)entity, NULL, 0);
        }
        break;
    }
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the first motion slot,
   without the call of EntityEvent_StartFrameCues. */
static void Salamander_ReactionLight(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xE, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkCB++;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
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

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the first motion slot. */
static void Salamander_ReactionHeavy(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xF, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkCB++;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
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

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots, with Entity.unk30 set
   to 20; from frame 30 of animation 16 it clears flag bit 28 and brings the model's pitch and roll
   back to 0. */
static void Salamander_Death(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, 0x10, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        entity->unk30 = 20.0f;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 >= 30) {
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
        }
        break;
    case 3:
        EntityMath_EaseAngleToward(&entity->model->unkD0[0], 0.0f, 10.0f);
        EntityMath_EaseAngleToward(&entity->model->unkD0[2], 0.0f, 10.0f);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 4:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, on the first motion slot. */
static void Salamander_MeleeAttack(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[kind];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, kind | 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            EntityAi_TurnToHeading(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= unk240->unk01) {
            EntityAi_TurnToHeading(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0;
        }
        elapsed = (unkE0->unk80.unk04 - entity->params->unk80[((EntityUnkE0SlotBits *)&unkE0->unk80)->index]) >> 8;
        if (!((s32)(entity->flags >> 54) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, unkE0->unk80.unk02);
        }
        if (!((s32)(entity->flags >> 55) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, unkE0->unk80.unk02);
        }
        if (!((s32)(entity->flags >> 56) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, unkE0->unk80.unk02);
        }
        if (unkE0->unk80.unk02 > 0 && unk240->unk03 >= unkE0->unk80.unk02) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, on the first motion slot; with
   animation 23 it launches the record's shots every other frame from frame 20 to 44 instead, the
   head following the target meanwhile. */
static void Salamander_RangedAttack(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= unk3D8->unk01) {
            EntityAi_TurnToHeading(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0;
        }
        if (((EntityUnkE0SlotBits *)&unkE0->unk80)->index == 0x17) {
            if (unkE0->unk80.unk02 >= 20 && unkE0->unk80.unk02 <= 44) {
                if (D_00446ED4 & 1) {
                    EntityAi_LaunchShots(entity, unk3D8, 20, 44);
                }
                ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
            }
        } else {
            EntityAi_LaunchShots(entity, unk3D8,
                          (unkE0->unk80.unk04 - entity->params->unk80[((EntityUnkE0SlotBits *)&unkE0->unk80)->index]) >> 8,
                          unkE0->unk80.unk02);
        }
        if (unkE0->unk80.unk02 > 0 && unk3D8->unk03 >= unkE0->unk80.unk02) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The think handler: as EntityAi_Think, with EntityAi_CheckFarFromSpawn3D where that one calls EntityAi_CheckFarFromSpawn,
   Salamander_ThinkReturn while flag bit 53 is set, Salamander_ThinkCombat for states 1 and 2,
   and func_00134DC8 for state 0 and, as long as work byte unk7A is 0 (Salamander_OnHit sets it),
   for every state. Keeps animation 3 on the second motion slot except in the death action. */
static void Salamander_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (!(entity->unkC6 & 0x40)) {
        EntityAi_StartSlotMotion(entity, unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn3D((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        Salamander_ThinkReturn(entity, unknown);
        return;
    }
    if (unkE0->unk7A == 0) {
        func_00134DC8(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        Salamander_ThinkCombat(entity, unknown);
        return;
    }
    func_00134DC8(entity, unknown);
}

/* The think function while flag bit 53 is set: walks back to the spawn point, turning away from
   whatever is pushing the entity, as EntityAi_ThinkReturn; with the command of EntityAi_MoveCommand3D and,
   while it is pushed, command bit 0x20 or 0x10 by the y of the push. */
static void Salamander_ThinkReturn(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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

static void func_00134DC8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 *point;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (EntityAi_IsPointInView(entity, D_00413F80, entity->params->unk43A * 0.017453292f,
                      entity->params->unk43C * 0.017453292f, entity->params->unk43E) &&
        unkE0->unk6A <= 0) {
        EntityAi_PickRangedAttack(entity);
        // match: an int result keeps $2 busy after the call
        ((int (*)(char *, void (*)(void), int))EntityAi_StartAction)((char *)entity, (void (*)(void))Salamander_RangedAttack, unknown);
        unkE0->unk6A = entity->params->unk438;
        return;
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (entity->unkA0 == 0) {
        point = unkE0->unk30;
        EntityPoint_PickNearWithHeight(point, entity->spawn_info->unk4C, entity->spawn_info->unk56, entity->spawn_info,
                      entity->model->position);
        entity->target = point;
        entity->unkA0 = rand() % 120 + 240;
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->unkC8 = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of states 1 and 2: it has none of EntityAi_ThinkCombat's guard, dodge and draw
   among the actions, but a sequence in work byte unk78. 0: goes for a point 220 below the player
   until the player is nearer than EntityParams.unk434. 1: goes for a point 190 below the player,
   and when EntityAi_IsPointInView finds that point within the angles EntityParams.unk43A and unk43C and the
   distance unk43E, starts Salamander_RangedAttack or Salamander_MeleeAttack as EntityAi_PickAttack
   draws. 2: goes for a point 800 from the player along the reverse of Entity.unk68, at the spawn
   point's y, for EntityParams.unk436 frames (work word unk68) or until the player is unk434 away.
   The commands come from EntityAi_MoveCommand3D, or EntityAi_PushTurnCommand3D while it is pushed. */
static void Salamander_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->unkC8 = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        switch (unkE0->unk78) {
        case 0: {
            f32 *point = unkE0->unk30;

            Vector_Copy(point, D_00413F80);
            unkE0->unk30[1] -= 220.0f;
            entity->target = point;
            entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, entity->radius + 100.0f + 30.0f);
            if (entity->distance_to_player < (f32)ENTITY_CONTROLLER(entity)->unk434) {
                unkE0->unk78++;
            }
            break;
        }
        case 1: {
            f32 *point = unkE0->unk30;

            Vector_Copy(point, D_00413F80);
            unkE0->unk30[1] -= 190.0f;
            entity->target = point;
            if (EntityAi_IsPointInView(entity, point, entity->params->unk43A * 0.017453292f,
                              entity->params->unk43C * 0.017453292f, entity->params->unk43E)) {
                if (EntityAi_PickAttack((char *)entity)) {
                    EntityAi_StartAction(entity, Salamander_RangedAttack, unknown);
                } else {
                    EntityAi_StartAction(entity, Salamander_MeleeAttack, unknown);
                }
                unkE0->unk68 = entity->params->unk436;
                unkE0->unk78++;
                return;
            }
            entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, entity->radius + 100.0f + 30.0f);
            break;
        }
        case 2: {
            f32 *point = unkE0->unk30;

            direction[0] = EntityMath_WrapAngle(entity->unk68 + 3.1415927f);
            EntityMath_FacingVector((Vector4 *)point, direction[0]);
            Vector_Scale(point, point, 800.0f);
            Vector_Add(point, point, D_00413F80);
            entity->target = point;
            unkE0->unk30[1] = entity->spawn_info->spawn_y;
            entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, 50.0f);
            if (entity->distance_to_player >= (f32)ENTITY_CONTROLLER(entity)->unk434 || unkE0->unk68 <= 0) {
                unkE0->unk78 = 0;
            }
            break;
        }
        }
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: turns as the command says, rolls the model 20 degrees into the turn, and
   pitches it up to 30 degrees for command bits 0x10 and 0x20 (through Entity.unk90); keeps
   animation 3 on the first motion slot. It adds no movement of its own. */
static void Salamander_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 value;
    f32 roll;
    f32 pitch;
    f32 pitch_change;
    f32 pitch_rate;

    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
        roll = 0.34906584f;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (unkE0->unk1C > entity->params->turn_limit) {
            unkE0->unk1C = entity->params->turn_limit;
        }
        roll = -0.34906584f;
    } else {
        unkE0->unk1C = 0.0f;
        roll = unkE0->unk1C;
    }
    value = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        value += value;
    }
    entity->unk94 += value;
    EntityMath_EaseAngleToward(&entity->model->unkD0[2], roll, 30.0f);
    if (entity->unkC8 & 0x10) {
        pitch = -0.5235988f;
    } else if (entity->unkC8 & 0x20) {
        pitch = 0.5235988f;
    } else {
        pitch = 0.0f;
    }
    pitch_change = pitch - entity->model->unkD0[0];
    if (ABS(pitch_change) > 0.5235988f) {
        pitch_rate = 60.0f;
    } else {
        pitch_rate = 30.0f;
    }
    value = entity->model->unkD0[0];
    EntityMath_EaseAngleToward(&value, pitch, pitch_rate);
    entity->unk90 += value - entity->model->unkD0[0];
    EntityAi_StartSlotMotion(entity, unkE0->unk80, 0, 3, 0, entity->params->unkC4, 0);
}
