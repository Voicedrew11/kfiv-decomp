/* DarkSoldier: the behaviour of entity type 44 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Soldier. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern PlayerData D_00413F80;
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityPoint_PickNear(float, void *, void *, void *);
void func_00196D48(Entity *);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s16 EntityAi_MoveCommand(float, void *, int); // match: this file only matches with this type; EntityAi_MoveCommand.c defines it differently
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
extern int D_00414184[];
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityMath_FacingVector(Vector4 *, float);
/* one frame of the arc of DarkSoldier_MoveAlongArc: forwards at unk44 along unk30, in y at minus
   unk48 (up); both wear off */
static inline void move_along_arc(Entity *entity, EntityUnkE0 *unkE0) {
    entity->unk84 = unkE0->unk44 * unkE0->unk30[0];
    entity->unk88 = -unkE0->unk48;
    entity->unk8C = unkE0->unk44 * unkE0->unk30[2];
    unkE0->unk48 -= entity->params->unk43E * 0.1f;
    unkE0->unk44 -= 0.5f;
    if (unkE0->unk44 < 0.0f) {
        unkE0->unk44 = 0.0f;
    }
}
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
void Vector_Add(void *, void *, void *);
f32 EntityMath_Distance(f32 *, f32 *);
void Vector_Normalize(void *, void *);
int EntityCollision_RayProps(void *, void *, void *, float); // match: the distance is the last argument (set up after the pointers)

static void DarkSoldier_Think(Entity *entity, s32 unknown);
static void DarkSoldier_ThinkCombat(Entity *entity, s32 unknown);
static void DarkSoldier_Move(Entity *entity);
static void DarkSoldier_MeleeAttack(Entity *entity);
static void DarkSoldier_PlayAnim6Or7BySide(Entity *entity);
static void DarkSoldier_MoveAlongArc(Entity *entity);
static void DarkSoldier_Dodge(Entity *entity);
static void DarkSoldier_OnHit(Entity *entity);
static s32 DarkSoldier_CanMoveAlongArc(Entity *entity);

/* Set-up of a type 44 entity: clears its work area and installs the type's handlers. */
void DarkSoldier_Init(Entity *entity) {
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
    unkE0->think = DarkSoldier_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))DarkSoldier_OnHit;
    unkE0->move = (void (*)(void))DarkSoldier_Move;
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

/* The think handler: as EntityAi_Think, with DarkSoldier_ThinkCombat for state 2; walking back to
   the spawn point (flag bit 53) gets command bits 0x440 added. Work byte unk7A goes to 0 with the
   player nearer than the radius plus 400 and to 1 with the player farther than the radius plus
   1100. */
static void DarkSoldier_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->distance_to_player < entity->radius + 300.0f + 100.0f && unkE0->unk7A == 1) {
        unkE0->unk7A = 0;
    } else if (entity->radius + 1000.0f + 100.0f < entity->distance_to_player && unkE0->unk7A == 0) {
        unkE0->unk7A = 1;
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if ((s32)(entity->flags >> 53) & 1) {
            // match: called as returning nothing, so $2 is free for the next statement
            ((void (*)(void *, int))EntityAi_ThinkReturn)(entity, unknown);
            entity->unkC8 |= 0x440;
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                DarkSoldier_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but the dodge is DarkSoldier_Dodge, and
   each time the timer is at 0 it may first start DarkSoldier_PlayAnim6Or7BySide (a chance of
   EntityParams.unk436 in 100, the entity within 60 degrees of the player's facing and the player
   within 60 of its heading) or DarkSoldier_MoveAlongArc (a chance of unk438 in 100, within 90
   degrees of the player's facing, EntitySpawnInfo.unk40 at 0 and DarkSoldier_CanMoveAlongArc
   true). A melee attack draws for the same two actions first, with work byte unk7B at 1 so that the
   attack follows them. In place of waiting it goes to a point EntityPoint_PickNear picks round the spawn
   point; closing in is at the run with command bit 0x400. */
static void DarkSoldier_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 point[4];
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, DarkSoldier_Dodge, 1);
        return;
    }
    if (entity->unkA0 == 0) {
        angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
        if (rand() % 100 < entity->params->unk436 && ABS(angle) < 1.0471976f
            && ABS(entity->unk68 - entity->model->unkD0[1]) < 1.0471976f) {
            unkE0->unk7B = 0;
            EntityAi_StartAction(entity, DarkSoldier_PlayAnim6Or7BySide, unknown);
            return;
        }
        if (rand() % 100 < entity->params->unk438 && ABS(angle) < 1.5707964f && entity->spawn_info->unk40 == 0
            && DarkSoldier_CanMoveAlongArc(entity)) {
            unkE0->unk7B = 0;
            EntityAi_StartAction(entity, DarkSoldier_MoveAlongArc, unknown);
            return;
        }
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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                return;
            }
            angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
            if (rand() % 100 < entity->params->unk436 && ABS(angle) < 0.6981317f) {
                unkE0->unk7B = 1;
                EntityAi_StartAction(entity, DarkSoldier_PlayAnim6Or7BySide, unknown);
                return;
            }
            if (rand() % 100 < entity->params->unk438 && ABS(angle) < 1.5707964f && entity->spawn_info->unk40 == 0
                && DarkSoldier_CanMoveAlongArc(entity)) {
                unkE0->unk7B = 1;
                EntityAi_StartAction(entity, DarkSoldier_MoveAlongArc, unknown);
                return;
            }
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 60;
            unkE0->unk78 = 0;
            break;
        case 2:
            EntityPoint_PickNear(entity->spawn_info->unk4C, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
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
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity->radius + 100.0f + 30.0f, entity, 1);
            }
            entity->unkC8 |= 0x400;
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: as EntityAi_Move, but it only turns the entity (twice as fast for command bits
   0x40 and 0x400) and picks the animation; it does not move it. */
