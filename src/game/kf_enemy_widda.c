/* Widda: the behaviour of entity type 19 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Widda Large, Widda. The game names
   no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records
   by hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern PlayerData D_00413F80;
float atan2f(float, float);
float EntityMath_EaseAngleToward(float *, float, float);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
float EntityMath_AngleXZ(float *, char *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
extern int D_00414184[];
int Motion_Start(void *, int, int, u32, int);

static void Widda_Animate(Entity *entity);
static void Widda_AimPartsAtTarget(Entity *entity);
static void Widda_Think(Entity *entity, s32 unknown);
static void Widda_ThinkCombat(Entity *entity, s32 unknown);
static void Widda_Move(Entity *entity);

/* Set-up of a type 19 entity: clears its work area and installs the type's handlers. */
void Widda_Init(Entity *entity) {
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
    unkE0->think = Widda_Think;
    unkE0->animate = (void (*)(void))Widda_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))Widda_Move;
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
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate, and Widda_AimPartsAtTarget turns two model parts.
   */
static void Widda_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
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
        Widda_AimPartsAtTarget(entity);
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

/* Helper of Widda_Animate: turns the two model parts EntityParams.unk436 and unk438 name
   towards the target, pitch from the player's height (at most 30 degrees) and yaw from the bearing
   (at most 40), each part by half of it; the angles are work floats unk48 to unk54. Without a
   target or while the entity is busy it only reads the parts' angles into them. */
static void Widda_AimPartsAtTarget(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->unkF8;
    s32 first = entity->params->unk436;
    s32 second = entity->params->unk438;
    f32 angle;
    f32 bearing;
    f32 limit;

    if (entity->target == NULL || entity->unkC6 != 0) {
        unkE0->unk48 = parts[first].unk50[0];
        unkE0->unk4C = parts[first].unk50[1];
        unkE0->unk50 = parts[second].unk50[0];
        unkE0->unk54 = parts[second].unk50[1];
    } else {
        angle = atan2f(D_00413F80.position[1] - entity->model->position[1], entity->distance_to_player);
        limit = 0.5235988f;
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        EntityMath_EaseAngleToward(&unkE0->unk48, angle * 0.5f, 10.0f);
        EntityMath_EaseAngleToward(&unkE0->unk50, angle * 0.5f, 10.0f);
        parts[first].unk50[0] = unkE0->unk48;
        parts[second].unk50[0] = unkE0->unk50;
        bearing = EntityMath_HeadingTo(entity->model->position, entity->target);
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], bearing);
        limit = 0.6981317f;
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        EntityMath_EaseAngleToward(&unkE0->unk4C, angle * 0.5f, 10.0f);
        EntityMath_EaseAngleToward(&unkE0->unk54, angle * 0.5f, 10.0f);
        parts[first].unk50[1] = unkE0->unk4C;
        parts[second].unk50[1] = unkE0->unk54;
    }
}

/* The think handler: as EntityAi_Think, with Widda_ThinkCombat for state 2. Sets work byte
   unk7A when flag bit 45, 47 or 48 is set during a dodge (busy bit 8). */
static void Widda_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if ((entity->unkC6 & 8) && (entity->flags & (0xD000L << 33))) {
        unkE0->unk7A = 1;
    }
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
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        Widda_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but closing in runs (work byte unk78 is
   1) once the player is farther than 900 and within 40 degrees ahead. Every tenth frame, with the
   player near and the entity within 40 degrees of the player's facing, a chance of
   EntityParams.unk434 in 100 makes it start EntityAi_Dodge when the player is 40 to 90 degrees off
   its heading, or close in at the run when the player is farther off or work byte unk7A is set. */
static void Widda_ThinkCombat(Entity *entity, s32 unknown) {
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
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.6981317f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
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
            entity->target = &D_00413F80;
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (entity->distance_to_player > 900.0f && ABS(angle) < 0.6981317f) {
                entity->unkA0 = 300;
                unkE0->unk78 = 1;
            } else {
                entity->unkA0 = rand() % 60 + 120;
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
    angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    if (entity->unk60 - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f
        && D_00446ED4 % 10U == entity->unkD7 % 10U && rand() % 100 < ENTITY_CONTROLLER(entity)->unk434) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
        if (unkE0->unk7A == 0) {
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                EntityAi_StartAction(entity, EntityAi_Dodge, 1);
                unkE0->unk7A = 0;
                return;
            }
            if (!(ABS(angle) >= 1.5707964f)) {
                goto skip;
            }
        }
        entity->unkCF = entity->unkCD;
        entity->target = &D_00413F80;
        entity->unkA0 = 300;
        unkE0->unk78 = entity->unkCD = 1;
        unkE0->unk7A = 0;
    }
skip:
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
                    entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
                if (entity->distance_to_player > 900.0f && ABS(angle) < 0.6981317f) {
                    entity->unkA0 = 300;
                    unkE0->unk78 = 1;
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x241;
                } else if (turn < 0) {
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

/* The move handler: as EntityAi_Move, but it only turns the entity (twice as fast for command bits
   0x40 and 0x400) and picks the animation; it does not move it. */
static void Widda_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    if (entity == (Entity *)D_00414184[0] && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (unkE0->unk1C > entity->params->turn_limit) {
            unkE0->unk1C = entity->params->turn_limit;
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
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
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
