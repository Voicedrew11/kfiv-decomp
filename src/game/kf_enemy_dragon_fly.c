/* DragonFly: the behaviour of entity type 17 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dragon Fly. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
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
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn3D(char *);
void EntityAi_StepState(void *);
f32 EntityCollision_DistanceToGround(f32 *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand3D(Entity *, s32, f32);
void Vector_Normalize(void *, void *);
void Vector_Cross(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
int rand(void);
void EntityPoint_PickNearWithHeight(f32 *, f32, f32, void *, void *);
int EntityAi_PushTurnCommand3D(Entity *, int);
extern PlayerData D_00413F80;
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
float EntityMath_AngleDifference(float, float);
void Vector_Copy(void *, void *);
f32 EntityMath_Distance(f32 *, f32 *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);

static void DragonFly_Animate(Entity *entity);
static void DragonFly_Think(Entity *entity, s32 unknown);
static void DragonFly_ThinkReturn(Entity *entity, s32 unknown);
static void DragonFly_ThinkWander(Entity *entity, s32 unknown);
static void DragonFly_ThinkCombat(Entity *entity, s32 unknown);
static void DragonFly_Move(Entity *entity);
static void DragonFly_MeleeAttack(Entity *entity);
static void DragonFly_RangedAttack(Entity *entity);

/* Set-up of a type 17 entity: clears its work area and installs the type's handlers. */
void DragonFly_Init(Entity *entity) {
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
    unkE0->think = (void (*)(Entity *, s32))DragonFly_Think;
    unkE0->animate = (void (*)(void))DragonFly_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))DragonFly_Move;
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
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void DragonFly_Animate(Entity *entity) {
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

/* The think handler: as EntityAi_Think, with EntityAi_CheckFarFromSpawn3D where that one calls EntityAi_CheckFarFromSpawn,
   DragonFly_ThinkReturn while flag bit 53 is set, DragonFly_ThinkCombat for state 2 and
   DragonFly_ThinkWander for the other states. It also moves the entity in y so that the ground
   EntityCollision_DistanceToGround finds is no nearer than Entity.unk30. */
static void DragonFly_Think(Entity *entity, s32 unknown) {
    f32 floor_distance;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn3D((char *)entity);
    EntityAi_StepState(entity);
    floor_distance = EntityCollision_DistanceToGround(entity->model->position);
    if (floor_distance < entity->unk30) {
        entity->model->position[1] -= entity->unk30 - floor_distance;
    }
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        DragonFly_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        DragonFly_ThinkCombat(entity, unknown);
        return;
    }
    DragonFly_ThinkWander(entity, unknown);
}

/* The think function while flag bit 53 is set: as EntityAi_ThinkReturn, with flag bits 11 to 13 at
   0, the command of EntityAi_MoveCommand3D (far marked), and, while it is pushed, command bit 0x40 and bit
   0x20 or 0x10 by the y of the push. */
