/* Skull: the behaviour of entity type 65 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Skull. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_shot.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
int rand(void);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
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
void func_00196D48(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleDifference(float, float);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
void EntityEvent_OnDeath(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float EntityMath_WrapAngle(float);
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
char *func_0010CAB8(char *, u32);
int func_001B4F30(u16, void *, void *, void *, void *);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void Skull_Think(Entity *entity, s32 unknown);
static void Skull_ThinkCombat(Entity *entity, s32 unknown);
static void Skull_Move(Entity *entity);
static void Skull_Death(Entity *entity);
static void Skull_ApplyMoveCommand(Entity *entity);
static void Skull_SwingPart3(Entity *entity);
static void Skull_Animate(Entity *entity);
static void Skull_RangedAttack0(Entity *entity);

/* Set-up of a type 65 entity: clears its work area and installs the type's handlers. */
void Skull_Init(Entity *entity) {
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
    unkE0->think = Skull_Think;
    unkE0->animate = (void (*)(void))Skull_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))Skull_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))Skull_Death;
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
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    unkE0->unk6A = rand() % 30;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with Skull_ThinkCombat for state 2. It keeps flag
   bits 22 and 28 set except in the death action. While bit 28 is set and no attack runs it adds
   command bit 0x20 or 0x10 (work byte unk7A), chosen anew every 20 to 34 frames (work word unk6A)
   so as to keep its y between 300 and 100 below the player's; then Skull_ApplyMoveCommand moves
   the entity. When work word unk68 has run below 0 it draws new values for
   Skull_SwingPart3 (work bytes unk7B and unk7C). */
