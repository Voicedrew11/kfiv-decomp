/* DarkBird: the behaviour of entity type 59 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Bird. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_shot.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
extern s32 D_003E6E40[][2];
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
void Motion_SetPartSlots(EntityModel *, s32, s32, s32);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
int EntityAi_StartSlotMotion(void *, void *, int, int, int, int, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern PlayerData D_00413F80;
int rand(void);
s32 EntityAi_PickCombatChoice(Entity *, s32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
int EntityAi_TurnFromPush(void *, void *);
void EntityPoint_PickNear(float, void *, void *, void *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
float sinf(float);
float cosf(float);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);
float EntityMath_WrapAngle(float);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *entity, s32 base);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float EntityMath_DistanceXZ(void *, void *);
float sqrtf(float);
float atan2f(float, float);
float EntityMath_AngleXZ(float *, char *);
f32 EntityMath_Approach(f32, f32, f32);
int func_001B4F30(u16, void *, void *, void *, void *);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
void EntityEvent_OnDeath(Entity *);
int EntityAi_StepSlotMotion(Entity *, EntityMotionSlot *, int, f32);
s32 Motion_StepSlot(Entity *, EntityUnkE0SlotBits *, s32, s32);
void EntityEvent_StartFrameEffects(Entity *, s32, s32, s32);
int EntityAi_LookAtPoint(void *, int, int, int, int, float);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);

static void DarkBird_Think(Entity *entity, s32 unknown);
static void DarkBird_ThinkCombat(Entity *entity, s32 unknown);
static void DarkBird_ThinkIdle(Entity *entity, s32 unknown);
static void DarkBird_Move(Entity *entity);
static void DarkBird_ApplyMoveCommand(Entity *entity);
static int func_00172BB8(Entity *entity);
static void DarkBird_DormantMoveToCircle(Entity *entity);
static void DarkBird_ApproachForMelee(Entity *entity);
static void DarkBird_MeleeAttack(Entity *entity);
static void DarkBird_RangedAttack(Entity *entity);
static s32 DarkBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static s32 DarkBird_LaunchThreeShots(Entity *entity, EntityParamsUnk3D8Full *record, s32 frame_from, s32 frame_to, f32 *angles);
static void DarkBird_OnHit(Entity *entity);
static void DarkBird_ReactionLight(Entity *entity);
static void DarkBird_ReactionHeavy(Entity *entity);
static void DarkBird_Death(Entity *entity);
static void DarkBird_Animate(Entity *entity);

/* Set-up of a type 59 entity: clears its work area and installs the type's handlers. */
void DarkBird_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)&entity->unkE0;
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
    unkE0->think = DarkBird_Think;
    unkE0->animate = (void (*)(void))DarkBird_Animate;
    unkE0->on_hit = (void (*)(void))DarkBird_OnHit;
    unkE0->move = (void (*)(void))DarkBird_Move;
    unkE0->death = (void (*)(void))DarkBird_Death;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->dormant = (void (*)(void))DarkBird_DormantMoveToCircle;
    } else {
        unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    }
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->flags |= 0x10000000;
    entity->target = NULL;
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
    Motion_SetPartSlots(entity->model, 6, 0xB, 1);
    slots->unk80.bit5 = TRUE;
    slots->unk80.bit6 = TRUE;
    slots->unk88.bit6 = TRUE;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with DarkBird_ThinkIdle for state 0 and
   DarkBird_ThinkCombat for state 2; the first time in state 2 (work byte unk7A) it starts
   DarkBird_ApproachForMelee instead. It keeps flag bit 28 set except in the death action, keeps
   animation 3 on the second motion slot outside attacks, heavy-hit reactions and the death, and
   calls DarkBird_ApplyMoveCommand while the entity is not busy or is in a hit reaction. */
