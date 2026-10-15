/* Spider: the behaviour of entity type 3 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Spider, Blue Spider. The game names
   no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records
   by hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
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
float EntityMath_DistanceXZ(void *, void *);
void func_00196D48(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void Vector_Copy(void *, void *);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityEvent_OnDeath(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, int *, float *);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);
s32 EntityCollision_RayProps(f32 *, f32 *, void *, f32);

static void Spider_Animate(Entity *entity);
static void Spider_Think(Entity *entity, s32 unknown);
static void Spider_ThinkWander(Entity *entity, s32 unknown);
static void Spider_ThinkCombat(Entity *entity, s32 unknown);
static void Spider_RangedAttack(Entity *entity);
static void Spider_Dormant(Entity *entity);
static void Spider_OnHit(Entity *entity);
static void Spider_Death(Entity *entity);
static void func_001299F8(Entity *entity);
static void func_00129CA0(Entity *entity);

/* Set-up of a type 3 entity: clears its work area and installs the type's handlers (one handler table when it
   starts with its action, another when it starts asleep). */
void Spider_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        unkE0->think = Spider_Think;
        unkE0->animate = (void (*)(void))Spider_Animate;
        unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
        unkE0->move = (void (*)(void))EntityAi_Move;
        unkE0->dormant = (void (*)(void))Spider_Dormant;
        unkE0->death = (void (*)(void))EntityAi_Death;
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        unkE0->think = Spider_Think;
        unkE0->animate = (void (*)(void))Spider_Animate;
        unkE0->on_hit = (void (*)(void))Spider_OnHit;
        unkE0->move = (void (*)(void))EntityAi_Move;
        unkE0->dormant = (void (*)(void))Spider_Dormant;
        unkE0->death = (void (*)(void))Spider_Death;
        EntityAi_StartAction(entity, func_001299F8, 1);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler (installed in both branches of the set-up function): as EntityAi_Animate
   without the test of the EntityParams kind bits and without the head turn; while work byte unk7A
   and flag bit 53 are both set it reverses this frame's turn, Entity.unk94. */
static void Spider_Animate(Entity *entity) {
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
    if (unkE0->unk7A != 0) {
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            entity->unk94 = -entity->unk94;
        }
    }
}

/* The think handler (installed in both branches of the set-up function): as EntityAi_Think, with an
   if in place of the switch: Spider_ThinkCombat in state 2, Spider_ThinkWander in the other states.
   Before that, while work byte unk7A is set (func_001299F8 sets it), it clears it and starts
   func_00129CA0 once the player is within EntityParams.unk432. */
static void Spider_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (unkE0->unk7A != 0 && entity->distance_to_player < (float)entity->params->unk432) {
        unkE0->unk7A = 0;
        EntityAi_StartAction(entity, func_00129CA0, 1);
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
    } else if (entity->state == 2) {
        Spider_ThinkCombat(entity, unknown);
    } else {
        Spider_ThinkWander(entity, unknown);
    }
}

/* The think function of every state but 2 (Spider_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 59 times in 100) or no
   target, takes the command of EntityAi_MoveCommand towards it (or the turn away from a push,
   EntityAi_TurnFromPush) and starts the move action. */
