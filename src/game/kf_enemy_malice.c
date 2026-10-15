/* Malice: the behaviour of entity type 58 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Malice. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_effect.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
extern f32 D_00446814[];
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
extern PlayerData D_00413F80;
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_DistanceXZ(void *, void *);
void EntityPoint_PickNearFromDistance(f32 *, f32 *, f32, f32);
void func_00196D48(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
extern f32 D_00446818[];
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_WrapAngle(float);
float EntityMath_HeadingTo(float *, float *);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
float atanf(float);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);

static void Malice_Think(Entity *entity, s32 unknown);
static void Malice_ThinkCombat(Entity *entity, s32 unknown);
static void Malice_Move(Entity *entity);
static void Malice_ApplyMoveCommand(Entity *entity);
static void Malice_ToggleFadeLevel(Entity *entity);
static void Malice_FadeOutAndRelocate(Entity *entity);
static void Malice_FadeBackIn(Entity *entity);
static void Malice_OnHit(Entity *entity);
static void Malice_Dormant(Entity *entity);
static void Malice_RangedAttack0(Entity *entity);
static void Malice_Death(Entity *entity);

/* Set-up of a type 58 entity: clears its work area and installs the type's handlers. */
void Malice_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Malice_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))Malice_OnHit;
    unkE0->move = (void (*)(void))Malice_Move;
    unkE0->dormant = (void (*)(void))Malice_Dormant;
    unkE0->death = (void (*)(void))Malice_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
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
    entity->unkD6 = 0;
    entity->model->unk15C = 0.0f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with Malice_ThinkCombat for state 2. It also keeps
   flag bits 18, 20, 21, 26 and 27 set while work byte unk7B is set or work word unk68 is 1, counts
   down work word unk6A, adds command bit 0x10 or 0x20 by what EntityCollision_DistanceToPropFloor reads at the entity's
   position against EntityParams.unk43C and unk43E (work byte unk7A), and calls
   Malice_ApplyMoveCommand while the entity is not busy or has one of the busy bits 0x3006. */
