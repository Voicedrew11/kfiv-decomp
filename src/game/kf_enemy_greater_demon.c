/* GreaterDemon: the behaviour of entity type 56 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Greater Demon. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "common.h"
#include "kf_entity_world.h"
#include "kf_entity.h"
#include "kf_map.h"

extern u32 D_00446ED4;
extern s32 D_00446770;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];
extern KfVector D_004FF390[];
extern f32 D_00446824[];
extern f32 D_00446828[];

void EntityAi_SetUpdate(char *, s32);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
static void GreaterDemon_Think(Entity *entity, s32 unknown);
static void GreaterDemon_Move(Entity *entity);
static void GreaterDemon_OnHit(Entity *entity);
void EntityAi_SensePlayer(void *, int);
void EntityPoint_PickNear(float, void *, void *, void *);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
static void GreaterDemon_ThinkCombat(Entity *, s32);
int rand(void);
static s32 GreaterDemon_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
static int GreaterDemon_SetRanged2Weight(Entity *entity);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
int EntitySight_EntityInWay(int *);
void func_00196D48(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s16 EntityAi_MoveCommand(float, void *, int); // match: this file only matches with this type; EntityAi_MoveCommand.c defines it differently
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void EntityAi_Dodge(Entity *);
void EntityAi_MeleeAttack(Entity *);
static void GreaterDemon_Rise(Entity *);
static void GreaterDemon_SlideToStop(Entity *);
static void GreaterDemon_PlayAnim6Or7BySide(Entity *);
static void GreaterDemon_Guard(Entity *);
static void GreaterDemon_RangedAttack2(Entity *);
static void GreaterDemon_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
static void GreaterDemon_DescendAtPlayer(Entity *);
int EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
float EntityMath_WrapAngle(float);
float sinf(float);
float cosf(float);
float EntityMath_AngleXZ(float *, char *);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
static void GreaterDemon_ReactionHeavy(Entity *);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
float EntityMath_DistanceXZ(void *, void *);
void EntityEvent_Respawn(char *);

/* Set-up of a type 56 entity: clears its work area and installs the type's handlers. */
void GreaterDemon_Init(Entity *entity) {
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
    unkE0->think = GreaterDemon_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))GreaterDemon_OnHit;
    unkE0->move = (void (*)(void))GreaterDemon_Move;
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
    unkE0->unk6C = entity->params->unk3D8[2].record.unk00;
    Motion_StepModel(entity->model, 0, 0);
}

