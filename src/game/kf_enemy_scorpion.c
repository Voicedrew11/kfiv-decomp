/* Scorpion: the behaviour of entity type 16 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Scorpion. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
extern u32 D_00446ED4;
int EntityAi_ThinkReturn(void *, int);
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
int rand(void);
int EntityAi_TurnFromPush(void *, void *);
void EntityPoint_PickNear(float, void *, void *, void *);
s16 EntityAi_MoveCommand(float, void *, int); // match: this file only matches with this type; EntityAi_MoveCommand.c defines it differently
extern PlayerData D_00413F80;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_WrapAngle(float);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
void func_00196D48(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Normalize(void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);
int Motion_Start(void *, int, int, u32, int);

static void Scorpion_Animate(Entity *entity);
static void Scorpion_Think(Entity *entity, s32 unknown);
static int Scorpion_ThinkWander(Entity *entity, s32 unknown);
static int Scorpion_ThinkCombat(Entity *entity, s32 unknown);
static void Scorpion_Move(Entity *entity);

/* Set-up of a type 16 entity: clears its work area and installs the type's handlers. */
void Scorpion_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = (void (*)(Entity *, s32))Scorpion_Think;
    unkE0->animate = (void (*)(void))Scorpion_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))Scorpion_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void Scorpion_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
    }
}

/* The think handler: as EntityAi_Think, with Scorpion_ThinkCombat for state 2 and Scorpion_ThinkWander
   for the other states. */
static void Scorpion_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        Scorpion_ThinkCombat(entity, unknown);
        return;
    }
    Scorpion_ThinkWander(entity, unknown);
}

/* The think function of every state but 2 (Scorpion_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 21 times in 100, for 60 to
   89 frames) or no target (120 to 179 frames), takes the command of EntityAi_MoveCommand towards it (or
   the turn away from a push, EntityAi_TurnFromPush) and starts the move action. */
static int Scorpion_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        if (rand() % 100 <= 20) {
            EntityPoint_PickNear(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 30 + 60;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 60 + 120;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = EntityAi_MoveCommand(50.0f, entity, 0);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 2: guard, dodge and the choice among attacking, closing in and
   waiting as in EntityAi_ThinkCombat; the head does not follow. Closing in has three ways, kept in
   work byte unk78: 0 turns on the spot until Entity.unk68 is within 10 degrees of the heading (flag
   bits 11 to 13 at 3); 1 goes for the player; 2 moves sideways (command bit 4 or 8, by the side of
   the player's facing the entity is on, work byte unk7A) until flag bit 45, 47 or 48 is set, which
   puts it on way 1. */
static int Scorpion_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 heading;
    f32 angle;
    s32 side;
    s32 turn;
    s32 mode;

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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.17453292f) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 60;
                unkE0->unk78 = 0;
            } else {
                heading = EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position) - D_00413F80.unk14;
                heading = EntityMath_WrapAngle(heading);
                if (ABS(heading) > 0.7853982f || rand() % 100 < entity->params->unk430) {
                    entity->target = &D_00413F80;
                    entity->unkA0 = rand() % 60 + 120;
                    unkE0->unk78 = 1;
                } else {
                    if (heading >= 0.0f) {
                        unkE0->unk7A = 0;
                    } else {
                        unkE0->unk7A = 1;
                    }
                    entity->unkA0 = rand() % 30 + 60;
                    unkE0->unk78 = 2;
                }
            }
            break;
        case 2:
            entity->unkA0 = 30;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    mode = entity->unkCD;
    if (mode == 1) {
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
            turn = EntityAi_TurnFromPush(entity, &heading);
            switch (unkE0->unk78) {
            case 0:
                angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->unkC8 |= 0x200;
                    } else {
                        entity->unkC8 |= 0x100;
                    }
                }
                if (ABS(angle) <= 0.17453292f) {
                    entity->unkA0 = 0;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 3;
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(entity->radius + 100.0f + 30.0f, entity, 0);
                }
                break;
            case 2:
                angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->unkC8 |= 0x200;
                    } else {
                        entity->unkC8 |= 0x100;
                    }
                }
                if (entity->flags & (0xD000L << 33)) {
                    unkE0->unk78 = 1;
                } else if (unkE0->unk7A != 0) {
                    entity->unkC8 |= 8;
                } else {
                    entity->unkC8 |= 4;
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: as EntityAi_Move, but it turns the heading only while the command has more than
   the two turn bits, and a turn alone gets animation 11 or 12. */
static void Scorpion_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 amount;
    s32 command;
    s32 first_command;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    // match: read through the union, and again below once the turn speed is stored
    first_command = ENTITY_COMMAND(entity);
    if (first_command & 0xFCFF) {
        if (first_command & 0x100) {
            unkE0->unk1C -= entity->params->unk58;
            if (unkE0->unk1C < -entity->params->turn_limit) {
                unkE0->unk1C = -entity->params->turn_limit;
            }
        } else if (first_command & 0x200) {
            unkE0->unk1C += entity->params->unk58;
            if (entity->params->turn_limit < unkE0->unk1C) {
                unkE0->unk1C = entity->params->turn_limit;
            }
        } else {
            unkE0->unk1C = 0.0f;
        }
        command = entity->unkC8;
        turn = unkE0->unk1C;
        if (command & 0x400) {
            turn += turn;
        }
        entity->unk94 += turn;
    } else {
        command = first_command;
    }
    if (ENTITY_FLAG_BITS(entity)->bits11 == 0) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
        if (command & 3) {
            amount = 0.0f;
            if (command & 1) {
                amount = -1.0f;
            } else if (command & 2) {
                amount = 1.0f;
            }
            vector[2] += amount;
        }
        if (command & 0xC) {
            amount = 0.0f;
            if (command & 4) {
                amount = 1.0f;
            } else if (command & 8) {
                amount = -1.0f;
            }
            vector[0] += amount;
        }
        if (command & 0x30) {
            amount = 0.0f;
            if (command & 0x10) {
                amount = -1.0f;
            } else if (command & 0x20) {
                amount = 1.0f;
            }
            vector[1] += amount;
        }
        if (command & 0x3F) {
            unkE0->unk44 = EntityAi_Accelerate(entity, unkE0->unk44);
            amount = unkE0->unk44;
            if (entity->unkC8 & 0x40) {
                amount += amount;
            }
            sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
            sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[0]);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[1]);
            Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            command = entity->unkC8;
            entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
            entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
            entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
        } else {
            unkE0->unk44 = 0.0f;
        }
    } else {
        unkE0->unk44 = 0.0f;
    }
    if (command & 1) {
        if (command & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (command & 2) {
        Motion_Start(entity, 5, 0, entity->params->unkC4, 0);
    } else if (command & 0x10) {
        Motion_Start(entity, 8, 0, entity->params->unkC4, 0);
    } else if (command & 0x20) {
        Motion_Start(entity, 9, 0, entity->params->unkC4, 0);
    } else if (command & 4) {
        Motion_Start(entity, 6, 0, entity->params->unkC4, 0);
    } else if (command & 8) {
        Motion_Start(entity, 7, 0, entity->params->unkC4, 0);
    } else if (command & 0x100) {
        Motion_Start(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (command & 0x200) {
        Motion_Start(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}
