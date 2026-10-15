/* Beetle: the behaviour of entity type 5 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Beetle Small, Beetle Very Large,
   Beetle White, Beetle Medium. The game names no monsters; these names are from
   https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points and experience, with
   resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
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
void EntityAi_StandOnce(Entity *);
int rand(void);
void EntityPoint_PickNear(float, void *, void *, void *);
int EntityAi_TurnFromPush(void *, void *);
s16 EntityAi_MoveCommand(float, void *, int); // match: this file only matches with this type; EntityAi_MoveCommand.c defines it differently
extern PlayerData D_00413F80;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
float EntityMath_EaseAngleToward(float *, float, float);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void Beetle_Animate(Entity *entity);
static void Beetle_Think(Entity *entity, s32 unknown);
static void Beetle_ThinkWander(Entity *entity, s32 unknown);
static void Beetle_ThinkCombat(Entity *entity, s32 unknown);
static void Beetle_TurnParts7And12(void *entity_arg);
static void Beetle_TurnParts23To32(Entity *entity);
static void Beetle_RangedAttack0(Entity *entity);

/* Set-up of a type 5 entity: clears its work area and installs the type's handlers. */
void Beetle_Init(Entity *entity) {
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
    unkE0->think = Beetle_Think;
    unkE0->animate = (void (*)(void))Beetle_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
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
   without the head turn; Beetle_TurnParts7And12 and Beetle_TurnParts23To32 turn model parts
   instead. */
static void Beetle_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 2) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        Beetle_TurnParts7And12(entity);
        Beetle_TurnParts23To32(entity);
    }
}

/* The think handler: as EntityAi_Think, with Beetle_ThinkCombat for state 2 and Beetle_ThinkWander
   for states 0 and 1; when it comes to state 0 from another state it starts the shared action
   EntityAi_StandOnce instead. */
static void Beetle_Think(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        if (entity->prev_state != 0) {
            EntityAi_StartAction(entity, EntityAi_StandOnce, unknown);
        } else {
            Beetle_ThinkWander(entity, unknown);
        }
        return;
    case 1:
        break;
    case 2:
        Beetle_ThinkCombat(entity, unknown);
        return;
    }
    Beetle_ThinkWander(entity, unknown);
}

/* The think function of every state but 2 (Beetle_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 79 times in 100, for 120 to
   179 frames) or no target (60 to 89 frames), takes the command of EntityAi_MoveCommand towards it (or the
   turn away from a push, EntityAi_TurnFromPush) and starts the move action. */
static void Beetle_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
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

/* The think function of state 2: as EntityAi_ThinkCombat with Beetle_RangedAttack0 as the
   ranged attack; it leaves flag bits 9 and 10 as they are, and closes in with command bit 0x40
   (EntityAi_MoveCommand with range 0, far marked). */
static void Beetle_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
                EntityAi_StartAction(entity, Beetle_RangedAttack0, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
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
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(0.0f, entity, 1);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Helper of Beetle_Animate: eases work float unk48 towards 20 degrees on the side the command
   turns to (0 with no turn bit) and puts half of it in angle unk50[1] of model parts 7 and 12. */
static void Beetle_TurnParts7And12(void *entity_arg) {
    Entity *entity = entity_arg;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *part;
    f32 angle;
    f32 limit;
    s32 i;

    limit = 0.34906584f;
    if (entity->unkC8 & 0x200) {
        angle = limit;
    } else if (entity->unkC8 & 0x100) {
        angle = -limit;
    } else {
        angle = 0.0f;
    }
    limit = 0.34906584f;
    if (angle > limit) {
        angle = limit;
    } else if (angle < -limit) {
        angle = -limit;
    }
    EntityMath_EaseAngleToward(&unkE0->unk48, angle, 10.0f);
    part = (EntityModelPart80 *)entity->model->unkF8;
    for (i = 0; i < ENTITY_MODEL(entity)->part_count; i++) {
        if (i == 7 || i == 12) {
            part->unk50[1] = unkE0->unk48 * 0.5f;
        }
        part++;
    }
}

/* Helper of Beetle_Animate: eases work float unk4C towards 20 degrees on the side the command
   turns to (0 with no turn bit) and puts minus half of it in angle unk50[1] of model parts 23, 28,
   31 and 32 (the name gives the first and the last of the four). */
static void Beetle_TurnParts23To32(Entity *entity) {
    f32 limit;
    f32 lean;
    EntityUnkE0 *unkE0;
    EntityModelPart80 *part;
    u16 command;
    u16 part_count;
    s32 i;

    f32 limit2;
    limit = 0.34906584f;
    command = entity->unkC8;
    unkE0 = &entity->unkE0;
    if (command & 0x200) {
        lean = limit;
    } else {
        lean = (command & 0x100) ? -limit : 0.0f;
    }
    limit2 = 0.34906584f;
    if (limit2 < lean || (limit2 = -limit2, lean < limit2)) {
        lean = limit2;
    }
    EntityMath_EaseAngleToward(&unkE0->unk4C, lean, 20.0f);
    part_count = ENTITY_MODEL(entity)->part_count;
    part = (EntityModelPart80 *)entity->model->unkF8;
    for (i = 0; i < part_count; i++) {
        if (i == 0x17 || i == 0x1C || i == 0x1F || i == 0x20) {
            part->unk50[1] = -unkE0->unk4C * 0.5f;
        }
        part++;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame EntityParams.unk430 to
   unk432 it launches the record's shots on every frame whose number is not a multiple of
   EntityParams.unk434. */
static void Beetle_RangedAttack0(Entity *entity) {
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
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (entity->model->unkFC->unk82 >= entity->params->unk430
            && entity->model->unkFC->unk82 <= entity->params->unk432) {
            if (ENTITY_CONTROLLER(entity)->unk434 == 0) {
                ENTITY_CONTROLLER(entity)->unk434++;
            }
            if (D_00446ED4 % ENTITY_CONTROLLER(entity)->unk434 != 0) {
                EntityAi_LaunchShots(entity, unk3D8, entity->params->unk430, entity->params->unk432);
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
