/* FireElemental: the behaviour of entity type 48 (Entity.params->unk02, the index into D_003E6E40): its
   set-up function, which the table names, and the handlers it installs. Used by: Fire Elemental. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
float EntityMath_WrapAngle(float);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleXZ(float *, char *);
extern u8 D_004141D8[];
extern EntityModel *D_00446978;
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void EntityAi_SetAttackDelay(Entity *, s32);
float EntityMath_DistanceXZ(float *a, float *b);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
float EntityMath_HeadingTo(float *, float *);
void Vector_Normalize(void *, void *);
void Vector_Cross(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);

static void FireElemental_Think(Entity *entity, s32 unknown);
static void FireElemental_ThinkCombat(Entity *entity, s32 unknown);
static void FireElemental_StandAndAttack(Entity *entity);
static void FireElemental_PlayTurnAnim(Entity *entity);
static void FireElemental_Dodge(Entity *entity);
static s32 func_0015A790(Entity *entity, f32 min_distance, f32 max_angle);
static s32 FireElemental_ShouldDodge(Entity *entity, f32 max_distance, f32 max_angle);
static void FireElemental_RangedAttack1(Entity *entity);
static void FireElemental_AlertSameKind(Entity *entity);
static void FireElemental_OnHit(Entity *entity);
static void FireElemental_BackAwayFromPlayer(Entity *entity);
static void FireElemental_Move(Entity *entity);
static s32 func_0015B340(Entity *entity, s32 unused, f32 reach);
static void FireElemental_ThinkReturn(Entity *entity, s32 unknown);
static void FireElemental_RangedAttack0(Entity *entity);

/* Set-up of a type 48 entity: clears its work area and installs the type's handlers. */
void FireElemental_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = FireElemental_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))FireElemental_OnHit;
    unkE0->move = (void (*)(void))FireElemental_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
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
    unkE0->unk7A = 0;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with FireElemental_ThinkReturn for flag bit 53 and its own
   state 2: FireElemental_StandAndAttack while work byte unk7A is 0 (states 0 and others clear it),
   then FireElemental_ThinkCombat. Work byte unk7C is put to 30 while D_00413F80.unk259 is set and
   counts down. While work word unk68 is 1 (FireElemental_AlertSameKind of another entity sets it) it
   starts FireElemental_BackAwayFromPlayer instead of all that. */
static void FireElemental_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7C = 30;
    }
    if (unkE0->unk7C > 0) {
        unkE0->unk7C--;
    }
    if (unkE0->unk68 == 1) {
        if (entity->unkC6 == 0) {
            EntityAi_StartAction(entity, FireElemental_BackAwayFromPlayer, 1);
            return;
        }
    } else if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            FireElemental_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                unkE0->unk7A = 0;
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                if (unkE0->unk7A == 0) {
                    EntityAi_StartAction(entity, FireElemental_StandAndAttack, unknown);
                } else {
                    FireElemental_ThinkCombat(entity, unknown);
                }
                break;
            default:
                unkE0->unk7A = 0;
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but it first starts EntityAi_Dodge when
   func_0015A790 says so, ranged record 1 is FireElemental_RangedAttack1, and closing in gives the
   backwards command (bit 2, from func_0015B340) where the shared one gives the forwards one. */