static void DragonFly_ThinkReturn(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->unkC8 = EntityAi_MoveCommand3D(entity, 1, 50.0f);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (0xC000L << 31)) {
            push[0] += entity->unkA4[0];
            push[1] += entity->unkA4[1];
            push[2] += entity->unkA4[2];
        }
        if ((s32)(entity->flags >> 48) & 1) {
            push[0] += entity->unkB0[0];
            push[1] += entity->unkB0[1];
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
            entity->unkC8 = 0x241;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x261;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x251;
            }
        } else if (turn < 0) {
            entity->unkC8 = 0x141;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x161;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x151;
            }
        } else {
            entity->unkC8 = EntityAi_MoveCommand3D(entity, 1, 50.0f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of every state but 2 (DragonFly_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNearWithHeight; 79 times in 100) or no
   target, takes the command of EntityAi_MoveCommand3D towards it (or of EntityAi_PushTurnCommand3D for the turn away from
   a push) and starts the move action. */
static void DragonFly_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNearWithHeight(unkE0->unk30, entity->spawn_info->unk4C, entity->spawn_info->unk56, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 30;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->unkC8 = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        entity->unkC8 = EntityAi_MoveCommand3D(entity, 1, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 2: it has none of EntityAi_ThinkCombat's guard, dodge and draw among
   the actions. Whenever the pause Entity.unkA2 is over and EntityAi_CanAttack finds the player in reach
   (and the way is not 4) it starts DragonFly_RangedAttack or DragonFly_MeleeAttack, as
   EntityAi_PickAttack draws. Otherwise it gives the command for one of the ways kept in work byte unk78:
   1 forward at the level of the player's y less 190 plus work byte unk7B; 2 forward to the level
   work float unk4C (the player's y less 220 to 919) until within 200 of that point; 3 (after an
   attack) forward for 90 frames; 4 forward with command bits 0x40 and 0x10 until the player's y is
   more than 700 above its own. */
static void DragonFly_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 target[4];
    f32 direction[4];
    f32 delta; /* angle to the wanted heading, then height to the wanted level */
    s32 turn;
    s32 roll;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (unkE0->unk78 != 4 && entity->unkA2 == 0 && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) { // match: the entity is passed
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, DragonFly_RangedAttack, unknown);
        } else {
            EntityAi_StartAction(entity, DragonFly_MeleeAttack, unknown);
        }
        unkE0->unk48 = 0.0f;
        unkE0->unk78 = 3;
        return;
    }
    if (unkE0->unk78 == 0) {
        if (rand() % 100 < 75) {
            roll = rand() % 60;
            unkE0->unk78 = 1;
            unkE0->unk7B = roll;
        } else {
            unkE0->unk4C = D_00413F80.position[1] - 220.0f - (f32)(rand() % 700);
            if (entity->radius + 100.0f + 200.0f < entity->distance_to_player) {
                unkE0->unk7A = 1;
            } else {
                unkE0->unk7A = 0;
            }
            entity->unkA0 = 120;
            unkE0->unk78 = 2;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->unkC8 = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        switch (unkE0->unk78) {
        case 1:
            delta = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - 190.0f + (f32)unkE0->unk7B - entity->model->position[1];
            if (delta > 10.0f) {
                entity->unkC8 |= 0x20;
            } else if (delta < -10.0f) {
                entity->unkC8 |= 0x10;
            }
            entity->unkC8 |= 1;
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->unkC8 |= 0x40;
            }
            break;
        case 2:
            delta = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = unkE0->unk4C - entity->model->position[1];
            if (delta > 30.0f) {
                entity->unkC8 |= 0x20;
            } else if (delta < -30.0f) {
                entity->unkC8 |= 0x10;
            }
            entity->unkC8 |= 1;
            if (unkE0->unk7A != 0) {
                entity->unkC8 |= 0x40;
            }
            Vector_Copy(target, &D_00413F80);
            target[1] = unkE0->unk4C;
            // match: the shared prototype has no result; it returns the distance
            if (((f32 (*)(f32 *, f32 *))EntityMath_Distance)(target, entity->model->position) <= 200.0f) {
                unkE0->unk78 = 0;
            }
            break;
        case 3:
            delta = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->unkC8 |= 0x41;
            }
            unkE0->unk68++;
            if (unkE0->unk68 > 90) {
                unkE0->unk68 = 0;
                unkE0->unk78 = 4;
            }
            break;
        case 4:
            delta = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - entity->model->position[1];
            entity->unkC8 |= 0x51;
            if (delta > 700.0f) {
                unkE0->unk78 = 0;
            }
            break;
        default:
            unkE0->unk78 = 0;
            break;
        }
    }
    if (entity->handler != (void (*)(Entity *))unkE0->move) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
    }
}

/* The move handler: as EntityAi_Move, but it moves whatever flag bits 11 to 13 say, and the y part
   of the movement is eased in work float unk48. */
static void DragonFly_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 amount;
    f32 climb_step;

    vector = ENTITY_SCRATCH_VECTOR;
    turn = vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
    climb_step = 0.02f;
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
        unkE0->unk1C = turn;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 3) {
        amount = 0.0f;
        if (entity->unkC8 & 1) {
            amount = -1.0f;
        } else if (entity->unkC8 & 2) {
            amount = 1.0f;
        }
        vector[2] += amount;
    }
    if (entity->unkC8 & 0xC) {
        amount = 0.0f;
        if (entity->unkC8 & 4) {
            amount = 1.0f;
        } else if (entity->unkC8 & 8) {
            amount = -1.0f;
        }
        vector[0] += amount;
    }
    if (entity->unkC8 & 0x30) {
        amount = 0.0f;
        if (entity->unkC8 & 0x10) {
            unkE0->unk48 -= climb_step;
            if (unkE0->unk48 < -1.0f) {
                unkE0->unk48 = -1.0f;
            }
            amount = unkE0->unk48;
        } else if (entity->unkC8 & 0x20) {
            unkE0->unk48 += climb_step;
            if (1.0f < unkE0->unk48) {
                unkE0->unk48 = 1.0f;
            }
            amount = unkE0->unk48;
        }
        vector[1] += amount;
    }
    if (entity->unkC8 & 0x3F) {
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
        entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
        entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
        entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
    } else {
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, (s32)((f32)entity->params->unkC4 * 0.5f), 0);
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

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack with flag bits 11 to 13 at 0; it leaves
   flag bit 9 as it is. */
static void DragonFly_MeleeAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack with flag bits 11 to 13 at 0; it leaves
   flag bit 9 as it is. */
static void DragonFly_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
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
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
