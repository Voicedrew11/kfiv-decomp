/* DarkKnight: the behaviour of entity type 64 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Knight. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionHeavy(Entity *);
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
int rand(void);
float sqrtf(float);
float atan2f(float, float);
char *func_0010CAB8(char *, u32);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
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
void func_00196D48(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleDifference(float, float);
float EntityMath_WrapAngle(float);
void EntityAi_Dodge(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHitAt(Entity *, void *, s32, s32, s32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
s32 EntityAi_CanMelee(Entity *);
s32 EntityAi_CanShoot(Entity *);
int EntitySight_EntityInWay(int *);
s32 EntityAi_CanUseMelee(Entity *, s32);
s32 EntityAi_CanUseRanged(Entity *, s32);
void EntityAi_LowerAttackWeight(Entity *);

static void DarkKnight_OnHit(Entity *entity);
static void DarkKnight_Think(Entity *entity, s32 unknown);
static void DarkKnight_ThinkCombat(Entity *entity, s32 unknown);
static void DarkKnight_PlayTurnAnim(Entity *entity);
static void func_001639A8(Entity *entity);
static void func_00163B28(Entity *entity);
static void DarkKnight_MeleeAttack2(Entity *entity);
static void DarkKnight_RangedAttack0(Entity *entity);
static void DarkKnight_RangedAttack1(Entity *entity);
static s32 DarkKnight_ShouldRangedAttack0(Entity *entity);
static s32 DarkKnight_ShouldHoldAnim13(Entity *entity, f32 min_distance, f32 max_angle);
static s32 func_00164A60(Entity *entity, s32 mode);
static s32 func_00164C88(Entity *entity);

/* Set-up of a type 64 entity: clears its work area and installs the type's handlers. */
void DarkKnight_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = DarkKnight_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))DarkKnight_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~0x10000000;
    if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
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

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy); a light hit only puts the entity in state 2. */
static void DarkKnight_OnHit(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_LowerWeightsAfterHit((char *)entity);
        entity->target = NULL;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler: as EntityAi_Think, with DarkKnight_ThinkCombat for state 2. Before that,
   every frame: it starts effect 0x3D7 at model points 30 and 31, pointing to points 28 and 29;
   while work word unk68 counts down (DarkKnight_RangedAttack0 sets it), for the first
   EntityParams.unk436 frames it starts one of effects 0x21F to 0x221 at point 4 and writes
   EntityParams.unk438 into unk10[n].unk00.unk00[8] of the three melee records, and 0 once the count
   is over; it counts down work word unk6C, set to EntityParams.unk43A while D_00413F80.unk259 is
   not 0; and while work word unk6A counts down it keeps flag bit 35 set, with effect 0x25C every
   third frame. */
static void DarkKnight_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind162BC8 *aims = (EntityUnkE0Kind162BC8 *)unkE0;
    f32 delta[4];
    f32 *from;
    f32 *tip;
    f32 *first_aim;
    f32 *second_aim;
    u16 frames;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    unkE0->unk40 = 0.5f;
    first_aim = aims->unk80;
    second_aim = aims->unk90;
    from = (f32 *)func_0010CAB8((char *)entity->model, 0x1E);
    tip = (f32 *)func_0010CAB8((char *)entity->model, 0x1C);
    delta[0] = tip[0] - from[0];
    delta[1] = tip[1] - from[1];
    delta[2] = tip[2] - from[2];
    first_aim[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
    first_aim[1] = atan2f(delta[0], delta[2]);
    first_aim[2] = 0.0f;
    from = (f32 *)func_0010CAB8((char *)entity->model, 0x1F);
    tip = (f32 *)func_0010CAB8((char *)entity->model, 0x1D);
    delta[0] = tip[0] - from[0];
    delta[1] = tip[1] - from[1];
    delta[2] = tip[2] - from[2];
    second_aim[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
    second_aim[1] = atan2f(delta[0], delta[2]);
    second_aim[2] = 0.0f;
    EntityEvent_StartEffectAtPoint((char **)entity, 0x3D7, 0x1E, (int)first_aim);
    EntityEvent_StartEffectAtPoint((char **)entity, 0x3D7, 0x1F, (int)second_aim);
    if (unkE0->unk68 > 0) {
        if (unkE0->unk68 > (s16)entity->params->unk434 - entity->params->unk436) {
            from = (f32 *)func_0010CAB8((char *)entity->model, 4);
            tip = (f32 *)func_0010CAB8((char *)entity->model, 5);
            delta[0] = tip[0] - from[0];
            delta[1] = tip[1] - from[1];
            delta[2] = tip[2] - from[2];
            aims->unk30[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
            aims->unk30[1] = atan2f(delta[0], delta[2]);
            aims->unk30[2] = 0.0f;
            EntityEvent_StartEffectAtPoint((char **)entity, rand() % 3 + 0x21F, 4, (int)aims->unk30);
            frames = entity->params->unk438;
            entity->params->unk240[0].unk10[0].unk00.unk00[8] = frames;
            entity->params->unk240[0].unk10[1].unk00.unk00[8] = frames;
            entity->params->unk240[0].unk10[2].unk00.unk00[8] = frames;
            entity->params->unk240[1].unk10[0].unk00.unk00[8] = frames;
            entity->params->unk240[1].unk10[1].unk00.unk00[8] = frames;
            entity->params->unk240[1].unk10[2].unk00.unk00[8] = frames;
            ((EntityParamsUnk240 *)entity->params->unk350)->unk10[0].unk00.unk00[8] = frames;
            ((EntityParamsUnk240 *)entity->params->unk350)->unk10[1].unk00.unk00[8] = frames;
            ((EntityParamsUnk240 *)entity->params->unk350)->unk10[2].unk00.unk00[8] = frames;
        }
        unkE0->unk68--;
    } else {
        entity->params->unk240[0].unk10[0].unk00.unk00[8] = 0;
        entity->params->unk240[0].unk10[1].unk00.unk00[8] = 0;
        entity->params->unk240[0].unk10[2].unk00.unk00[8] = 0;
        entity->params->unk240[1].unk10[0].unk00.unk00[8] = 0;
        entity->params->unk240[1].unk10[1].unk00.unk00[8] = 0;
        entity->params->unk240[1].unk10[2].unk00.unk00[8] = 0;
        ((EntityParamsUnk240 *)entity->params->unk350)->unk10[0].unk00.unk00[8] = 0;
        ((EntityParamsUnk240 *)entity->params->unk350)->unk10[1].unk00.unk00[8] = 0;
        ((EntityParamsUnk240 *)entity->params->unk350)->unk10[2].unk00.unk00[8] = 0;
    }
    if (unkE0->unk6C > 0) {
        unkE0->unk6C--;
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk6C = entity->params->unk43A;
    }
    if (unkE0->unk6A > 0) {
        if (D_00446ED4 % 3U == 0) {
            EntityEvent_StartEffectAtPoint((char **)entity, 0x25C, 0, (int)entity->model->unkD0);
        }
        ENTITY_FLAGS(entity) |= 0x8000L << 20;
        unkE0->unk6A--;
    } else {
        ENTITY_FLAGS(entity) &= ~(0x8000L << 20);
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
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
                DarkKnight_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, with these changes. In place of the guard
   it starts func_001639A8 when DarkKnight_ShouldHoldAnim13 says so; it starts
   DarkKnight_RangedAttack0 whenever DarkKnight_ShouldRangedAttack0 says so; ranged index 1 has
   DarkKnight_RangedAttack1 and melee index 3 DarkKnight_MeleeAttack2; closing in from more than
   500 away adds command bits 0x40 and 0x400; and with Entity.unk68 more than 90 degrees off the
   heading it may start DarkKnight_PlayTurnAnim (work byte unk7B: the side). */
static void DarkKnight_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind162BC8 *work = (EntityUnkE0Kind162BC8 *)unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    entity->flags = (entity->flags & ~0x3800) | 0x800;
    if (DarkKnight_ShouldHoldAnim13(entity, 200.0f, 2.443461f)) {
        EntityAi_StartAction(entity, func_001639A8, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (DarkKnight_ShouldRangedAttack0(entity)) {
        EntityAi_StartAction(entity, DarkKnight_RangedAttack0, unknown);
        return;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    entity->flags |= 0x200;
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 1) {
                    EntityAi_StartAction(entity, DarkKnight_RangedAttack1, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                }
            } else {
                if (((s32)(entity->flags >> 58) & 3) == 3) {
                    EntityAi_StartAction(entity, DarkKnight_MeleeAttack2, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
                }
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            unkE0->unk78 = 0;
            if (entity->distance_to_player > 500.0f) {
                entity->unkA0 = rand() % 30 + 60;
                work->unk6E = 1;
            } else {
                entity->unkA0 = rand() % 30 + 60;
                work->unk6E = 0;
            }
            break;
        case 2:
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        side = EntityAi_TurnFromPush(entity, direction);
        func_00196D48(entity);
        if ((s32)(entity->flags >> 31) & 1) {
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
        } else if (work->unk6E != 0) {
            if (side > 0) {
                entity->unkC8 = 0x241;
            } else if (side < 0) {
                entity->unkC8 = 0x141;
            } else {
                if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                    && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                    unkE0->unk7B = (EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, DarkKnight_PlayTurnAnim, unknown);
                    return;
                }
                entity->unkC8 = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
            }
            entity->unkC8 |= 0x400;
        } else {
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                    && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                    unkE0->unk7B = (EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, DarkKnight_PlayTurnAnim, unknown);
                    return;
                }
                entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x1000) that DarkKnight_ThinkCombat starts: plays animation 12 or 11, by
   the sign of work byte unk7B, to its end. */
static void DarkKnight_PlayTurnAnim(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7B > 0) {
            Motion_Start(entity, 0xC, 0, entity->params->unkC4, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

static void func_001639A8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xD, 0, 0xA, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = entity->params->unkCE;
            ENTITY_FLAGS(entity) &= ~0x4000;
            unkE0->unk6A = entity->params->unk43C;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->unkA0 == 0) {
            ENTITY_FLAGS(entity) |= 0x4000;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

static void func_00163B28(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[unkE0->unk7A];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        if (ABS(entity->unk68 - entity->model->unkD0[1]) < 0.5235988f) {
            Motion_Start(entity, 0x11, 0, 5, 1);
            unkE0->unk7A = 0;
        } else if (entity->unk68 - entity->model->unkD0[1] > 0.5235988f && entity->unk68 - entity->model->unkD0[1] < 1.5707964f) {
            Motion_Start(entity, 0x12, 0, 5, 1);
            unkE0->unk7A = 2;
        } else if (entity->unk68 - entity->model->unkD0[1] < -0.5235988f && entity->unk68 - entity->model->unkD0[1] > -1.5707964f) {
            Motion_Start(entity, 0x13, 0, 5, 1);
            unkE0->unk7A = 2;
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 1;
        entity->action_step++;
        break;
    case 1:
        unkE0->unk40 = 0.5f;
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (unkE0->unk7A == 0) {
                Motion_Start(entity, 0x14, 0xE, 0, 1);
            } else if (unkE0->unk7A == 2) {
                Motion_Start(entity, 0x16, 0xE, 0, 1);
            }
            entity->action_step++;
        }
        break;
    case 3:
        unkE0->unk40 = 0.5f;
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 4:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            if (!ENTITY_FLAG_BITS(entity)->bit54) {
                ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f));
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
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

/* The melee attack with record 2 (EntityParams.unk350; busy bit 1), which DarkKnight_ThinkCombat
   starts for attack index 3: animation 22; part 0 is checked along the heading and, when it has not
   hit, part 1 along the reverse of the heading; part 2 along the heading (EntityAi_CheckMeleeHitAt). */
static void DarkKnight_MeleeAttack2(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = (EntityParamsUnk240 *)entity->params->unk350;
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x16, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            if (!ENTITY_FLAG_BITS(entity)->bit54) {
                ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f));
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
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

/* The ranged attack with record 0 (busy bit 1): as EntityAi_RangedAttack with animation 23; at its
   end it also sets work word unk68 to EntityParams.unk434. */
static void DarkKnight_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        EntityAi_LaunchShots(entity, unk3D8,
                      (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
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
            unkE0->unk68 = entity->params->unk434;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 1 (busy bit 1): as EntityAi_RangedAttack with animation 24, but the
   shots are launched along the reverse of the heading (EntityShot_Launch). */
static void DarkKnight_RangedAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x18, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAGS(entity) &= ~0x3800;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(direction, entity->model->unkD0);
        direction[1] -= 3.1415927f;
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction);
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

/* Helper of DarkKnight_ThinkCombat: whether to start DarkKnight_RangedAttack0, which is
   whenever the count in work word unk68 has run out. */
static s32 DarkKnight_ShouldRangedAttack0(Entity *entity) {
    return entity->unkE0.unk68 < 1;
}

/* Helper of DarkKnight_ThinkCombat: whether to start func_001639A8, the action (busy bit 0x10)
   that starts animation 13 and stops the motion for EntityParams.unkCE frames. Only while work word
   unk6C runs and unk6A does not, the entity is not busy, the player is at least min_distance away
   (less the radius and 100) and Entity.unk68 is within max_angle of the heading; then
   EntityParams.unkCB / 5 times in 100. */
static s32 DarkKnight_ShouldHoldAnim13(Entity *entity, f32 min_distance, f32 max_angle) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 distance;
    f32 angle;

    if (unkE0->unk6C <= 0) {
        return FALSE;
    }
    if (unkE0->unk6A > 0) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCB / 5;
}

static s32 func_00164A60(Entity *entity, s32 mode) {
    s32 chances[4];
    s32 weights[4];
    s32 choice = -1;
    s32 i;
    s32 total;
    s32 fade;
    s32 elapsed;
    s32 roll;
    s32 sum;

    for (i = 0; i < 4; i++) {
        weights[i] = entity->params->unk14[i];
    }
    if (!EntityAi_CanMelee(entity) && (entity->unkA2 != 0 || !EntityAi_CanShoot(entity))) {
        weights[0] = 0;
    }
    if (mode == 0) {
        if ((u32)(entity->flags >> 63) || EntitySight_EntityInWay((int *)entity)) {
            weights[0] = 0;
        }
    } else if (mode == 1) {
        if (EntitySight_EntityInWay((int *)entity)) {
            weights[0] = 0;
        }
    } else if (mode == 2) {
        if ((u32)(entity->flags >> 63)) {
            weights[0] = 0;
        }
    }
    total = 0;
    if (weights[0] + weights[1] + weights[2] + weights[3] == 0) {
        return -1;
    }
    for (i = 0; i < 4; i++) {
        if (i == entity->unkCD) {
            elapsed = (u16)D_00446ED4 - entity->unk9E;
            if (elapsed < 240) {
                fade = 0x80 - (elapsed * (elapsed << 7)) / 57600;
                fade += 0x80;
            } else {
                fade = 0x80;
            }
        } else {
            fade = 0x80;
        }
        chances[i] = (fade * ((entity->unkBC[i] * weights[i]) >> 6)) >> 7;
        total += chances[i];
    }
    if (total == 0) {
        return -1;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 4; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    return choice;
}

/* Picks the next attack by weighted chance: three melee attacks (EntityParams.unk240), then the
   three ranged ones, each only if its test passes; the choice goes into flag bits 57-59. */
static s32 func_00164C88(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 i;
    s32 roll;
    s32 sum;
    s32 chance;
    s32 slot;

    for (i = 0; i < 6; i++) {
        if (i < 3) {
            slot = i;
            if (EntityAi_CanUseMelee(entity, slot)) {
                chance = (entity->unkC0[i] * entity->params->unk240[slot].unk00) >> 6;
            } else {
                chance = 0;
            }
        } else if (entity->unkA2 != 0) {
            chance = 0;
        } else {
            slot = i - 3;
            if (!EntityAi_CanUseRanged(entity, slot)) {
                chance = 0;
            } else {
                chance = (entity->unkC0[i] * ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[slot].record.unk00) >> 6;
            }
        }
        chances[i] = chance;
        total += chance;
    }
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->bits58 = choice;
        ENTITY_FLAG_BITS(entity)->bit57 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits58 = choice - 3;
        ENTITY_FLAG_BITS(entity)->bit57 = 1;
    }
    EntityAi_LowerAttackWeight(entity);
    return (s32)(entity->flags >> 57) & 1;
}