static void DarkSoldier_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    if (entity == (Entity *)D_00414184[0] && (entity->params->flags & 0x800)) {
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
    if (entity->unkC8 & 0x440) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        Motion_Start(entity, 5, 0, entity->params->unkC4, 0);
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

/* The melee attack (busy bit 1): the same as EntityAi_MeleeAttack. DarkSoldier_PlayAnim6Or7BySide and
   DarkSoldier_MoveAlongArc start it after calling EntityAi_PickAttack. */
static void DarkSoldier_MeleeAttack(Entity *entity) {
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
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->unkFC->unk82);
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

/* An action (busy bit 0x1000): plays animation 6 or 7 to its end, turning to the player while it
   blends in. With work byte unk7B at 1 the side is the one of the player's facing the entity is on,
   and DarkSoldier_MeleeAttack follows; with unk7B at 0 the side is drawn, weighted by that angle,
   and it thinks afterwards. */
static void DarkSoldier_PlayAnim6Or7BySide(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 side;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        side = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
        if (unkE0->unk7B == 1) {
            if (side > 0.0f) {
                Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            } else {
                Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        } else if (side < 0.0f) {
            if ((f32)(rand() % 120) > side * 57.29578f + 60.0f) {
                Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            } else {
                Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        } else {
            if ((f32)(rand() % 120) > side * 57.29578f + 60.0f) {
                Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            } else {
                Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            }
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (unkE0->unk7B == 0) {
                if ((s32)(entity->flags >> 5) & 1) {
                    entity->unkE0.think(entity, 1);
                } else {
                    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
                }
            } else {
                EntityAi_PickAttack((char *)entity);
                EntityAi_StartAction(entity, DarkSoldier_MeleeAttack, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x2000): the move DarkSoldier_CanMoveAlongArc tried out. Plays animation 8
   turning to the player, then from frame 10 moves the entity along the arc (move_along_arc:
   forwards at work float unk44 along unk30, in y at unk48, both from EntityParams.unk43A to unk43E)
   through animations 17 and 9, until flag bit 44, 48, 49 or 50 is set. Afterwards, with work byte
   unk7B set, it calls EntityAi_PickAttack and starts DarkSoldier_MeleeAttack; else it thinks. */
static void DarkSoldier_MoveAlongArc(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModel *model;
    f32 unused[4]; // match: the frame has room for it, and no call here is a tail call

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit17 = FALSE;
        entity->action_step++;
    case 1:
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        model = entity->model;
        if (model->unkFC->unk88 == 0) {
            unkE0->unk48 = entity->params->unk43A;
            unkE0->unk44 = entity->params->unk43C;
            entity->action_step++;
            EntityMath_FacingVector((Vector4 *)unkE0->unk30, model->unkD0[1]);
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 10) {
            move_along_arc(entity, unkE0);
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            Motion_Start(entity, 0x11, 0, 1, 1);
            entity->action_step += 2;
        }
        break;
    case 4:
        move_along_arc(entity, unkE0);
        if (unkE0->unk48 < 10.0f) {
            if (ABS(entity->unk68 - entity->model->unkD0[1]) > 1.5707964f) {
                ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
                entity->action_step++;
            } else {
                Motion_Start(entity, 9, 0, 5, 1);
                entity->action_step += 2;
            }
        }
        break;
    case 5:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            Motion_Start(entity, 9, 0, 1, 1);
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step = 8;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
        move_along_arc(entity, unkE0);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            Motion_Start(entity, 9, 0, 1, 1);
            entity->action_step = 7;
        }
        break;
    case 6:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step = 8;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
    case 3:
        move_along_arc(entity, unkE0);
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            entity->action_step++;
        }
        break;
    case 7:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
        move_along_arc(entity, unkE0);
        break;
    case 8:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
            if (unkE0->unk7B == 0) {
                if (ENTITY_FLAG_BITS(entity)->bit5) {
                    unkE0->think(entity, 1);
                } else {
                    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
                }
            } else {
                EntityAi_PickAttack((char *)entity);
                EntityAi_StartAction(entity, DarkSoldier_MeleeAttack, 1);
            }
        }
        break;
    }
}

/* The dodge action (busy bit 8): plays animation 6 or 7, by the side of the player's facing the
   entity is on, to its end, turning to the player while it blends in. It goes from step 1 straight
   to step 5: steps 2 to 4 (a movement in y by work float unk48, with animation 9) are never
   reached. */
static void DarkSoldier_Dodge(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 landed;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        if (EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
            Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 1);
        }
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            unkE0->unk48 = entity->params->unk43A * 0.8f;
            entity->action_step = 5;
        }
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 4) {
            entity->unk88 = -unkE0->unk48;
            unkE0->unk48 -= entity->params->unk43E * 0.1f;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            Motion_Start(entity, 9, 0, entity->params->unkC4 >> 1, 1);
            entity->action_step++;
        }
        break;
    case 3:
        entity->unk88 = -unkE0->unk48;
        unkE0->unk48 -= entity->params->unk43E * 0.1f;
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            entity->action_step++;
        }
        break;
    case 4:
        entity->unk88 = -unkE0->unk48;
        unkE0->unk48 -= entity->params->unk43E * 0.1f;
        landed = (s32)(ENTITY_FLAGS(entity) >> 44) & 1;
        if (landed == 1) {
            entity->unk84 = 0.0f;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            ENTITY_FLAGS(entity) |= 0x4000;
            ENTITY_FLAGS(entity) &= ~0x10000000;
            entity->action_step++;
        }
        break;
    case 5:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
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