static void FireElemental_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (func_0015A790(entity, 200.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                    EntityAi_StartAction(entity, FireElemental_RangedAttack1, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                }
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
                entity->unkC8 = 0x202;
            } else if (turn < 0) {
                entity->unkC8 = 0x102;
            } else {
                // match: called like EntityAi_MoveCommand, with a mode the callee ignores
                entity->unkC8 = ((s16 (*)(float, void *, int))func_0015B340)(entity->radius + 100.0f + 30.0f, entity, 1);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x1000) that FireElemental_Think starts in state 2 while work byte unk7A is 0:
   plays animation 17 turning to the player, then stays in its last step and decides each frame. It
   sets unk7A and starts FireElemental_Dodge when func_0015A790 or FireElemental_ShouldDodge
   says so, or EntityAi_MeleeAttack with record 1 when the player is within EntityParams.unk436 plus
   100 plus the radius and 20 degrees ahead. Each time the timer runs out it starts
   FireElemental_RangedAttack0 (and sets unk7A) when the pause in work word unk6A is over and the
   player is within 20 degrees ahead, starts FireElemental_PlayTurnAnim when the player is more than
   90 degrees off, and else turns towards the player in animation 4 or stands in animation 0 for 30
   to 59 frames. */
static void FireElemental_StandAndAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s8 turn;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk6A = entity->params->unk438 / 2 + rand() % (entity->params->unk438 / 2);
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 1;
        if (func_0015A790(entity, 200.0f, 1.0471976f) || FireElemental_ShouldDodge(entity, 300.0f, 0.5235988f)) {
            unkE0->unk7A = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_StartAction(entity, FireElemental_Dodge, 1);
            break;
        }
        if (entity->distance_to_player < (f32)entity->params->unk436 + 100.0f + entity->radius &&
            ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) < 0.34906584f) {
            unkE0->unk7A = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bits58 = 1;
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
            break;
        }
        if (entity->unkA0 == 0) {
            if (unkE0->unk6A == 0 && ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) < 0.34906584f) {
                unkE0->unk7A = 1;
                unkE0->unk6A = entity->params->unk438 / 2 + rand() % (entity->params->unk438 / 2);
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                EntityAi_StartAction(entity, FireElemental_RangedAttack0, 1);
                break;
            }
            if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f) {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                unkE0->unk7B = EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1]) > 0.0f ? 1 : -1;
                EntityAi_StartAction(entity, FireElemental_PlayTurnAnim, 1);
                break;
            }
            if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) > 0.34906584f) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
            } else {
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = -1;
            }
        }
        turn = EntityAi_TurnFromPush(entity, direction);
        switch (unkE0->unk78) {
        case 0:
            Motion_Start(entity, 4, 0, entity->params->unkC4, 0);
            if (turn > 0) {
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
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
            direction[0] = unkE0->unk1C;
            if (entity->unkC8 & 0x400) {
                direction[0] += direction[0];
            }
            entity->unk94 += direction[0];
            unkE0->unk44 = 0.0f;
            break;
        case -1:
            Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
            unkE0->unk1C = 0.0f;
            unkE0->unk44 = 0.0f;
            break;
        }
        break;
    }
}

/* An action (busy bit 0x2000): plays animation 12 or, with work byte unk7B below 1, animation 11 to
   its end. FireElemental_StandAndAttack starts it when the player is more than 90 degrees off the
   heading. */
static void FireElemental_PlayTurnAnim(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
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

/* The dodge action (busy bit 8): as EntityAi_Dodge; when it ends, one time in two and with the
   player within 30 degrees of the heading, it starts FireElemental_RangedAttack1 with record 1.
   FireElemental_StandAndAttack starts it; the think function uses EntityAi_Dodge. */
static void FireElemental_Dodge(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xA, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (!(rand() & 1) && ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) < 0.5235988f) {
                ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~(3L << 58)) | (1L << 58);
                EntityAi_StartAction(entity, FireElemental_RangedAttack1, 1);
            } else if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

static s32 func_0015A790(Entity *entity, f32 min_distance, f32 max_angle) {
    f32 distance;
    f32 angle;
    f32 player_angle;

    if (entity->unkE0.unk7C <= 0) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    player_angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (ABS(player_angle) > 0.5235988f) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCA / 10;
}

/* Helper of FireElemental_StandAndAttack: whether to dodge the player's swing: the player within
   max_distance and max_angle of the heading, D_004141D8[0] set and the motion of D_00446978 before
   frame 6, then a chance of EntityParams.unkCA / 5 in 100. */
static s32 FireElemental_ShouldDodge(Entity *entity, f32 max_distance, f32 max_angle) {
    f32 distance;
    f32 angle;

    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (distance > max_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCA / 5;
}

/* The ranged attack with record 1 (busy bit 1): it takes the record from flag bits 58-59 as
   EntityAi_RangedAttack does, but every place that starts it does so for record 1. It turns to the
   player all through the blend and launches the shots with EntityShot_Launch along the reverse of its
   heading plus 5 degrees. */
static void FireElemental_RangedAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 1, 1);
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
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(direction, entity->model->unkD0);
        direction[1] = EntityMath_WrapAngle(entity->model->unkD0[1] - 3.1415927f + 0.08726646f);
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

/* Helper of FireElemental_OnHit: sets work bytes unk7A and unk68 to 1 in every other live entity
   with the same EntityParams that is within EntityParams.unk434 of this one, has unk68 at 0 and has
   the player within EntitySpawnInfo.unk4C of its spawn point. */
static void FireElemental_AlertSameKind(Entity *entity) {
    Entity *other;
    EntityUnkE0 *unkE0;

    for (other = D_004467E0; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & 1) && ((s32)(other->flags >> 1) & 1)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            unkE0 = &other->unkE0;
            if (unkE0->unk68 == 0
                && EntityMath_DistanceXZ(entity->model->position, other->model->position) < (s16)entity->params->unk434
                && EntityMath_DistanceXZ((float *)other->spawn_info, (float *)&D_00413F80) < other->spawn_info->unk4C) {
                unkE0->unk7A = unkE0->unk68 = 1;
            }
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, then FireElemental_AlertSameKind. */
static void FireElemental_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
    FireElemental_AlertSameKind(entity);
}