static void Spider_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 40) {
            EntityPoint_PickNear(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
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

/* The think function of state 2 (called under `entity->state == 2`, not from a switch): as
   EntityAi_ThinkCombat with Spider_RangedAttack as the ranged attack; the head does not
   follow. It closes in on the player only while the player is within EntitySpawnInfo.unk4C of the
   spawn point; otherwise it goes for a point near the spawn point. */
static void Spider_ThinkCombat(Entity *entity, s32 unknown) {
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
                EntityAi_StartAction(entity, Spider_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            if (EntityMath_DistanceXZ(&D_00413F80, entity->spawn_info) < (f32)entity->spawn_info->unk4C) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 120;
                unkE0->unk78 = 0;
            } else {
                EntityPoint_PickNear(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
                entity->target = unkE0->unk30;
                entity->unkA0 = rand() % 60 + 120;
                unkE0->unk78 = 1;
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
            switch (unkE0->unk78) {
            case 0:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(entity->radius + 100.0f + 30.0f, entity, 0);
                }
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(50.0f, entity, 0);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, the shots turned towards a point 30
   below the player (EntityShot_LaunchAimed). */
static void Spider_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(direction, &D_00413F80);
        direction[1] -= 30.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction, 0.7853982f, 0.17453294f);
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

/* The dormant action (busy bit 0x20), installed in both branches of the set-up function and started
   when the spawn has flag bit 58: plays animation 3 until the entity is EntityParams.unk430 or more
   from its spawn point (EntityMath_DistanceXZ), then thinks. It does not wait for the player. */
static void Spider_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 3, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) >= (f32)entity->params->unk430) {
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
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

/* The on_hit handler that the set-up function installs when the spawn has flag bit 58 clear, and
   func_001299F8 again when the drop ends on result 2 or 3; otherwise, and once func_00129CA0 has
   ended, the slot holds EntityAi_OnHit. No reaction: it starts func_00129CA0 and goes to state 2.
   */
static void Spider_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    EntityAi_StartAction(entity, func_00129CA0, 0);
    EntityAi_SetState((char *)entity, 2);
}

/* The death action (busy bit 0x40), installed and replaced together with Spider_OnHit
   (otherwise the slot holds EntityAi_Death): as EntityAi_Death, while it brings the model's pitch
   and roll back to 0; it clears flag bits 22 to 24 and 27 at the start and sets 26 and 27 for the
   fade. */
static void Spider_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    EntityMath_EaseAngleToward(&entity->model->unkD0[0], 0.0f, 6.0f);
    EntityMath_EaseAngleToward(&entity->model->unkD0[2], 0.0f, 6.0f);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
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

/* Action: drops along its own up axis until it lands on a floor (awake) or on something else (asleep). */
static void func_001299F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 landing[4];
    f32 direction[4];
    f32 matrix[16];
    s32 hit;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    direction[0] = 0.0f;
    direction[1] = 1.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    entity->unkC6 = 0x1000;
    sceVu0RotMatrixZ(matrix, D_00431AF0, entity->model->unkD0[2]);
    sceVu0RotMatrixX(matrix, matrix, entity->model->unkD0[0]);
    sceVu0RotMatrixY(matrix, matrix, entity->model->unkD0[1]);
    sceVu0ApplyMatrix(direction, (int *)matrix, direction);
    hit = EntityCollision_RayProps(entity->model->position, direction, landing, 200.0f);
    if (hit == 1) {
        Vector_Copy(entity->model->position, landing);
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        unkE0->unk7A = 0;
        unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
        unkE0->death = (void (*)(void))EntityAi_Death;
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
        }
    } else if (hit == 2 || hit == 3) {
        Vector_Copy(entity->model->position, landing);
        unkE0->unk48 = landing[0];
        unkE0->unk4C = landing[1];
        unkE0->unk50 = landing[2];
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        unkE0->unk7A = 1;
        unkE0->on_hit = (void (*)(void))Spider_OnHit;
        unkE0->death = (void (*)(void))Spider_Death;
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
        }
    } else {
        Vector_Scale(direction, direction, 20.0f);
        Vector_Add(entity->model->position, entity->model->position, direction);
    }
}

/* Action: levels the model out (pitch and roll back to 0), then goes back to its handler. */
static void func_00129CA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 pitch_left;
    f32 roll_left;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkC6 = 0x2000;
    pitch_left = EntityMath_EaseAngleToward(&entity->model->unkD0[0], 0.0f, 6.0f);
    roll_left = EntityMath_EaseAngleToward(&entity->model->unkD0[2], 0.0f, 6.0f);
    if (entity->flags & (0x8200L << 35)) {
        pitch_left = ABS(pitch_left);
        roll_left = ABS(roll_left);
        if (ABS(pitch_left) < 0.01f && ABS(roll_left) < 0.01f) {
            entity->model->unkD0[0] = 0.0f;
            entity->model->unkD0[2] = 0.0f;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
            unkE0->death = (void (*)(void))EntityAi_Death;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
        }
    }
}
