/* LavaBird: the behaviour of entity type 68 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Lava Bird. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_shot.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Motion_SetPartSlots(EntityModel *, s32, s32, s32);
void Vector_Copy(void *, void *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
int rand(void);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_StartSlotMotion(void *, void *, int, int, int, int, int);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_TurnFromPush(void *, void *);
extern PlayerData D_00413F80;
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityPoint_PickNear(float, void *, void *, void *);
extern f32 D_00431AF0[16];
float EntityMath_EaseAngleToward(float *, float, float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
void EntityAi_SetState(char *, u8);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityAi_SetAttackDelay(Entity *entity, s32 base);
float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float EntityMath_WrapAngle(float);
float EntityMath_DistanceXZ(void *, void *);
float EntityMath_AngleXZ(float *, char *);
f32 EntityMath_Approach(f32, f32, f32);
int func_001B4F30(u16, void *, void *, void *, void *);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *entity, void (*callback)(void), int run);
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
void EntityEvent_OnDeath(Entity *);
int EntityAi_StepSlotMotion(Entity *, EntityMotionSlot *, int, f32);
s32 Motion_StepSlot(Entity *, EntityUnkE0SlotBits *, s32, s32);
void EntityEvent_StartFrameEffects(Entity *, s32, s32, s32);
int EntityAi_LookAtPoint(void *, int, int, int, int, float);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);

static void LavaBird_Think(Entity *entity, s32 unknown);
static void LavaBird_ThinkCombat(Entity *entity, s32 unknown);
static void LavaBird_ThinkPursue(Entity *entity, s32 unknown);
static void LavaBird_ThinkIdle(Entity *entity, s32 unknown);
static void LavaBird_Move(Entity *entity);
static void LavaBird_ApplyMoveCommand(Entity *entity);
static s32 func_0017EA98(Entity *entity, s32 mark_far, f32 range);
static void LavaBird_Dormant(Entity *entity);
static void LavaBird_RangedAttack(Entity *entity);
static s32 LavaBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static void LavaBird_OnHit(Entity *entity);
static void LavaBird_ReactionLight(Entity *entity);
static void LavaBird_ReactionHeavy(Entity *entity);
static void LavaBird_Death(Entity *entity);
static void LavaBird_Animate(Entity *entity);

/* Set-up of a type 68 entity: clears its work area and installs the type's handlers. */
void LavaBird_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *work = (EntityUnkE0Kind151D30 *)&entity->unkE0;
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
    unkE0->think = LavaBird_Think;
    unkE0->animate = (void (*)(void))LavaBird_Animate;
    unkE0->on_hit = (void (*)(void))LavaBird_OnHit;
    unkE0->move = (void (*)(void))LavaBird_Move;
    unkE0->death = (void (*)(void))LavaBird_Death;
    unkE0->dormant = (void (*)(void))LavaBird_Dormant;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
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
    Motion_SetPartSlots(entity->model, 0x10, 0x19, 1);
    work->unk80.bit5 = TRUE;
    work->unk80.bit6 = TRUE;
    work->unk88.bit6 = TRUE;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with the type's own functions for the three states. It
   keeps flag bit 28 set except in the death action, and animation 3 on the second motion slot
   outside heavy-hit reactions and the death; every 60th frame it draws work float unk54 (0 to 9).
   After the state's function, every frame outside the death, it gives the command of func_0017EA98
   (or the turn away from a push) and calls LavaBird_ApplyMoveCommand. */
static void LavaBird_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (D_00446ED4 % 60U == entity->unkD7 % 10U) {
        unkE0->unk54 = rand() % 10;
    }
    if (entity->unkC6 & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (!(entity->unkC6 & 0x44)) {
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)unkE0)->unk88, 1, 3, 5, entity->params->unkC4, 0);
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                LavaBird_ThinkIdle(entity, unknown);
                break;
            case 1:
                LavaBird_ThinkPursue(entity, unknown);
                break;
            case 2:
                LavaBird_ThinkCombat(entity, unknown);
                break;
            default:
                LavaBird_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (entity->unkC6 & 0x40) {
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
    } else {
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->unkC8 = 0x201;
        } else if (side < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: called like EntityAi_MoveCommand, with a speed and a mode the callee ignores
            entity->unkC8 = func_0017EA98(entity, 0, 0.0f);
        }
        LavaBird_ApplyMoveCommand(entity);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat without the guard and the dodge, with the
   command of func_0017EA98. It also picks the pitch command, work byte unk79: 1 or -1 at random
   (EntityParams.unk438 times in 100), 2 while the player's y less its own is below
   EntityParams.unk436, -2 while it is above unk434. Its only attack is LavaBird_RangedAttack,
   when EntityAi_PickAttack draws a ranged one. */