/* An action (busy bit 0x3000) that FireElemental_Think starts while work word unk68 is 1: plays
   animation 5, then each frame guards or dodges the player's swing as FireElemental_ThinkCombat
   does, starts the attack EntityAi_PickAttack picks once EntityAi_PickCombatChoice chooses attacking (unk68 becomes
   2; ranged record 1 is FireElemental_RangedAttack1), and else gives the backwards command towards
   the player and calls the move handler itself. Flag bit 53 ends it with unk68 at 2. */
static void FireElemental_BackAwayFromPlayer(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 5, 0, entity->params->unkC4, 0);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (func_0015A790(entity, 200.0f, 1.0471976f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Guard, 1);
            break;
        }
        if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            unkE0->unk68 = 2;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        side = EntityAi_PickCombatChoice(entity, 0);
        ENTITY_FLAGS(entity) |= 0x200;
        if (side == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = 0;
            if (entity->unkCF != 0) {
                entity->unk9E = D_00446ED4;
            }
            EntityAi_LowerChoiceWeight(entity, 0);
            if (entity->unkCD == 0) {
                unkE0->unk68 = 2;
                if (EntityAi_PickAttack((char *)entity)) {
                    if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                        EntityAi_StartAction(entity, FireElemental_RangedAttack1, 1);
                    } else {
                        EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
                    }
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
                }
            }
            break;
        }
        entity->target = &D_00413F80;
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->unkC8 = 0x202;
        } else if (side < 0) {
            entity->unkC8 = 0x102;
        } else {
            // match: called like EntityAi_MoveCommand, with a mode the callee ignores
            entity->unkC8 = ((s16 (*)(float, void *, int))func_0015B340)(entity->radius + 100.0f + 30.0f, entity, 1);
        }
        ((void (*)(Entity *))unkE0->move)(entity);
        break;
    }
}

/* The move handler: as EntityAi_Move, but it only turns the entity and picks the animation, it does
   not move it; going backwards is animation 18. */
static void FireElemental_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
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
    unkE0->unk44 = 0.0f;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        Motion_Start(entity, 0x12, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        Motion_Start(entity, 8, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        Motion_Start(entity, 9, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        Motion_Start(entity, 6, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 8) {
        Motion_Start(entity, 7, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}

/* The movement command for walking to the target: a turn bit when the target is off the heading by more than
   the turn limit, and bit 1 while it is farther away than reach. */
static s32 func_0015B340(Entity *entity, s32 unused, f32 reach) {
    f32 turn;
    f32 limit;
    f32 heading;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    heading = EntityMath_HeadingTo(entity->model->position, entity->target);
    turn = EntityMath_AngleDifference(entity->model->unkD0[1], heading);
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

/* The think function while flag bit 53 is set: as EntityAi_ThinkReturn (walk back to the spawn
   point, turning away from whatever is pushing the entity), but with the backwards command of
   func_0015B340 where the shared one gives the forwards one. */
static void FireElemental_ThinkReturn(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        // match: called like EntityAi_MoveCommand, with a mode the callee ignores
        entity->unkC8 = ((s16 (*)(float, void *, int))func_0015B340)(entity->radius + 100.0f + 30.0f, entity, 1);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (0xC000L << 31)) {
            push[0] += entity->unkA4[0];
            push[2] += entity->unkA4[2];
        }
        if ((s32)(entity->flags >> 48) & 1) {
            push[0] += entity->unkB0[0];
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
            entity->unkC8 = 0x202;
        } else if (turn < 0) {
            entity->unkC8 = 0x102;
        } else {
            // match: called like EntityAi_MoveCommand, with a mode the callee ignores
            entity->unkC8 = ((s16 (*)(float, void *, int))func_0015B340)(entity->radius + 100.0f + 30.0f, entity, 1);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The ranged attack with record 0 (busy bit 1): animation 23, as EntityAi_RangedAttack, but the
   shots are launched with EntityShot_Launch along the reverse of its heading. FireElemental_StandAndAttack
   starts it. */
static void FireElemental_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 direction[4];
    s32 elapsed;

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
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(direction, entity->model->unkD0);
        direction[1] = EntityMath_WrapAngle(direction[1] - 3.1415927f);
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
