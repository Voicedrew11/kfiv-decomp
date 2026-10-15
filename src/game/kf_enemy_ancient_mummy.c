/* AncientMummy: the behaviour of entity type 42 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Ancient Mummy. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
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
void func_00196D48(Entity *);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
float EntityMath_HeadingTo(float *origin, float *target);
float EntityMath_DistanceXZ(float *a, float *b);
extern f32 D_00446810[];
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityPoint_PickNearFromDistance(f32 *, f32 *, f32, f32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
float EntityMath_WrapAngle(float);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetState(char *, u8);

static void AncientMummy_Think(Entity *entity, s32 unknown);
static void AncientMummy_ThinkCombat(Entity *entity, s32 unknown);
static s32 func_00155678(Entity *entity, s32 unused, f32 reach);
static void AncientMummy_ShootAndRelocate(Entity *entity);
static void AncientMummy_FadeBackIn(Entity *entity);

/* Set-up of a type 42 entity: clears its work area and installs the type's handlers. */
void AncientMummy_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = AncientMummy_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
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
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with AncientMummy_ThinkCombat for state 2. Counts down work
   word unk68, the pause before the next AncientMummy_ShootAndRelocate. */
static void AncientMummy_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
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
                AncientMummy_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but with the entity within 40 degrees of
   the player's facing it first starts AncientMummy_ShootAndRelocate when the player is nearer than 300
   and the pause is over, or, with the player nearer than 800 and less than 90 degrees off its
   heading, goes backwards for 90 to 149 frames (work byte unk78 is 1) with the command
   func_00155678 gives. Closing in never runs. */
static void AncientMummy_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
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
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    if (entity->unk60 - entity->radius - 100.0f < 300.0f && ABS(angle) < 0.6981317f && unkE0->unk68 == 0) {
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        EntityAi_StartAction(entity, AncientMummy_ShootAndRelocate, 1);
        return;
    }
    if (entity->unk60 - entity->radius - 100.0f < 800.0f && ABS(angle) < 0.6981317f) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
        if (ABS(angle) < 1.5707964f) {
            unkE0->unk78 = 1;
            entity->target = &D_00413F80;
            entity->unkCD = 1;
            entity->unkA0 = rand() % 60 + 90;
        }
    }
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
            turn = EntityAi_TurnFromPush(entity, direction);
            switch (unkE0->unk78) {
            case 0:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x102;
                } else if (turn < 0) {
                    entity->unkC8 = 0x202;
                } else {
                    // match: the caller passes an integer argument the callee does not take
                    entity->unkC8 = func_00155678(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The movement command for walking to the target: a turn bit when the target is off the heading by more than
   the turn limit, and bit 1 while it is farther away than reach. */
static s32 func_00155678(Entity *entity, s32 unused, f32 reach) {
    f32 turn;
    f32 limit;
    f32 heading;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    heading = EntityMath_HeadingTo(entity->model->position, entity->target);
    turn = EntityMath_AngleDifference(entity->model->unkD0[1], heading);
    limit = entity->params->turn_limit;
    if (0.0f <= turn ? limit < turn : limit < -turn) {
        command = 0x100;
        if (0.0f <= turn) {
            command = 0x200;
        }
    }
    if (reach < EntityMath_DistanceXZ(entity->model->position, entity->target)) {
        command |= 2;
    }
    return command;
}

/* An action (busy bit 0x1000): plays animation 23 and launches the shots of ranged record 0 on
   their frames. Meanwhile it looks for a point (EntityPoint_PickNearFromDistance, EntityCollision_DistanceToPropFloor) that is more than 90
   degrees off the player's facing; from frame 35 it shrinks the model in x and z, stretches it in y
   and fades it out over EntityParams.unk438 frames, then puts the entity at the point if it found
   one and starts AncientMummy_FadeBackIn. */
static void AncientMummy_ShootAndRelocate(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 *position;
    f32 *destination;
    f32 height;
    f32 yaw;
    f32 step;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk1C = 0.0f;
        unkE0->unk6C = entity->params->unk438;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            unkE0->unk7A = 0;
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (unkE0->unk7A == 0) {
            position = entity->model->position;
            destination = unkE0->unk30;
            EntityPoint_PickNearFromDistance(destination, position, EntityMath_DistanceXZ(position, (float *)entity->spawn_info), entity->spawn_info->unk4C);
            height = EntityCollision_DistanceToPropFloor(entity->unk14, destination);
            if (height != D_00446810[0]) {
                yaw = EntityMath_AngleXZ(D_00413F80.position, (char *)destination);
                if (EntityMath_WrapAngle(yaw - D_00413F80.unk14) < -1.5707964f || EntityMath_WrapAngle(yaw - D_00413F80.unk14) > 1.5707964f) {
                    unkE0->unk7A = 1;
                    unkE0->unk30[1] += height;
                }
            }
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8, entity->model->unkFC->unk82);
        if (entity->model->unkFC->unk82 >= 0x23) {
            unkE0->unk6C--;
            ENTITY_FLAG_BITS(entity)->bit20 = 1;
            ENTITY_FLAG_BITS(entity)->bit18 = 1;
            ENTITY_FLAG_BITS(entity)->bit21 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            step = 1.3f / entity->params->unk438;
            entity->model->unkE0[0] -= step;
            entity->model->unkE0[1] += step;
            entity->model->unkE0[2] -= step;
            entity->unkD6 -= 0x80 / entity->params->unk438;
        }
        if (unkE0->unk6C < 0) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk6C = (u16)entity->params->unk43A + (u16)entity->params->unk43C;
            entity->unkD6 = 0;
            if (unkE0->unk7A == 1) {
                Vector_Copy(entity->model->position, unkE0->unk30);
            }
            EntityAi_StartAction(entity, AncientMummy_FadeBackIn, 1);
        }
        break;
    }
}

/* An action (busy bit 0x2000) that AncientMummy_ShootAndRelocate starts: the second half of it.
   Waits EntityParams.unk436 frames, turns the entity straight to the player, then over
   EntityParams.unk43A frames brings the model's scale and Entity.unkD6 back (to 1.3 and 0x80),
   waits unk43C frames more, clears the flag bits the first half set, lets the motion end, goes to
   state 2 and thinks, with the pause in work word unk68 set to EntityParams.unk434. */
static void AncientMummy_FadeBackIn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 step;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk6A = 0;
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
        break;
    case 1:
        if (entity->params->unk436 < unkE0->unk6A++) {
            entity->model->unkD0[1] = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            entity->action_step++;
        }
        break;
    case 2:
        unkE0->unk6C--;
        if (unkE0->unk6C < 0) {
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit27 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->action_step++;
        } else if (entity->params->unk43C < unkE0->unk6C) {
            step = 1.3f / entity->params->unk43A;
            entity->model->unkE0[0] += step;
            entity->model->unkE0[1] -= step;
            entity->model->unkE0[2] += step;
            entity->unkD6 += 0x80 / entity->params->unk43A;
        } else if (unkE0->unk6C == entity->params->unk43C) {
            entity->model->unkE0[0] = 1.3f;
            entity->model->unkE0[1] = 1.3f;
            entity->model->unkE0[2] = 1.3f;
            entity->unkD6 = 0x80;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetState((char *)entity, 2);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk68 = entity->params->unk434;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