static void LavaBird_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 next_action = 0; // match: the unused initial value changes the instruction count that sizes a compiler hash table, and with it two registers
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else if (entity->unkA0 == 0) {
            unkE0->unk79 = 0;
        }
    }
    next_action = EntityAi_PickCombatChoice(entity, 0);
    if (D_00413F80.position[1] - entity->model->position[1] < entity->params->unk436) {
        entity->target = &D_00413F80;
        entity->unkA0 = rand() % 30 + 30;
        unkE0->unk78 = 0;
        next_action = 1;
        entity->unkCD = next_action;
        unkE0->unk79 = 2;
    }
    if (D_00413F80.position[1] - entity->model->position[1] > (s16)entity->params->unk434) { // match: read signed here
        entity->target = &D_00413F80;
        entity->unkA0 = rand() % 30 + 30;
        unkE0->unk78 = 0;
        next_action = 1;
        entity->unkCD = next_action;
        unkE0->unk79 = -2;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || next_action == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = next_action;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with one argument here
        ((void (*)(Entity *))EntityAi_LowerChoiceWeight)(entity);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, LavaBird_RangedAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 0;
            break;
        default:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 0;
            break;
        }
    }
    if (entity->unkCD == 1) {
        side = EntityAi_TurnFromPush(entity, direction);
        if (unkE0->unk78 == 0) {
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                // match: called like EntityAi_MoveCommand, with a speed and a mode the callee ignores
                entity->unkC8 = func_0017EA98(entity, 0, 0.0f);
            }
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 1: as EntityAi_ThinkPursue, with flag bits 11 to 13 at 0 and the
   command of func_0017EA98. */
static void LavaBird_ThinkPursue(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[2];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (entity->unkA0 == 0) {
        entity->target = &D_00413F80;
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = func_0017EA98(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 0: as EntityAi_ThinkIdle, always with a point to go to, the head
   following, the command of func_0017EA98, and the pitch command (work byte unk79) picked as in
   LavaBird_ThinkCombat. */
static void LavaBird_ThinkIdle(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0) {
        EntityPoint_PickNear((f32)entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
        entity->target = unkE0->unk30;
        entity->unkA0 = rand() % 60 + 60;
        if (D_00413F80.position[1] - entity->model->position[1] < entity->params->unk436) {
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        }
        if (D_00413F80.position[1] - entity->model->position[1] > (s16)entity->params->unk434) { // match: read signed here
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else {
            unkE0->unk79 = 0;
        }
    }
    side = EntityAi_TurnFromPush(entity, direction);
    if (side > 0) {
        entity->unkC8 = 0x201;
    } else if (side < 0) {
        entity->unkC8 = 0x101;
    } else {
        // match: called like EntityAi_MoveCommand, with a speed and a mode the callee ignores
        entity->unkC8 = func_0017EA98(entity, 0, 0.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: keeps animation 3 on both motion slots, flag bits 11 to 13 at 0. LavaBird_ApplyMoveCommand
   does the turning and the moving. */
static void LavaBird_Move(Entity *entity) {
    entity->flags &= ~0x3800ULL;
    EntityAi_StartSlotMotion(entity, entity->unkE0.unk80, 0, 3, 5, ENTITY_CONTROLLER(entity)->unkC4, 0);
    EntityAi_StartSlotMotion(entity, entity->unkE0.unk88, 1, 3, 5, ENTITY_CONTROLLER(entity)->unkC4, 0);
}

/* Carries out the movement command in Entity.unkC8; LavaBird_Think and LavaBird_Dormant
   call it (the move handler only keeps the animation). It turns the entity, rolls it to one side or
   the other with the turn bits, eases its pitch towards 10 degrees for each unit of work byte unk79
   (back to level at 0), and speeds up or slows down along its axes by EntityParams.unk50 a frame up
   to unk54. */
static void LavaBird_ApplyMoveCommand(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 roll;
    f32 step;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
        roll = (unkE0->unk54 + 50.0f) * 0.017453292f;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
        roll = (-50.0f - unkE0->unk54) * 0.017453292f;
    } else {
        if (entity->params->unk58 < unkE0->unk1C) {
            unkE0->unk1C -= entity->params->unk58;
        } else if (unkE0->unk1C < -entity->params->unk58) {
            unkE0->unk1C += entity->params->unk58;
        } else {
            unkE0->unk1C = 0.0f;
        }
        roll = 0.0f;
    }
    entity->unk94 += unkE0->unk1C;
    EntityMath_EaseAngleToward(&entity->model->unkD0[2], roll, 60.0f);
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
    step = entity->params->unk50;
    if (entity->unkC8 & 1) {
        unkE0->unk48 -= step;
        if (unkE0->unk48 < -entity->params->unk54) {
            unkE0->unk48 = -entity->params->unk54;
        }
    } else if (entity->unkC8 & 2) {
        unkE0->unk48 += step;
        if (entity->params->unk54 < unkE0->unk48) {
            unkE0->unk48 = entity->params->unk54;
        }
    } else if (step < unkE0->unk48) {
        unkE0->unk48 -= step;
    } else if (unkE0->unk48 < -step) {
        unkE0->unk48 += step;
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
    // match: a test whose result is never used (DarkSorcerer_ApplyMoveCommand doubles the speed on this bit); it leaves
    // no code, but the scratchpad reads below only get their own address loads when it is here
    if (entity->unkC8 & 0x40) {
        step += step;
    }
    entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
    entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
    entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
}

/* Called like EntityAi_MoveCommand; the last two parameters are not used. */
static s32 func_0017EA98(Entity *entity, s32 mark_far, f32 range) {
    s32 result;
    f32 angle;
    f32 limit;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = EntityMath_HeadingTo(entity->model->position, entity->target);
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], angle);
    limit = entity->params->turn_limit;
    if ((0.0f <= angle) ? (limit < angle) : (limit < -angle)) {
        if (0.0f <= angle) {
            result = 0x201;
        } else {
            result = 0x101;
        }
    }
    return result;
}

/* The dormant action (busy bit 0x20), on the motion slots: holds animation 1 until the player is
   within EntitySpawnInfo.unk54 or flag bit 43 is set; then it starts effect 0x166 at model point
   0xFF and moves the entity in y by a speed that starts at EntityParams.unk43A and falls off, plays
   animation 2 to its end while LavaBird_ApplyMoveCommand moves it with command 1, goes to state 2 and thinks.
   */
static void LavaBird_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &slots->unk80, 0, 1, 0, 0, 1);
        EntityAi_StartSlotMotion(entity, &slots->unk88, 1, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            unkE0->unk50 = entity->params->unk43A;
            EntityEvent_StartEffectAtPoint((char **)entity, 0x166, 0xFF, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        entity->model->position[1] -= unkE0->unk50;
        unkE0->unk50 -= 3.0f;
        if (unkE0->unk50 <= 0.0f) {
            EntityAi_StartSlotMotion(entity, &slots->unk80, 0, 2, 0, entity->params->unkC4, 1);
            EntityAi_StartSlotMotion(entity, &slots->unk88, 1, 2, 0, entity->params->unkC4, 1);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->position[1] -= unkE0->unk50;
        unkE0->unk50 -= 1.0f;
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (slots->unk80.unk02 < 0x32) {
            entity->model->position[1] -= unkE0->unk50;
            unkE0->unk50 -= 0.1f;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        entity->unkC8 = 1;
        LavaBird_ApplyMoveCommand(entity);
        break;
    }
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack on the first motion slot, flag bits 11
   to 13 at 0; on the frame of the record's first shot LavaBird_LaunchBothShots launches at
   the player. */
static void LavaBird_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->bits58);
    s32 frame;
    s32 launch_frame;
    s32 current_frame;
    f32 unused_vector[4]; // match: the frame has 16 unused bytes, and they keep the tail calls from becoming jumps

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
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
        frame = (slots->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        current_frame = slots->unk80.unk02;
        launch_frame = ((EntityParamsUnk3D8Full *)unk3D8)->unk10[0].unk02;
        if (launch_frame != 0 && frame < launch_frame && launch_frame <= current_frame) {
            LavaBird_LaunchBothShots(entity, unk3D8, frame, current_frame, D_00413F80.position, 0.7853982f, 0.7853982f);
        }
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

/* Helper of LavaBird_RangedAttack: launches both shots of a record at once, whatever the
   animation frame, each from its point and turned towards `target` by at most the two rates. */
static s32 LavaBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
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

/* The on_hit handler: as EntityAi_OnHit with the type's own two reactions; a light hit starts
   LavaBird_ReactionLight whether flag bit 19 is set or not. */
static void LavaBird_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))LavaBird_ReactionLight, 0);
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))LavaBird_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the first motion slot,
   flag bits 11 to 13 at 0. */
static void LavaBird_ReactionLight(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xE, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
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

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the motion slots, flag
   bits 11 to 13 at 0. */
static void LavaBird_ReactionHeavy(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xF, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk88, 1, 0xF, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
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
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots, flag bits 11 to 13 at
   0. */
static void LavaBird_Death(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0x10, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk88, 1, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
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

/* The animate handler: puts flag bits 11 to 13 to 0, then plays the two motion slots of the work
   area (unk80 and unk88) where EntityAi_Animate plays the model's one motion, then turns the head
   (EntityAi_LookAtPoint with the target while flag bit 9 is set and there is one). */
static void LavaBird_Animate(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 limits[4];
    f32 unused_vector[4]; // match: the frame has 16 unused bytes
    s32 previous;
    f32 rate;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
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