static void GreaterDemon_Think(Entity *entity, s32 unknown) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    MapSlot *slot;
    KfVector *point;
    f32 height;
    f32 no_floor;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7C = 30;
    }
    if (unkE0->unk7C > 0) {
        unkE0->unk7C--;
    }
    if (entity->distance_to_player < entity->radius + 500.0f + 100.0f && unkE0->unk7A == 1 && unkE0->unk7B == 0) {
        unkE0->unk7A = 0;
    } else if (entity->radius + 1000.0f + 100.0f < entity->distance_to_player && unkE0->unk7A == 0) {
        unkE0->unk7A = 1;
    }
    if (unkE0->unk6A < 5) {
        EntityPoint_PickNear(500.0f, &D_004FF390[unkE0->unk6A], &D_00413F80, &D_00413F80);
        slot = &D_0038CD20[D_00446770];
        height = EntityCollision_DistanceToPropFloor((MapRecord *)slot->records + D_00413F80.unk252, &D_004FF390[unkE0->unk6A]);
        no_floor = D_00446824[0];
        if (height == no_floor) {
            height = EntityCollision_DistanceToPropFloor(entity->unk14, &D_004FF390[unkE0->unk6A]);
            if (height != no_floor) {
                point = &D_004FF390[unkE0->unk6A];
                point->y += height;
                unkE0->unk6A++;
            }
        } else {
            point = &D_004FF390[unkE0->unk6A];
            point->y += height;
            unkE0->unk6A++;
        }
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if ((s32)(entity->flags >> 53) & 1) {
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
                GreaterDemon_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

static void GreaterDemon_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    f32 direction[4];
    f32 angle;
    f32 turn_left;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (GreaterDemon_ShouldGuard(entity, 200.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, GreaterDemon_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (entity->spawn_info->unk40 == 1) {
        GreaterDemon_SetRanged2Weight(entity);
    } else {
        entity->params->unk3D8[2].record.unk00 = 0;
    }
    // match: angle is read here before anything has set it
    if (unkE0->unk7B == 0 && entity->distance_to_player < entity->radius + 50.0f + 100.0f && entity->unkA0 == 0 &&
        rand() % 100 < (s16)entity->params->unk434 && ABS(angle) < 1.0471976f) {
        turn_left = entity->unk68 - entity->model->unkD0[1];
        if (ABS(turn_left) < 1.0471976f) {
            EntityAi_StartAction(entity, GreaterDemon_PlayAnim6Or7BySide, unknown);
            return;
        }
    }
    side = EntityAi_PickCombatChoice(entity, 2);
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
            if (unkE0->unk7B == 1) {
                EntityAi_StartAction(entity, GreaterDemon_SlideToStop, unknown);
            } else if (EntityAi_PickAttack((char *)entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 2) {
                    EntityAi_StartAction(entity, GreaterDemon_RangedAttack2, unknown);
                } else {
                    EntityAi_StartAction(entity, GreaterDemon_RangedAttack, unknown);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            if (unkE0->unk7A == 1 && unkE0->unk7B == 0 && EntitySight_EntityInWay((int *)entity) == 0) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 0xF0;
                unkE0->unk78 = 0;
                EntityAi_StartAction(entity, GreaterDemon_Rise, unknown);
                return;
            }
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 0x78;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (entity->unkCD == 1) {
        turn = EntityAi_TurnFromPush(entity, direction);
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
        } else if (unkE0->unk7A == 1) {
            if (EntitySight_EntityInWay((int *)entity) == 0 && unkE0->unk7B == 0) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 0xF0;
                unkE0->unk78 = 0;
                EntityAi_StartAction(entity, GreaterDemon_Rise, unknown);
                return;
            }
            if (turn > 0) {
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity->radius + 100.0f + 30.0f, entity, 1);
            }
        } else {
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

static void GreaterDemon_Move(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    EntityParams *params;
    f32 turn;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    params = entity->params;
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= params->unk58;
        if (unkE0->unk1C < -params->turn_limit) {
            unkE0->unk1C = -params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += params->unk58;
        if (params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = params->turn_limit;
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
            Motion_Start(entity, 4, 0, params->unkC4, 0);
        } else {
            Motion_Start(entity, 3, 0, params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        Motion_Start(entity, 5, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        Motion_Start(entity, 8, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        Motion_Start(entity, 9, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        Motion_Start(entity, 6, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 8) {
        Motion_Start(entity, 7, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        Motion_Start(entity, 3, 0, params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, params->unkC4, 0);
    }
}

/* An action (busy bits 0x3000) that GreaterDemon_ThinkCombat starts: plays animation 8 turning to
   Entity.unk68 while it blends in; from frame 41 it moves the entity up, by work float unk48 a
   frame, which grows by 0.8 to 12 (unk88 is given minus unk48; y grows downward, EntityCollision_DistanceToPropFloor).
   When the motion has ended it sets work byte unk7B and starts GreaterDemon_DescendAtPlayer. */
static void GreaterDemon_Rise(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit17 = 0;
        unkE0->unk48 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        return;
    case 2:
        if ((int)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7B = 1;
            EntityAi_StartAction(entity, GreaterDemon_DescendAtPlayer, 1);
        }
        if (entity->model->unkFC->unk82 >= 0x29) {
            float next_unk48;
            entity->unk88 = -unkE0->unk48;
            next_unk48 = unkE0->unk48 + 0.8f;
            unkE0->unk48 = next_unk48;
            if (next_unk48 > 12.0f) unkE0->unk48 = 12.0f;
        }
        break;
    }
}

/* An action (busy bit 0x4000) that GreaterDemon_Rise starts. In animation 17 it goes on moving up
   while EntityCollision_DistanceToPropFloor, the distance to the floor below, reads under 220, and turns to Entity.unk68;
   once the motion has ended and the heading is within 20 degrees of unk68 it plays animations 18
   and 19 and, from frame 16 of 18, moves the entity along its heading at work float unk44 (12,
   growing by 9 percent a frame up to 80) and down at unk48 (20, falling by 0.8 to 3). When flag bit
   44, 45, 46, 48 or 49 is set it stops, checks part 0 of the third melee record
   (EntityParams.unk350) for a hit if it was bit 49, and starts GreaterDemon_SlideToStop. */
static void GreaterDemon_DescendAtPlayer(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 flag;

    entity->unkC6 |= 0x4000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, 10, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk6E = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        entity->unk88 = -unkE0->unk48;
        unkE0->unk48 += 0.8f;
        if (unkE0->unk48 > 12.0f) {
            unkE0->unk48 = 12.0f;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) < 0.34906584f) {
                Motion_Start(entity, 0x12, 0, 1, 1);
                entity->action_step++;
                unkE0->unk48 = 20.0f;
                unkE0->unk44 = 12.0f;
            }
        }
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        if (EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position) < 220.0f) {
            entity->unk88 = -unkE0->unk48;
            unkE0->unk48 += 0.8f;
            if (unkE0->unk48 > 12.0f) {
                unkE0->unk48 = 12.0f;
            }
        }
        break;
    case 3:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            Motion_Start(entity, 0x13, 0, 10, 1);
            entity->action_step++;
        }
        if (entity->model->unkFC->unk82 >= 0x10) {
            entity->unk84 = -sinf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
            entity->unk88 = unkE0->unk48;
            entity->unk8C = -cosf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
            unkE0->unk44 *= 1.09f;
            unkE0->unk48 -= 0.8f;
            if (unkE0->unk44 > 80.0f) {
                unkE0->unk44 = 80.0f;
            }
            if (unkE0->unk48 < 3.0f) {
                unkE0->unk48 = 3.0f;
            }
        }
        break;
    case 5:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->bit45 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1) {
            char *unk350;

            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            unk350 = entity->params->unk350;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            flag = ENTITY_FLAG_BITS(entity)->bit49;
            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
            EntityAi_StartAction(entity, GreaterDemon_SlideToStop, 1);
            return;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->action_step++;
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        }
        entity->unk84 = -sinf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk88 = unkE0->unk48;
        entity->unk8C = -cosf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 *= 1.09f;
        unkE0->unk48 -= 0.8f;
        if (unkE0->unk44 > 80.0f) {
            unkE0->unk44 = 80.0f;
        }
        if (unkE0->unk48 < 3.0f) {
            unkE0->unk48 = 3.0f;
        }
        break;
    case 6:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->bit45 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1) {
            char *unk350;

            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            unk350 = entity->params->unk350;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            flag = ENTITY_FLAG_BITS(entity)->bit49;
            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
            EntityAi_StartAction(entity, GreaterDemon_SlideToStop, 1);
            return;
        }
        entity->unk84 = -sinf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk88 = unkE0->unk48;
        entity->unk8C = -cosf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 *= 1.09f;
        unkE0->unk48 -= 0.8f;
        if (unkE0->unk44 > 80.0f) {
            unkE0->unk44 = 80.0f;
        }
        if (unkE0->unk48 < 3.0f) {
            unkE0->unk48 = 3.0f;
        }
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        return;
    }
    return; // match: with this the call above becomes a tail call
}

/* An action (busy bits 0x5000) that GreaterDemon_DescendAtPlayer starts (and
   GreaterDemon_ThinkCombat, while work byte unk7B is 1): plays animation 29 to its end while the
   entity goes on along its heading at work float unk44, which falls by 2 a frame to 0. Once, when
   flag bit 49 is set, it checks part 0 of the third melee record (EntityParams.unk350) for a hit,
   unless GreaterDemon_DescendAtPlayer did. At the end it clears unk7B and thinks. */
static void GreaterDemon_SlideToStop(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    char *unk350 = entity->params->unk350;

    entity->unkC6 |= 0x5000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x1D, 0, 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->flags |= 0x20000;
            entity->action_step++;
        }
        if (unkE0->unk6E == 0) {
            s32 flag = (s32)(entity->flags >> 49) & 1;

            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
        }
        break;
    case 2:
        if (unkE0->unk6E == 0) {
            s32 flag = (s32)(entity->flags >> 49) & 1;

            if (flag == 1) {
                EntityAi_CheckMeleeHit(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7B = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        entity->unk84 = -sinf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk8C = -cosf(EntityMath_WrapAngle(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 -= 2.0f;
        if (unkE0->unk44 < 0.0f) {
            unkE0->unk44 = 0.0f;
        }
        break;
    }
}

/* An action (busy bit 0x1000) that GreaterDemon_ThinkCombat starts: plays animation 6 or 7, by the
   side of the player's facing the entity is on, to its end, turning to Entity.unk68 throughout;
   then thinks. Animations 6 and 7 are the ones EntityAi_Move plays for the sideways commands. */
static void GreaterDemon_PlayAnim6Or7BySide(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        if (0.0f < EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position)) * 57.29578f) {
            Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 1);
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
        EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
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

/* When the state field in flags is 2, resets the entity and registers GreaterDemon_ReactionHeavy; then sets state 2. */
static void GreaterDemon_OnHit(Entity *entity) {
    if ((((u64)entity->flags >> 36) & 3) == 2) {
        EntityAi_LowerWeightsAfterHit((char *)entity);
        entity->target = NULL;
        entity->flags &= ~0x400;
        entity->flags &= ~0x200;
        EntityAi_SetReaction((char *)entity, (void (*)(void))GreaterDemon_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

int EntityAi_SetReaction(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call

static void GreaterDemon_ReactionHeavy(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        Motion_Start(entity, 0xF, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        unkE0->unk7B = 0;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
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
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

static void GreaterDemon_Guard(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xD, 0, 0xA, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = entity->params->unkCE;
            entity->flags |= 0x8000L << 20;
            entity->flags &= ~0x4000;
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit43 && ENTITY_FLAG_BITS(entity)->bit39 && !ENTITY_FLAG_BITS(entity)->bit38) {
            entity->flags &= ~(0x8000L << 28);
            entity->unkA0 = entity->params->unkCE;
        }
        EntityEvent_StartEffectAtPoint((char **)entity, 0x32F, 0, (int)entity->model->unkD0);
        if (entity->unkA0 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
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

/* Helper of GreaterDemon_ThinkCombat: whether to start GreaterDemon_Guard. Only when the entity is
   not busy, the player is at least min_gap away (less the radius and 100), Entity.unk68 is within
   max_angle of the heading, the entity is within 45 degrees of the player's facing and work byte
   unk7C is above 0; then EntityParams.unkCB times in 100. */
static s32 GreaterDemon_ShouldGuard(Entity *entity, f32 min_gap, f32 max_angle) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    f32 gap = entity->distance_to_player - (entity->radius + 100.0f);
    f32 angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    f32 facing = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));

    if (entity->unkC6 != 0 || gap < min_gap || max_angle < ABS(angle) || 0.7853982f < ABS(facing) || unkE0->unk7C <= 0) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCB;
}

static void GreaterDemon_RangedAttack2(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    EntityParams *params = entity->params;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(params, kind);
    MapSlot *slot;
    KfVector *floor_point;
    KfVector *point;
    Entity *minion;
    EntityModel *minion_model;
    f32 height;
    f32 no_floor;
    f32 spin;
    s32 minion_index;
    s32 point_index;
    s32 count;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk6A = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unkE0->unk6A < 5) {
            EntityPoint_PickNear(500.0f, &D_004FF390[unkE0->unk6A], &D_00413F80, &D_00413F80);
            slot = &D_0038CD20[D_00446770];
            height = EntityCollision_DistanceToPropFloor((MapRecord *)slot->records + D_00413F80.unk252, &D_004FF390[unkE0->unk6A]);
            no_floor = D_00446828[0];
            if (height == no_floor) {
                height = EntityCollision_DistanceToPropFloor(entity->unk14, &D_004FF390[unkE0->unk6A]);
                if (height == no_floor) {
                    break;
                }
            }
            floor_point = &D_004FF390[unkE0->unk6A];
            floor_point->y += height;
            unkE0->unk6A++;
        }
        break;
    case 2:
        unkE0->unk1C = 0.0f;
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->unkFC->unk84 - params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
        if (entity->model->unkFC->unk82 == 0x11) {
            minion_index = 0;
            count = rand() % 5 + 1;
            if (unkE0->unk6A < count) {
                count = unkE0->unk6A;
            }
            for (point_index = 0; point_index < count; point_index++) {
                for (; minion_index < 8; minion_index++) {
                    minion = Entity_GetBySpawnId(minion_index + 10);
                    if (minion != NULL && (minion->flags & 3) == 1 &&
                        (((s32)(minion->flags >> 2) & 3) == 0 || ((s32)(minion->flags >> 2) & 3) == 3)) {
                        point = &D_004FF390[point_index];
                        if (EntityMath_DistanceXZ(point, &D_00413F80) > 180.0f) {
                            EntityEvent_Respawn((char *)minion);
                            minion->model->position[0] = point->x;
                            minion->model->position[1] = point->y;
                            minion->model->position[2] = point->z;
                            minion->model->unkD0[1] = EntityMath_WrapAngle((f32)(rand() % 360) * 0.017453292f);
                            minion_model = minion->model;
                            spin = minion_model->unkE0[0];
                            if (rand() % 3 != 0) {
                                spin = -spin;
                            }
                            minion_model->unkE0[0] = spin;
                            break;
                        }
                    }
                }
            }
        }
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

/* Helper of GreaterDemon_ThinkCombat (spawn mode 1 only): counts those of the entities with spawn
   ids 10 to 17 that have flag bit 0 set, bit 1 clear and flag bits 30-31 at 0 or 3, and gives
   ranged record 2 the weight kept in work area unk68[2] times that count over 8. Returns 0. */
static int GreaterDemon_SetRanged2Weight(Entity *entity) {
    EntityUnkE0Kind17FC60 *work;
    Entity *ally;
    s32 ally_index;
    s32 ally_count;
    u64 ally_flags;
    s32 flag_bits;

    work = (EntityUnkE0Kind17FC60 *)&entity->unkE0;
    ally_count = 0;
    for (ally_index = 0; ally_index < 8; ally_index++) {
        ally = Entity_GetBySpawnId(ally_index + 0xA);
        if (ally != NULL) {
            ally_flags = ally->flags;
            if ((ally_flags & 3) == 1) {
                flag_bits = (s32)((ally_flags << 0x1E) >> 0x20); // match: bits 30-31 of the flag word
                if (((flag_bits & 3) == 0) || ((flag_bits & 3) == 3)) {
                    ally_count++;
                }
            }
        }
    }
    entity->params->unk3D8[2].record.unk00 = (f32)work->unk68[2] * (f32)ally_count * 0.125f;
    return 0;
}

static void GreaterDemon_RangedAttack(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[3];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
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
        direction[0] = 0.0f;
        direction[1] = EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f);
        direction[2] = 0.0f;
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction);
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