static void DarkBird_Think(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 & 0x40) {
        entity->flags &= ~0x10000000;
    } else {
        entity->flags |= 0x10000000;
    }
    if (!(entity->unkC6 & 0x45)) {
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                unkE0->unk7A = 0;
                DarkBird_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                if (unkE0->unk7A == 0) {
                    EntityAi_StartAction(entity, DarkBird_ApproachForMelee, unknown);
                    unkE0->unk7A = 1;
                } else {
                    DarkBird_ThinkCombat(entity, unknown);
                }
                break;
            default:
                DarkBird_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (entity->unkC6 == 0 || (entity->unkC6 & 6)) {
        DarkBird_ApplyMoveCommand(entity);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat without the guard and the dodge, and with
   the command of func_00172BB8. It also picks the pitch command, work byte unk79: 1 or -1 at random
   (EntityParams.unk438 times in 100), 2 or -2 to keep the height in bounds (in mode 0 EntityCollision_DistanceToPropFloor
   between EntityParams.unk436 and unk434, in modes 1 and 2 the y against the spawn point's). A
   ranged attack is DarkBird_RangedAttack, but only when the player's y less its own is 200 or
   more; a melee one is DarkBird_MeleeAttack when that difference is between 100 and 400, else
   DarkBird_ApproachForMelee. */
static void DarkBird_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    f32 height;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else if (entity->unkA0 == 0) {
            unkE0->unk79 = 0;
        }
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    if (entity->spawn_info->unk40 == 0) {
        if (EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position) < (f32)entity->params->unk436 ||
            ENTITY_FLAG_BITS(entity)->bit44 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        } else if (EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position) > (f32)(s16)entity->params->unk434 ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
    } else if (entity->spawn_info->unk40 == 1 || entity->spawn_info->unk40 == 2) {
        if (entity->spawn_info->spawn_y - entity->model->position[1] <
                (f32)entity->spawn_info->unk42 * (entity->spawn_info->unk43 != 0 ? 100.0f : -100.0f) ||
            ENTITY_FLAG_BITS(entity)->bit44 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
    }
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
                if (D_00413F80.position[1] - entity->model->position[1] < 200.0f) {
                    entity->target = &D_00413F80;
                    entity->unkA0 = rand() % 30 + 0x1E;
                    unkE0->unk78 = 0;
                    entity->unkCD = 1;
                    unkE0->unk79 = 2;
                } else {
                    EntityAi_StartAction(entity, DarkBird_RangedAttack, unknown);
                    return;
                }
            } else {
                height = D_00413F80.position[1] - entity->model->position[1];
                if (100.0f < height && height < 400.0f) {
                    EntityAi_StartAction(entity, DarkBird_MeleeAttack, unknown);
                } else {
                    EntityAi_StartAction(entity, DarkBird_ApproachForMelee, unknown);
                }
                return;
            }
            break;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x3C;
            unkE0->unk78 = 0;
            break;
        default:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x3C;
            unkE0->unk78 = 0;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 0: as EntityAi_ThinkIdle, always with a point to go to, the head
   following, the command of func_00172BB8, and the pitch command (work byte unk79) picked as in
   DarkBird_ThinkCombat. */
static void DarkBird_ThinkIdle(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0) {
        EntityPoint_PickNear((f32)entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
        entity->target = unkE0->unk30;
        entity->unkA0 = rand() % 60 + 0x3C;
        if (entity->spawn_info->unk40 == 0) {
            if (EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position) < (f32)entity->params->unk436) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 2;
            } else if (EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position) > (f32)(s16)entity->params->unk434) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -2;
            }
        } else if (entity->spawn_info->unk40 == 1 || entity->spawn_info->unk40 == 2) {
            if (entity->spawn_info->spawn_y - entity->model->position[1] <
                    (f32)entity->spawn_info->unk42 * (entity->spawn_info->unk43 != 0 ? 100.0f : -100.0f) ||
                ENTITY_FLAG_BITS(entity)->bit44 == 1) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 2;
            } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                       ENTITY_FLAG_BITS(entity)->bit46 == 1) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -2;
            }
        }
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else {
            unkE0->unk79 = 0;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        // match: retail passes two more arguments here and takes the result as 16 bits
        entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: keeps animation 3 on both motion slots, flag bits 11 to 13 at 0. DarkBird_ApplyMoveCommand
   does the turning and the moving. */
static void DarkBird_Move(Entity *entity) {
    entity->flags &= ~0x3800UL;
    EntityAi_StartSlotMotion(entity, entity->unkE0.unk80, 0, 3, 0, entity->params->unkC4, 0);
    EntityAi_StartSlotMotion(entity, entity->unkE0.unk88, 1, 3, 0, entity->params->unkC4, 0);
}

/* Carries out the movement command in Entity.unkC8; DarkBird_Think and
   DarkBird_ApproachForMelee call it (the move handler only keeps the animation). It turns the
   entity, eases its pitch towards 10 degrees for each unit of work byte unk79 (back to level at 0),
   and speeds up or slows down along its axes by EntityParams.unk50 a frame up to unk54. */
static void DarkBird_ApplyMoveCommand(Entity *entity) {
    EntityUnkE0Kind1719F8 *unkE0 = (EntityUnkE0Kind1719F8 *)&entity->unkE0;

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
    } else if (entity->params->unk58 < unkE0->unk1C) {
        unkE0->unk1C -= entity->params->unk58;
    } else if (unkE0->unk1C < -entity->params->unk58) {
        unkE0->unk1C += entity->params->unk58;
    } else {
        unkE0->unk1C = 0.0f;
    }
    entity->unk94 += unkE0->unk1C;
    if (unkE0->unk79 > 0) {
        if (ABS((f32)unkE0->unk79) * -0.17453292f < entity->model->unkD0[0]) {
            unkE0->unk4C -= 0.006981317f;
            if (unkE0->unk4C < -0.017453292f) {
                unkE0->unk4C = -0.017453292f;
            }
        } else {
            unkE0->unk4C = 0.0f;
        }
    } else if (unkE0->unk79 < 0) {
        if (entity->model->unkD0[0] < ABS((f32)unkE0->unk79) * 0.17453292f) {
            unkE0->unk4C += 0.006981317f;
            if (0.017453292f < unkE0->unk4C) {
                unkE0->unk4C = 0.017453292f;
            }
        } else {
            unkE0->unk4C = 0.0f;
        }
    } else if (unkE0->unk79 == 0) {
        if (0.017453292f < entity->model->unkD0[0]) {
            unkE0->unk4C = -0.017453292f;
        } else if (entity->model->unkD0[0] < -0.017453292f) {
            unkE0->unk4C = 0.017453292f;
        } else {
            unkE0->unk4C = -entity->model->unkD0[0];
        }
    }
    entity->unk90 += unkE0->unk4C;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    if (entity->unkC8 & 1) {
        unkE0->unk48 -= entity->params->unk50;
        if (unkE0->unk48 < -entity->params->unk54) {
            unkE0->unk48 = -entity->params->unk54;
        }
    } else if (entity->unkC8 & 2) {
        unkE0->unk48 += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk48) {
            unkE0->unk48 = entity->params->unk54;
        }
    } else if (unkE0->unk48 > entity->params->unk50) {
        unkE0->unk48 -= entity->params->unk50;
    } else if (unkE0->unk48 < -entity->params->unk50) {
        unkE0->unk48 += entity->params->unk50;
    } else {
        unkE0->unk48 = 0.0f;
    }
    if (entity->unkC8 & 4) {
        unkE0->unk44 += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk44) {
            unkE0->unk44 = entity->params->unk54;
        }
    } else if (entity->unkC8 & 8) {
        unkE0->unk44 -= entity->params->unk50;
        if (unkE0->unk44 < -entity->params->unk54) {
            unkE0->unk44 = -entity->params->unk54;
        }
    } else if (entity->params->unk50 < unkE0->unk44) {
        unkE0->unk44 -= entity->params->unk50;
    } else if (unkE0->unk44 < -entity->params->unk50) {
        unkE0->unk44 += entity->params->unk50;
    } else {
        unkE0->unk44 = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = unkE0->unk44;
    ENTITY_SCRATCH_VECTOR[1] = 0.0f;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk48;
    sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
    sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[0]);
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[1]);
    Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    if (entity->unkC8 & 0x40) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
    entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
    entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
}