/* The on_hit handler: as EntityAi_OnHit, but it keeps the target and clears Entity.unk5C and flag
   bit 28. */
static void DarkSoldier_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->flags &= ~0x400;
    ENTITY_UNK5C(entity)->as_s32 = 0;
    entity->flags &= ~0x200;
    entity->flags &= ~0x10000000;
    if (((entity->flags >> 36) & 3) == 1) {
        if ((s32)((s64)(entity->flags << 13) >> 32) & 1) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (((entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* Helper of DarkSoldier_ThinkCombat: whether the way is free for DarkSoldier_MoveAlongArc. It
   follows the arc from the entity's feet (the arithmetic of move_along_arc) and asks EntityCollision_RayProps
   about the straight line to where it ends; TRUE when that returns 0. */
static s32 DarkSoldier_CanMoveAlongArc(Entity *entity) {
    f32 direction[4];
    f32 from[4];
    f32 target[4];
    f32 rise;
    f32 advance;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 distance;

    EntityMath_FacingVector((Vector4 *)direction, entity->model->unkD0[1]);
    rise = entity->params->unk43A;
    advance = entity->params->unk43C;
    delta_x = delta_y = delta_z = 0.0f;
    while (rise > 0.0f) {
        delta_x += advance * direction[0];
        delta_z += advance * direction[2];
        delta_y -= rise;
        rise -= entity->params->unk43E * 0.1f;
        advance -= 0.5f;
        if (advance < 0.0f) {
            advance = 0.0f;
        }
    }
    direction[0] = delta_x;
    direction[1] = delta_y - 100.0f;
    direction[2] = delta_z;
    Vector_Copy(from, entity->model->position);
    from[1] -= entity->unk34;
    Vector_Add(target, from, direction);
    distance = EntityMath_Distance(from, target);
    Vector_Normalize(direction, direction);
    return EntityCollision_RayProps(from, direction, NULL, distance) == 0;
}
