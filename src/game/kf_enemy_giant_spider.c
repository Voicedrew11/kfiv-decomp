/* GiantSpider: the behaviour of entity type 4 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Giant Spider. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
float EntityMath_AngleDifference(float, float);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionHeavy(Entity *);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_StepState(void *);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
extern PlayerData D_00413F80;
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void GiantSpider_Animate(Entity *entity);
static void GiantSpider_OnHit(Entity *entity);
static void GiantSpider_Think(Entity *entity, s32 unknown);
static void GiantSpider_ThinkTurnToPlayer(Entity *entity, s32 unknown);
static void GiantSpider_ThinkCombat(Entity *entity, s32 unknown);
static void GiantSpider_Move(Entity *entity);
static void GiantSpider_PlayTurnAnim6Or11(Entity *entity);
static void GiantSpider_PlayTurnAnim7Or12(Entity *entity);
static void GiantSpider_RangedAttack(Entity *entity);

/* Set-up of a type 4 entity: clears its work area and installs the type's handlers. */
void GiantSpider_Init(Entity *entity) {
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
    unkE0->think = GiantSpider_Think;
    unkE0->animate = (void (*)(void))GiantSpider_Animate;
    unkE0->on_hit = (void (*)(void))GiantSpider_OnHit;
    unkE0->move = (void (*)(void))GiantSpider_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
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
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate, but in place of the head turn it turns the part
   EntityModel.unk138 itself: towards Entity.unk68, at most 30 degrees to either side, eased in work
   float unk20[1]; in animations 14 to 16 it takes the part's own angle instead. */
static void GiantSpider_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;
    EntityModelPart80 *part;
    f32 angle;

    unkE0 = &entity->unkE0;
    flags = entity->flags;
    part = entity->model->unk138;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 2) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        if (part != NULL) {
            if ((u8)(entity->unkD1 - 0xE) < 3) {
                unkE0->unk20[1] = -part->unk50[1];
            } else {
                angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
                if (angle < -0.5235988f) {
                    angle = -0.5235988f;
                }
                if (0.5235988f < angle) {
                    angle = 0.5235988f;
                }
                EntityMath_EaseAngleToward(&unkE0->unk20[1], angle, 10.0f);
                part->unk50[1] = -unkE0->unk20[1];
            }
        }
    }
}

/* The on_hit handler: every hit starts EntityAi_ReactionHeavy, whatever its strength; sets work
   byte unk7A. */
static void GiantSpider_OnHit(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->flags &= ~0x200;
    EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    EntityAi_SetState((char *)entity, 2);
    unkE0->unk7A = 1;
}

/* The think handler: as EntityAi_Think without EntityAi_CheckFarFromSpawn and the return to the spawn point,
   flag bit 9 kept set, with GiantSpider_ThinkCombat for state 2 and GiantSpider_ThinkTurnToPlayer for the other
   states. */
static void GiantSpider_Think(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    entity->flags |= 0x200;
    EntityAi_StepState(entity);
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        GiantSpider_ThinkCombat(entity, unknown);
        return;
    }
    GiantSpider_ThinkTurnToPlayer(entity, unknown);
}

/* The think function of every state but 2 (GiantSpider_Think calls it): turns the entity to
   Entity.unk68, the bearing the shared code turns to when it turns to the player. When unk68 is
   more than 25 degrees off the heading it starts GiantSpider_PlayTurnAnim7Or12 (positive side) or
   GiantSpider_PlayTurnAnim6Or11; else it starts the move action with no command. */
static void GiantSpider_ThinkTurnToPlayer(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (ABS(angle) > 0.43633232f) {
        if (angle >= 0.0f) {
            EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim7Or12, unknown);
        } else {
            EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim6Or11, unknown);
        }
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
    }
}

/* The think function of state 2: guards the player's swing and picks among attacking, closing in
   and waiting as EntityAi_ThinkCombat does; it has no dodge, and its ranged attack is
   GiantSpider_RangedAttack. It gives no command: where that one closes in, this one starts
   GiantSpider_PlayTurnAnim7Or12 or GiantSpider_PlayTurnAnim6Or11 (by the side) when Entity.unk68 is more than
   25 degrees off the heading. An attack clears work byte unk7A. */
static void GiantSpider_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
                EntityAi_StartAction(entity, GiantSpider_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            unkE0->unk7A = 0;
            return;
        case 1:
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.43633232f) {
                if (angle >= 0.0f) {
                    EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim7Or12, unknown);
                    return;
                }
                EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim6Or11, unknown);
                return;
            }
            unkE0->unk7A = 0;
            unkE0->unk78 = -1;
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
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: only picks the animation: 12 or 11 for command bit 0x100 or 0x200, 7 or 6 when
   bit 0x400 is set as well, else 0. */
static void GiantSpider_Move(Entity *entity) {
    u16 command;

    command = entity->unkC8;
    if (command & 0x100) {
        if (command & 0x400) {
            Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 0);
            return;
        }
        Motion_Start(entity, 0xC, 0, entity->params->unkC4, 0);
        return;
    }
    if (command & 0x200) {
        if (command & 0x400) {
            Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 0);
            return;
        }
        Motion_Start(entity, 0xB, 0, entity->params->unkC4, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
}

/* An action (busy bit 0x1000), which GiantSpider_ThinkCombat and GiantSpider_ThinkTurnToPlayer start when
   Entity.unk68 is more than 25 degrees off the heading on the negative side: plays animation 6 when
   work byte unk7A is set (GiantSpider_OnHit sets it), else animation 11, to its end, with flag bits
   11 to 13 at 3. */
static void GiantSpider_PlayTurnAnim6Or11(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7A != 0) {
            Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 3;
        ENTITY_UNK5C(entity)->as_s32 = 0;
        entity->action_step++;
        break;
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

/* An action (busy bit 0x2000), the same as GiantSpider_PlayTurnAnim6Or11 for the positive side: plays
   animation 7 when work byte unk7A is set, else animation 12, to its end. */
static void GiantSpider_PlayTurnAnim7Or12(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7A != 0) {
            Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 1);
        } else {
            Motion_Start(entity, 0xC, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 3;
        ENTITY_UNK5C(entity)->as_s32 = 0;
        entity->action_step++;
        break;
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, the shots turned towards a point 160
   below the player by at most 15 degrees (EntityShot_LaunchAimed). */
static void GiantSpider_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
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
        Vector_Copy(direction, &D_00413F80);
        direction[1] -= 160.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction, 0.2617994f, 0.2617994f);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
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