/* Returns 0 with no target, 1 if the turn to it is within the limit, else 0x101/0x201 by turn direction. */
static int func_00172BB8(Entity *entity) {
    float angle;
    float limit;
    int result;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = EntityMath_HeadingTo(&ENTITY_MODEL(entity)->position.x, entity->target);
    angle = EntityMath_AngleDifference(ENTITY_MODEL(entity)->rotation.y, angle);
    limit = ENTITY_CONTROLLER(entity)->turn_limit;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        result = 0x101;
        if (0.0f <= angle) {
            result = 0x201;
        }
    }
    return result;
}

/* The dormant action of a spawn in mode 1 (EntitySpawnInfo.unk40 is 1; busy bit 0x20): holds
   animation 1 as EntityAi_Dormant does; when the player is within EntitySpawnInfo.unk54 or flag bit
   43 is set it goes to state 2 at once, puts the entity at a random point of the circle of radius
   2000 round the origin (x and z) and thinks. */
static void DarkBird_DormantMoveToCircle(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 angle;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < (f32)entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            angle = EntityMath_WrapAngle((f32)(rand() % 361) * 0.017453292f);
            entity->model->position[0] = sinf(angle) * 2000.0f;
            entity->model->position[2] = cosf(angle) * 2000.0f;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bits 0x3000): animation 3 on both motion slots; for up to 212 frames it goes for
   the player (func_00172BB8, DarkBird_ApplyMoveCommand) with the pitch command that brings the player's y less
   its own between 200 and 400, and starts DarkBird_MeleeAttack once the player is within the
   radius plus 350 and that difference is between 100 and 400. */
