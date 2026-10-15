/* ExpeditionArcher: the behaviour of entity type 13 (Entity.params->unk02, the index into D_003E6E40): its
   set-up function, which the table names, and the handlers it installs. Used by: Expedition Archer. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
double atan2(double, double);
float EntityMath_WrapAngle(float);
float EntityMath_EaseAngleToward(float *, float, float);
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
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);
void EntityEvent_Respawn(char *);

static void ExpeditionArcher_Animate(Entity *entity);
static void ExpeditionArcher_PitchPartToPlayer(Entity *actor);
static void ExpeditionArcher_Think(Entity *entity, s32 unknown);
static void ExpeditionArcher_ThinkCombat(Entity *entity, s32 unknown);
static void ExpeditionArcher_RangedAttack(Entity *entity);
static void ExpeditionArcher_RangedAttack1(Entity *entity);
static void ExpeditionArcher_Death(Entity *entity);

/* Set-up of a type 13 entity: clears its work area and installs the type's handlers. */
void ExpeditionArcher_Init(Entity *entity) {
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
    unkE0->think = ExpeditionArcher_Think;
    unkE0->animate = (void (*)(void))ExpeditionArcher_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))ExpeditionArcher_Death;
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

/* The animate handler: as EntityAi_Animate; while flag bit 10 is set ExpeditionArcher_PitchPartToPlayer
   pitches a model part, and otherwise work float unk48 is cleared. */
static void ExpeditionArcher_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    s64 flags_now;
    EntityUnkE0 *unkE0;
    s32 look_at_target;
    EntityParams *params;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if ((s32)((s64)(flags << 18) >> 32) & 1) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 2) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
            flags = entity->flags;
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
            flags = entity->flags;
        }
    }
    if ((s32)((s64)(flags << 22) >> 32) & 1) {
        ExpeditionArcher_PitchPartToPlayer(entity);
    } else {
        unkE0->unk48 = 0.0f;
    }
    flags_now = entity->flags;
    if ((s32)((s64)(flags_now << 18) >> 32) & 1) {
        params = entity->params;
        look_at_target = (s32)(flags_now >> 9) & 1;
        limits[1] = params->unk430 * 0.017453292f;
        limits[0] = -params->unk430 * 0.017453292f;
        limits[3] = params->unk432 * 0.017453292f;
        limits[2] = -params->unk432 * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

/* Helper of ExpeditionArcher_Animate: pitches the model part EntityParams.unk434 names towards the
   player, at most 50 degrees either way (work float unk48, rate 10). */
static void ExpeditionArcher_PitchPartToPlayer(Entity *actor) {
    ModelInstance *model = ENTITY_MODEL(actor);
    ModelPart *part = &model->parts[ENTITY_CONTROLLER(actor)->unk434];
    EntityUnkE0 *sub = &actor->unkE0;
    float angle;

    if (part != NULL) {
        angle = EntityMath_WrapAngle(atan2(D_00413F80.position[1] - model->position.y, actor->distance_to_player));
        if (0.87266463f < angle) {
            angle = 0.87266463f;
        } else if (angle < -0.87266463f) {
            angle = -0.87266463f;
        }
        EntityMath_EaseAngleToward(&sub->unk48, angle, 10.0f);
        part->unk50 = sub->unk48;
    }
}

/* The think handler: as EntityAi_Think, with ExpeditionArcher_ThinkCombat for state 2. Before that it
   clears bit 40 of model part 5's unk68 during frames 9 to 46 of animations 23 and 24 and sets it
   otherwise. */
static void ExpeditionArcher_Think(Entity *entity, s32 unknown) {
    if (((u8)entity->unkD1 == 0x17 || (u8)entity->unkD1 == 0x18) && entity->model->unkFC->unk88 == 0 &&
        entity->model->unkFC->unk82 >= 9 && entity->model->unkFC->unk82 < 0x2F) {
        entity->model->unkF8[5].unk68 &= ~(0x8000L << 25);
    } else {
        entity->model->unkF8[5].unk68 |= 0x8000L << 25;
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
        ExpeditionArcher_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but ranged record 1 is
   ExpeditionArcher_RangedAttack1 and the other ranged records ExpeditionArcher_RangedAttack; closing in
   never runs. */
static void ExpeditionArcher_ThinkCombat(Entity *entity, s32 unknown) {
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
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                    EntityAi_StartAction(entity, ExpeditionArcher_RangedAttack1, unknown);
                } else {
                    EntityAi_StartAction(entity, ExpeditionArcher_RangedAttack, unknown);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 0x78;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 0x1E;
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but it sets flag bit 10 and aims its
   shots at a point 170 below the player's position, within 10 degrees (EntityShot_LaunchAimed).
   ExpeditionArcher_ThinkCombat starts it for records 0 and 2. */
static void ExpeditionArcher_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
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
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(target, D_00413F80.position);
        target[1] -= 170.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.17453294f, 0.17453294f);
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

/* The ranged attack with record 1 (busy bit 1): it takes the record from flag bits 58-59, but
   ExpeditionArcher_ThinkCombat starts it for record 1 only. It turns to the player with flag bit 10
   set; at frame 42 it stops the motion until Entity.unk68 no longer changes, the player is nearer
   than 200 or 90 frames have gone by, then plays on and launches the shots at a point 170 below the
   player's position, within 10 degrees (EntityShot_LaunchAimed). */
static void ExpeditionArcher_RangedAttack1(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->bits58);
    f32 target[4];
    f32 angle;
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        EntityAi_TurnToHeading(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if (entity->model->unkFC->unk82 >= 0x2A) {
            entity->unkA0 = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        EntityAi_TurnToHeading(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
        angle = EntityMath_AngleDifference(unkE0->unk4C, entity->unk68);
        if (entity->unkA0 == 0 || entity->distance_to_player < 200.0f || ABS(angle) <= 0.010471975f) {
            entity->action_step++;
        }
        break;
    case 4:
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(target, D_00413F80.position);
        target[1] -= 170.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.17453294f, 0.17453294f);
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
    unkE0->unk4C = entity->unk68;
}

/* The death action (busy bit 0x40): as EntityAi_Death; when the fade starts and
   EntitySpawnInfo.unk40 is set, it calls EntityEvent_Respawn for the entity with that spawn id (if its
   flag bit 0 is set) and puts it at this entity's position. */
static void ExpeditionArcher_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
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
        if (entity->model->unkFC->unk88 == 0) {
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
            if (entity->spawn_info->unk40 != 0) {
                Entity *partner = Entity_GetBySpawnId(entity->spawn_info->unk40);

                if (partner != NULL && ((s32)partner->flags & 1)) {
                    EntityEvent_Respawn((char *)partner);
                    Vector_Copy(partner->model->position, entity->model->position);
                }
            }
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