static void Skull_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
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
                Skull_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (ENTITY_FLAG_BITS(entity)->bit28 && !(entity->unkC6 & 1)) {
        if (unkE0->unk6A <= 0) {
            if (entity->model->position[1] < D_00413F80.position[1] - 300.0f) {
                unkE0->unk7A = 1;
            } else if (D_00413F80.position[1] - 100.0f < entity->model->position[1]) {
                unkE0->unk7A = 0;
            } else {
                unkE0->unk7A = unkE0->unk7A == 0;
            }
            unkE0->unk6A = rand() % 15 + 20;
        }
        if (unkE0->unk7A != 0) {
            entity->unkC8 |= 0x20;
        } else {
            entity->unkC8 |= 0x10;
        }
    }
    Skull_ApplyMoveCommand(entity);
    if (unkE0->unk68 < 0) {
        unkE0->unk7C = (rand() % 10 + 10) * 2;
        if (rand() % 10 == 1) {
            unkE0->unk7B = 0;
        } else {
            unkE0->unk7B = rand() % entity->params->unk438;
        }
        unkE0->unk68 = unkE0->unk7C * 2;
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat with Skull_RangedAttack0 as the
   ranged attack. It attacks only while its y is between 300 and 50 below the player's; otherwise it
   sets the choice of command bit 0x20 or 0x10 (work byte unk7A) for 15 to 29 frames. One time in
   three it closes in with command bit 0x40. */
static void Skull_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & 0x100) {
        entity->flags &= ~0x3800;
    } else {
        entity->flags = (entity->flags & ~0x3800) | 0x800;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
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
            if (entity->model->position[1] < D_00413F80.position[1] - 50.0f && D_00413F80.position[1] - 300.0f < entity->model->position[1]) {
                if (EntityAi_PickAttack((char *)entity)) {
                    EntityAi_StartAction(entity, Skull_RangedAttack0, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
                }
                return;
            }
            if (entity->model->position[1] < D_00413F80.position[1]) {
                unkE0->unk7A = 1;
            } else {
                unkE0->unk7A = 0;
            }
            unkE0->unk6A = rand() % 15 + 15;
            break;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            if (rand() % 3 == 0) {
                unkE0->unk78 = 1;
            } else {
                unkE0->unk78 = 0;
            }
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
    if (entity->unkCD == 1) {
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
        } else {
            side = EntityAi_TurnFromPush(entity, direction);
            switch (unkE0->unk78) {
            case 0:
                if (side > 0) {
                    entity->unkC8 = 0x201;
                } else if (side < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case 1:
                if (side > 0) {
                    entity->unkC8 = 0x241;
                } else if (side < 0) {
                    entity->unkC8 = 0x141;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: only picks the animation, 3 with command bit 1, else 0.
   Skull_ApplyMoveCommand does the turning and the moving. */
static void Skull_Move(Entity *entity) {
    if (entity->unkC8 & 1) {
        Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; it sets Entity.unk30 to 80. */
static void Skull_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->flags &= ~0x10000000;
        entity->unk30 = 80.0f;
        entity->flags &= ~0x400000;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}

/* Helper of Skull_Think: turns and moves the entity as the command says, the speeds eased in
   work floats unk44 (sideways), unk48 (y) and unk4C (forward). While the entity is busy with
   anything but a hit reaction only command bits 0x10 and 0x20 count, and the turn and the other
   speeds ease out. */
static void Skull_ApplyMoveCommand(Entity *entity) {
    EntityUnkE0Kind16BF98 *unkE0 = (EntityUnkE0Kind16BF98 *)&entity->unkE0;
    f32 turn = 0.0f;
    f32 step;
    f32 limit;

    if (entity->unkC6 == 0 || (entity->unkC6 & 6)) {
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
        } else if (unkE0->unk4C > entity->params->unk50) {
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
        } else if (unkE0->unk44 > entity->params->unk50) {
            unkE0->unk44 -= entity->params->unk50;
        } else if (unkE0->unk44 < -entity->params->unk50) {
            unkE0->unk44 += entity->params->unk50;
        } else {
            unkE0->unk44 = 0.0f;
        }
    } else {
        if (entity->params->unk58 < unkE0->unk1C) {
            unkE0->unk1C -= entity->params->unk58;
        } else if (unkE0->unk1C < -entity->params->unk58) {
            unkE0->unk1C += entity->params->unk58;
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (unkE0->unk4C > entity->params->unk50) {
            unkE0->unk4C -= entity->params->unk50;
        } else if (unkE0->unk4C < -entity->params->unk50) {
            unkE0->unk4C += entity->params->unk50;
        } else {
            unkE0->unk4C = 0.0f;
        }
        if (entity->params->unk50 < unkE0->unk44) {
            unkE0->unk44 -= entity->params->unk50;
        } else if (unkE0->unk44 < -entity->params->unk50) {
            unkE0->unk44 += entity->params->unk50;
        } else {
            unkE0->unk44 = 0.0f;
        }
    }
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50 * 0.8f;
    limit = entity->params->unk54 * 0.5f;
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

/* Helper of Skull_Animate: swings angle unk50[0] of model part 3 up to work byte unk7B
   degrees and back to 0 over twice work byte unk7C frames, counted down in work word unk68; the
   angle is kept in work float unk50. */
static void Skull_SwingPart3(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *part = &((EntityModelPart80 *)entity->model->unkF8)[3];
    f32 angle;

    if (part != NULL) {
        if (unkE0->unk68 >= unkE0->unk7C) {
            unkE0->unk50 += (f32)unkE0->unk7B / (f32)unkE0->unk7C * 0.017453292f;
            if (unkE0->unk7B * 0.017453292f < unkE0->unk50) {
                unkE0->unk50 = unkE0->unk7B * 0.017453292f;
            }
        } else {
            unkE0->unk50 -= (f32)unkE0->unk7B / (f32)unkE0->unk7C * 0.017453292f;
            if (unkE0->unk50 < 0.0f) {
                unkE0->unk50 = 0.0f;
            }
        }
        angle = unkE0->unk50;
        part->unk50[0] = angle;
        part->unk50[0] = EntityMath_WrapAngle(angle);
        unkE0->unk68--;
    }
}

/* The animate handler: as EntityAi_Animate without the head turn. Skull_SwingPart3 swings
   model part 3 while the entity is not busy. Every tenth frame it launches shot 0xD0 from the point
   func_0010CAB8 gives for 0xFF (func_001B4F30); on the other frames it starts effect 0x39D at model
   point 0xFF. */
static void Skull_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;
    EntityUnk1B1390 *shot;
    f32 unused[4]; /* retail reserves the 0x10 bytes */

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x6000000) {
            entity->flags = flags & ~0x3800L;
        }
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        if (entity->unkC6 == 0 && entity->model->unkFC->unk88 == 0) {
            Skull_SwingPart3(entity);
        } else {
            unkE0->unk50 = 0.0f;
        }
        if (D_00446ED4 % 10U == 0) {
            shot = Shot_Alloc();
            shot->unk00 = 1;
            shot->unk20 = 0;
            shot->unk22 = 0;
            shot->unk28 = entity->params->unk238;
            func_001B4F30(0xD0, shot, func_0010CAB8((char *)entity->model, 0xFF), entity->model->unkD0, entity);
        } else {
            EntityEvent_StartEffectAtPoint((char **)entity, 0x39D, 0xFF, (int)entity->model->unkD0);
        }
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame EntityParams.unk43A to
   unk43C launches the record's shots every other frame. */
static void Skull_RangedAttack0(Entity *entity) {
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
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (entity->model->unkFC->unk82 >= entity->params->unk43A && entity->model->unkFC->unk82 <= entity->params->unk43C &&
            (D_00446ED4 & 1)) {
            EntityAi_LaunchShots(entity, unk3D8, entity->params->unk43A, entity->params->unk43C);
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