static void DarkBird_ApproachForMelee(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    f32 height;
    s32 turn;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, 3, 0, entity->params->unkC4, 0);
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 1;
        entity->target = &D_00413F80;
        unkE0->unk68 = 0;
        entity->action_step++;
    case 1:
        if (entity->distance_to_player < entity->radius + 250.0f + 100.0f) {
            f32 above = D_00413F80.position[1] - entity->model->position[1];

            if (100.0f < above && above < 400.0f) {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                EntityAi_StartAction(entity, DarkBird_MeleeAttack, 1);
                return;
            }
        }
        if (unkE0->unk68++ >= 0xD3) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
            return;
        }
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
        }
        height = D_00413F80.position[1] - entity->model->position[1];
        if (height < 200.0f || ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit45 == 1) {
            unkE0->unk79 = 2;
        } else if (400.0f < height || ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            unkE0->unk79 = -2;
        } else {
            unkE0->unk79 = 0;
        }
        DarkBird_ApplyMoveCommand(entity);
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, on the motion slots. */
static void DarkBird_MeleeAttack(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[kind];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, kind | 0x14, 0, entity->params->unkC4 >> 1, 1);
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, ((s32)(entity->flags >> 58) & 3) | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
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
        elapsed = (unkE0->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, on the motion slots, with launches of
   its own. Record 1: on the frame of the record's first shot and on frames EntityParams.unk43A and
   unk43C, DarkBird_LaunchThreeShots launches, pitched at the player and up to 14 degrees to
   either side of the reverse of the heading. The other records: on the frame of the first shot
   DarkBird_LaunchBothShots launches at a point 200 below the player. The pause Entity.unkA2
   is the record's unk02 as it is. */
static void DarkBird_RangedAttack(Entity *entity) {
    EntityUnkE0Kind1719F8 *unkE0 = (EntityUnkE0Kind1719F8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 target[4];
    f32 angles[4];
    f32 *point;
    s32 frame;
    s32 elapsed;
    s32 launch_frame;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, ((s32)(entity->flags >> 58) & 3) + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (record->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= record->unk01) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = unkE0->unk80.unk02;
        elapsed = (unkE0->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        if (((s32)(entity->flags >> 58) & 3) == 1) {
            for (i = 0; i < 3; i++) {
                switch (i) {
                case 0:
                    launch_frame = record->unk10[0].unk02;
                    break;
                case 1:
                    launch_frame = entity->params->unk43A;
                    break;
                case 2:
                    launch_frame = entity->params->unk43C;
                    break;
                }
                if (launch_frame != 0 && elapsed < launch_frame && frame >= launch_frame) {
                    point = (f32 *)func_0010CAB8((char *)entity->model, 0);
                    angles[0] = EntityMath_WrapAngle(-atan2(D_00413F80.position[1] - point[1], EntityMath_DistanceXZ(point, &D_00413F80)));
                    if (!(rand() & 1)) {
                        angles[1] = EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f + (f32)(rand() % 15) * 0.017453292f);
                    } else {
                        angles[1] = EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f - (f32)(rand() % 15) * 0.017453292f);
                    }
                    angles[2] = 0.0f;
                    DarkBird_LaunchThreeShots(entity, record, elapsed, unkE0->unk80.unk02, angles);
                }
            }
        } else {
            Vector_Copy(target, &D_00413F80);
            target[1] -= 200.0f;
            if (record->unk10[0].unk02 != 0 && elapsed < record->unk10[0].unk02 && frame >= record->unk10[0].unk02) {
                DarkBird_LaunchBothShots(entity, (EntityParamsUnk3D8 *)record, elapsed, unkE0->unk80.unk02, target, 0.7853982f, 0.0f);
            }
        }
        if (unkE0->unk80.unk02 > 0 && record->unk03 >= unkE0->unk80.unk02) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->unkA2 = record->unk02;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* Helper of DarkBird_RangedAttack: launches both shots of a record at once, whatever the
   animation frame, each from its point and turned towards `target` by at most the two rates. */
static s32 DarkBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
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

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
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
    return 0;
}

/* Helper of DarkBird_RangedAttack: launches the three shots of a record at once, whatever the
   animation frame, each from its point, all at the given angles. */
static s32 DarkBird_LaunchThreeShots(Entity *entity, EntityParamsUnk3D8Full *record, s32 frame_from, s32 frame_to, f32 *angles) {
    EntityUnk1B1390 *shot;
    char *point;
    s32 i;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        shot = Shot_Alloc();
        shot->unk00 = 1;
        shot->unk20 = 0;
        shot->unk22 = 0;
        shot->unk28 = entity->params->unk238;
        point = func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
        if (point != NULL) {
            func_001B4F30(record->unk10[i].unk00, shot, point, angles, entity);
        }
    }
    return 0;
}

/* The on_hit handler: as EntityAi_OnHit with the type's own two reactions; a light hit starts
   DarkBird_ReactionLight whether flag bit 19 is set or not. */
static void DarkBird_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))DarkBird_ReactionLight, 0);
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))DarkBird_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the motion slots
   (animation 14 on the first, 3 on the second). */
static void DarkBird_ReactionLight(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, 0xE, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, 3, 0, 5, 0);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
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
            unkE0->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the motion slots. */
static void DarkBird_ReactionHeavy(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, 0xF, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, 0xF, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) {
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
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
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots. */
static void DarkBird_Death(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &unkE0->unk80, 0, 0x10, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &unkE0->unk88, 1, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}

/* The animate handler: plays the two motion slots of the work area (unk80 and unk88) where
   EntityAi_Animate plays the model's one motion, then turns the head (EntityAi_LookAtPoint with the target
   while flag bit 9 is set and there is one). */
static void DarkBird_Animate(Entity *entity) {
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