static void Malice_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (unkE0->unk7B != 0 || unkE0->unk68 == 1) {
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
    } else {
        ENTITY_FLAG_BITS(entity)->bit20 = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 0;
        ENTITY_FLAG_BITS(entity)->bit26 = 0;
        ENTITY_FLAG_BITS(entity)->bit27 = 0;
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
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
                Malice_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (ENTITY_FLAG_BITS(entity)->bit28) {
        height = EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position);
        if (height != D_00446814[0]) {
            if (entity->params->unk43C < height) {
                unkE0->unk7A = 1;
            } else if (height < entity->params->unk43E) {
                unkE0->unk7A = 0;
            }
            if (unkE0->unk7A != 0) {
                entity->unkC8 |= 0x20;
            } else {
                entity->unkC8 |= 0x10;
            }
        } else {
            unkE0->unk7A = 0;
            entity->unkC8 |= 0x10;
        }
    }
    if (entity->unkC6 == 0 || (entity->unkC6 & 0x3006)) {
        Malice_ApplyMoveCommand(entity);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but first it starts
   Malice_ToggleFadeLevel (when work byte unk7B is 1 and work word unk6A has run out, or with a
   chance of EntityParams.unk43A in 100 when the timer is at 0) or Malice_FadeOutAndRelocate (a
   chance of unk438 in 100). It attacks only with unk7B at 0 and the player within 200 in y, and
   else goes to a point EntityPoint_PickNearFromDistance picks; ranged record 0 is Malice_RangedAttack0. Closing
   in never runs. */
static void Malice_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 retreat_point[4];
    f32 destination[4];
    f32 direction[4];
    EntityModel *model;
    f32 height_difference;
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
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (unkE0->unk7B == 1 && unkE0->unk6A == 0) {
        EntityAi_StartAction(entity, Malice_ToggleFadeLevel, 1);
        return;
    }
    if (entity->unkA0 == 0 && rand() % 100 < entity->params->unk43A) {
        EntityAi_StartAction(entity, Malice_ToggleFadeLevel, 1);
        return;
    }
    if (entity->unkA0 == 0 && rand() % 100 < entity->params->unk438) {
        EntityAi_StartAction(entity, Malice_FadeOutAndRelocate, 1);
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
            model = entity->model;
            height_difference = D_00413F80.position[1] - model->position[1];
            if (ABS(height_difference) > 200.0f) {
                EntityPoint_PickNearFromDistance(retreat_point, model->position, EntityMath_DistanceXZ(model->position, entity->spawn_info), entity->radius * 4.0f);
                entity->target = retreat_point;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
            } else if (unkE0->unk7B == 0) {
                if (EntityAi_PickAttack((char *)entity)) {
                    if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                        EntityAi_StartAction(entity, Malice_RangedAttack0, unknown);
                    } else {
                        EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                    }
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
                }
                return;
            } else {
                EntityPoint_PickNearFromDistance(destination, model->position, EntityMath_DistanceXZ(model->position, entity->spawn_info), entity->radius * 4.0f);
                entity->target = destination;
                entity->unkA0 = rand() % 30 + 60;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
            }
            break;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
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
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: only picks the animation, 3 going forward and 0 otherwise;
   Malice_ApplyMoveCommand moves the entity. */
static void Malice_Move(Entity *entity) {
    if (entity == D_00413F80.unk204) {
        if (entity->params->flags & 0x800) {
            entity->unkC8 = 0;
        }
    }
    if (entity->unkC8 & 1) {
        Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
}

/* Helper of Malice_Think: carries out the command in Entity.unkC8. It turns the entity and
   speeds up or slows down along its three axes by EntityParams.unk50 a frame up to unk54 (work
   floats unk44, unk48 and unk4C; in y half the step and a fifth of the limit), twice as fast for
   command bit 0x40. */
static void Malice_ApplyMoveCommand(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;
    f32 step;
    f32 limit;

    turn = 0.0f;
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
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50 * 0.5f;
    limit = entity->params->unk54 * 0.2f;
    if (entity->unkC8 & 1) {
        unkE0->unk4C -= entity->params->unk50;
        if (unkE0->unk4C < -entity->params->unk54) {
            unkE0->unk4C = -entity->params->unk54;
        }
    } else if (entity->unkC8 & 2) {
        unkE0->unk4C += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk4C) {
            unkE0->unk4C = entity->params->unk54;
        }
    } else if (entity->params->unk50 < unkE0->unk4C) {
        unkE0->unk4C -= entity->params->unk50;
    } else if (unkE0->unk4C < -entity->params->unk50) {
        unkE0->unk4C += entity->params->unk50;
    } else {
        unkE0->unk4C = 0.0f;
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
    if (entity->unkC8 & 0x10) {
        unkE0->unk48 -= step;
        if (unkE0->unk48 < -limit) {
            unkE0->unk48 = -limit;
        }
    } else if (entity->unkC8 & 0x20) {
        unkE0->unk48 += step;
        if (limit < unkE0->unk48) {
            unkE0->unk48 = limit;
        }
    } else if (step < unkE0->unk48) {
        unkE0->unk48 -= step;
    } else if (unkE0->unk48 < -step) {
        unkE0->unk48 += step;
    } else {
        unkE0->unk48 = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = unkE0->unk44;
    ENTITY_SCRATCH_VECTOR[1] = unkE0->unk48;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk4C;
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

/* An action (busy bit 0x1000): plays animation 1 while Entity.unkD6 goes by 1 a frame up to 0x30
   (work byte unk7B set) or down to 0x10 (clear), then flips unk7B, sets work word unk6A to
   EntityParams.unk436 and thinks. */
static void Malice_ToggleFadeLevel(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unkE0->unk7B != 0) {
            entity->unkD6++;
        } else {
            entity->unkD6--;
        }
        if ((unkE0->unk7B == 1 && entity->unkD6 >= 0x30) || (unkE0->unk7B == 0 && entity->unkD6 <= 0x10)) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->unkD6 = unkE0->unk7B ? 0x30 : 0x10;
            unkE0->unk7B = unkE0->unk7B == 0;
            unkE0->unk6A = entity->params->unk436;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* A second action that plays animation 1 (busy bit 0x2000): sets work word unk68 to 1 and flag bits
   18, 20, 21 and 27, calls EntityEvent_StartEffectAtPoint with 0x234 and takes Entity.unkD6 down to 0; meanwhile it
   looks for a point round the spawn point (EntityPoint_PickNearFromDistance, EntityCollision_DistanceToPropFloor) more than 45 degrees off
   the player's facing. Then it puts the entity there if it found one and starts
   Malice_FadeBackIn. */
static void Malice_FadeOutAndRelocate(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *destination;
    f32 height;
    f32 yaw;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk68 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit20 = 1;
            ENTITY_FLAG_BITS(entity)->bit18 = 1;
            ENTITY_FLAG_BITS(entity)->bit21 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            unkE0->unk7C = 0;
            unkE0->unk6E = 0;
            EntityEvent_StartEffectAtPoint((char **)entity, 0x234, 0, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        if (unkE0->unk7C == 0) {
            destination = unkE0->unk30;
            EntityPoint_PickNearFromDistance(destination, (f32 *)entity->spawn_info, EntityMath_DistanceXZ(entity->model->position, entity->spawn_info), entity->spawn_info->unk4C);
            height = EntityCollision_DistanceToPropFloor(entity->unk14, destination);
            if (height != D_00446818[0]) {
                yaw = EntityMath_AngleXZ(D_00413F80.position, (char *)destination);
                if (ABS(EntityMath_WrapAngle(yaw - D_00413F80.unk14)) > 0.7853982f) {
                    unkE0->unk30[1] += height - (f32)(rand() % 200);
                    unkE0->unk7C = 1;
                }
            }
        }
        entity->unkD6 -= (unkE0->unk7B != 0) ? 2 : 4;
        if (entity->unkD6 > 0x80) {
            entity->unkD6 = 0;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (unkE0->unk7C == 1) {
                Vector_Copy(entity->model->position, unkE0->unk30);
            }
            EntityAi_StartAction(entity, Malice_FadeBackIn, 1);
        }
        break;
    }
}

/* A third action that plays animation 1 (busy bit 0x3000), started by Malice_FadeOutAndRelocate:
   waits EntityParams.unk434 frames, turns the entity to the player, calls EntityEvent_StartEffectAtPoint with 0x235
   and takes Entity.unkD6 back up to 0x30 (0x10 with work byte unk7B set), then clears work word
   unk68 and thinks. */
static void Malice_FadeBackIn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk6C = 0;
        entity->unkD6 = 0;
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
        break;
    case 1:
        if ((s16)entity->params->unk434 < unkE0->unk6C++) {
            Motion_Start(entity, 1, 0, 0, 0);
            entity->model->unkD0[1] = EntityMath_HeadingTo(entity->model->position, (f32 *)&D_00413F80);
            EntityEvent_StartEffectAtPoint((char **)entity, 0x235, 0, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        entity->unkD6++;
        if (entity->unkD6 > (unkE0->unk7B != 0 ? 0x10 : 0x30)) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->unkD6 = unkE0->unk7B ? 0x10 : 0x30;
            unkE0->unk68 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The on_hit handler: the same as EntityAi_OnHit. */
static void Malice_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
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
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, but in place of animation 2 it takes
   Entity.unkD6 up by 1 a frame to 0x30. */
static void Malice_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
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
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        entity->unkD6++;
        if (entity->unkD6 >= 0x30) {
            entity->unkD6 = 0x30;
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
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

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame 28 to 60 it launches the
   record's shots every other frame with EntityShot_Launch, pitched at the player and along the reverse
   of its heading. */
static void Malice_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 direction[4];

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
        if (entity->model->unkFC->unk82 >= 0x1C && entity->model->unkFC->unk82 <= 0x3C) {
            Vector_Copy(direction, entity->model->unkD0);
            direction[0] = atanf((entity->model->position[1] - D_00413F80.position[1]) / entity->distance_to_player);
            direction[1] = EntityMath_WrapAngle(direction[1] - 3.1415927f);
            if (D_00446ED4 & 1) {
                EntityShot_Launch(entity, unk3D8, 0x1C, 0x3C, direction);
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

/* The death action (busy bit 0x40): as EntityAi_Death, but it frees the entity's effects
   (Effect_FreeForOwner) when EntityParams.unkC6 is neither 0 nor 0xFFFF and leaves flag bit 28
   alone. */
static void Malice_Death(Entity *entity) {
    EntityParams *params;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        params = entity->params;
        if (params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        if ((((EntityParamsWord *)params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        if (params->unkC6 != 0 && params->unkC6 != 0xFFFF) {
            Effect_FreeForOwner(entity);
        }
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
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
