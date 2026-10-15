/* DarkSuccubus: the behaviour of entity type 54 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Succubus. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionRewind(Entity *);
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
extern u8 D_0034A018[];
// match: the size is unknown; a complete small type would be read through $gp
extern f32 D_0044681C[];
int rand(void);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
void EntityEvent_StartCue(Entity *, s32, s32, s32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float angle_from, float angle_to);
s32 EntityAi_CanMelee(Entity *);
s32 EntityAi_PickMeleeAttack(Entity *);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_TurnFromPush(void *, void *);
float EntityMath_WrapAngle(float);
s32 EntityAi_IsPushedFromFront(Entity *, f32);
int EntityAi_TurnCommand(Entity *);
int EntityAi_PickAttack(char *);
int Motion_Start(void *, int, int, u32, int);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
// match: the size is unknown (a complete small type would be read through $gp) and it is
// const: the store to unk54 may then be scheduled after the lui of this address
extern const f32 D_00446820[];
f32 EntityMath_Approach(f32, f32, f32);
int func_0019A3F0(void *, int);

static void DarkSuccubus_OnHit(Entity *entity);
static void DarkSuccubus_Animate(Entity *entity);
static void DarkSuccubus_Think(Entity *entity, s32 unknown);
static void DarkSuccubus_ThinkCombat(Entity *entity, s32 unknown);
static void func_00160630(Entity *entity, s32 unknown);
static void func_00160B30(Entity *entity, s32 unknown);
static void DarkSuccubus_Move(Entity *entity);
static void DarkSuccubus_ApplyMoveCommand(Entity *entity);
static void DarkSuccubus_ReactionLight(Entity *entity);
static void DarkSuccubus_ReactionHeavy(Entity *entity);
static void DarkSuccubus_Death(Entity *entity);
static void DarkSuccubus_MeleeAttack(Entity *entity);
static void DarkSuccubus_RangedAttack(Entity *entity);
static void DarkSuccubus_MoveAlongArc(Entity *entity);
static void DarkSuccubus_Dodge(Entity *entity);
static void DarkSuccubus_DodgeAnim29Or30(Entity *entity);
static void DarkSuccubus_PlayAnim18Turning(Entity *entity);
static void DarkSuccubus_FadeOut(Entity *entity);
static s32 DarkSuccubus_ShouldDodgeAnim29Or30(Entity *entity, f32 reach, f32 angle_limit);
static void DarkSuccubus_CountLiveSpawns(Entity *entity);

/* Set-up of a type 54 entity: clears its work area and installs the type's handlers. */
void DarkSuccubus_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = DarkSuccubus_Think;
    unkE0->animate = (void (*)(void))DarkSuccubus_Animate;
    unkE0->on_hit = (void (*)(void))DarkSuccubus_OnHit;
    unkE0->move = (void (*)(void))DarkSuccubus_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))DarkSuccubus_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags |= 0x10000000;
    if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
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
    // match: called as returning an int, which keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))Motion_StepModel)(entity->model, 0, 0);
    entity->flags |= 0x40000;
}

/* The on_hit handler: as EntityAi_OnHit with the type's own reactions to a light and a heavy hit;
   it keeps the target, clears work word unk68, sets work word unk6A to EntityParams.unk438 and
   Entity.unkD6 to 0x80. */
static void DarkSuccubus_OnHit(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))DarkSuccubus_ReactionLight, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))DarkSuccubus_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
    unkE0->unk68 = 0;
    unkE0->unk6A = entity->params->unk438;
    entity->unkD6 = 0x80;
}

/* The animate handler: as EntityAi_Animate, after DarkSuccubus_ApplyMoveCommand has carried out the
   movement command. */
static void DarkSuccubus_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    DarkSuccubus_ApplyMoveCommand(entity);
    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, -1, NULL, 8.0f);
        }
    }
}

/* The think handler: as EntityAi_Think, with three functions for state 2, picked by work byte unk7C
   (DarkSuccubus_CountLiveSpawns sets it): DarkSuccubus_ThinkCombat at 0, func_00160630 at 1,
   func_00160B30 above. While D_0034A018[0] is 0 it does nothing but set model flag 0x800, flag bits
   18 and 21 and Entity.unkD6 to 0x80. With the player's x at 6600 or more it goes back to its spawn
   point instead of thinking. It adds command bit 0x10 or 0x20 by what EntityCollision_DistanceToPropFloor reads at the
   entity's position (work byte unk7A). While work word unk68 counts down, model flag 0x800 and flag
   bit 21 are set and Entity.unkD6 is 0x80; work word unk6A is the pause after it. With unk7C at 0
   the handler starts that time itself now and then, taking unkD6 down over 20 frames first. Work
   word unk6C is put to EntityParams.unk43A while D_00413F80.unk259 is set and counts down. */
static void DarkSuccubus_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind15F8C0 *timers = (EntityUnkE0Kind15F8C0 *)unkE0;
    f32 height;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (D_0034A018[0] == 0) {
        entity->model->unk102 |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1E;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        ENTITY_FLAGS(entity) |= 0x40000;
        return;
    }
    ENTITY_FLAGS(entity) &= ~0x40000;
    if (timers->unk6C > 0) {
        timers->unk6C--;
    }
    if (D_00413F80.unk259 != 0) {
        timers->unk6C = entity->params->unk43A;
    }
    DarkSuccubus_CountLiveSpawns(entity);
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (D_00413F80.position[0] >= 6600.0f) {
            ENTITY_FLAGS(entity) |= 0x40000;
            ENTITY_FLAGS(entity) &= ~0x3800;
            entity->target = entity->spawn_info;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 500.0f) | 0x440;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        } else if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
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
                if (timers->unk7C == 0) {
                    DarkSuccubus_ThinkCombat(entity, unknown);
                } else if (timers->unk7C == 1) {
                    func_00160630(entity, unknown);
                } else {
                    func_00160B30(entity, unknown);
                }
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (((s32)(ENTITY_FLAGS(entity) >> 28) & 1) && !(entity->unkC6 & 0x1000)) {
        height = EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position);
        if (height != D_0044681C[0]) {
            if (height > 45.0f) {
                timers->unk7A = 1;
            } else if (height < 35.0f) {
                timers->unk7A = 0;
                timers->unk7B = 1;
            }
            if (timers->unk7A != 0) {
                entity->unkC8 |= 0x20;
            } else {
                entity->unkC8 |= 0x10;
            }
        }
    }
    if (timers->unk7C == 0) {
        if (timers->unk68 == 0 && timers->unk6A == 0 && timers->unk7D == 0 && rand() % 100 < 60) {
            timers->unk7D = 1;
            timers->unk7E = 20;
        }
        if (timers->unk7D != 0) {
            if (timers->unk7E != 0) {
                entity->unkD6 = (timers->unk7E << 7) / 20;
                timers->unk7E--;
            } else {
                timers->unk68 = entity->params->unk434;
                timers->unk7D = 0;
                EntityEvent_StartCue(entity, 7, 0, 0);
            }
        }
    }
    if (timers->unk68 != 0) {
        entity->model->unk102 |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1E;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        if (--timers->unk68 <= 0) {
            timers->unk6A = entity->params->unk438;
        }
    } else {
        entity->model->unk102 &= ~0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1F;
        ENTITY_FLAGS(entity) &= ~0x200000;
        if (timers->unk6A != 0) {
            timers->unk6A--;
        }
    }
}

/* The think function of state 2 while work byte unk7C is 0 (func_00160630 and func_00160B30 are the
   other two): starts DarkSuccubus_DodgeAnim29Or30 when DarkSuccubus_ShouldDodgeAnim29Or30 says so and DarkSuccubus_Dodge for
   the player's swing; starts DarkSuccubus_MeleeAttack when EntityAi_CanMelee finds a melee record in
   reach (EntityAi_PickMeleeAttack picks it); else, each time the timer runs out, starts
   DarkSuccubus_MoveAlongArc with the player nearer than 350 and Entity.unk68 within 40 degrees of
   the player's facing, or goes sideways round the player for 30 frames (work byte unk78: 2 or 3 the
   side, 1 backwards, 0 towards, 4 turning only). */
static void DarkSuccubus_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    entity->flags &= ~0x3800;
    if (DarkSuccubus_ShouldDodgeAnim29Or30(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, DarkSuccubus_DodgeAnim29Or30, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, DarkSuccubus_Dodge, 1);
        return;
    }
    entity->flags |= 0x200;
    EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    unkE0->unk5C = 100.0f;
    unkE0->unk60 = 250.0f;
    if (entity->unkA2 == 0 && EntityAi_CanMelee(entity)) {
        EntityAi_PickMeleeAttack(entity);
        EntityAi_StartAction(entity, DarkSuccubus_MeleeAttack, unknown);
        return;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    if (entity->unkA0 == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        if (entity->unkCD == 1) {
            entity->target = &D_00413F80;
            angle = EntityMath_AngleDifference(D_00413F80.unk14, entity->unk68);
            if (entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                EntityAi_StartAction(entity, DarkSuccubus_MoveAlongArc, unknown);
                return;
            }
            if (EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = 30;
        } else {
            unkE0->unk78 = -1;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->unkC8 |= 0x201;
        } else if (side < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
        }
        break;
    case 1:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f))) {
            unkE0->unk78 = rand() % 3 + 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 2;
        }
        break;
    case 2:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] - 1.5707964f))) {
            unkE0->unk78 = 3;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 4;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 3:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] + 1.5707964f))) {
            unkE0->unk78 = 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 8;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 4:
        entity->unkC8 |= EntityAi_TurnCommand(entity);
        break;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

static void func_00160630(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind15F8C0 *timers = (EntityUnkE0Kind15F8C0 *)unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;
    s32 may_strike;

    entity->flags &= ~0x3800;
    if (DarkSuccubus_ShouldDodgeAnim29Or30(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, DarkSuccubus_DodgeAnim29Or30, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, DarkSuccubus_Dodge, 1);
        return;
    }
    entity->flags |= 0x200;
    EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    may_strike = TRUE;
    unkE0->unk5C = 100.0f;
    unkE0->unk60 = 250.0f;
    mode = EntityAi_PickCombatChoice(entity, 0);
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, DarkSuccubus_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, DarkSuccubus_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            angle = EntityMath_AngleDifference(D_00413F80.unk14, entity->unk68);
            if (may_strike && entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                EntityAi_StartAction(entity, DarkSuccubus_MoveAlongArc, unknown);
                return;
            }
            if (timers->unk68 == 0 && timers->unk6A == 0 && rand() % 100 < 60) {
                EntityAi_StartAction(entity, DarkSuccubus_FadeOut, unknown);
                return;
            }
            if (EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = 30;
            break;
        case 2:
            entity->target = &D_00413F80;
            entity->unkA0 = 30;
            unkE0->unk78 = 4;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->unkC8 |= 0x201;
        } else if (side < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
        }
        break;
    case 1:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f))) {
            unkE0->unk78 = rand() % 3 + 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 2;
        }
        break;
    case 2:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] - 1.5707964f))) {
            unkE0->unk78 = 3;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 4;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 3:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] + 1.5707964f))) {
            unkE0->unk78 = 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 8;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 4:
        entity->unkC8 |= EntityAi_TurnCommand(entity);
        break;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

static void func_00160B30(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind15F8C0 *timers = (EntityUnkE0Kind15F8C0 *)unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;
    s32 may_strike;

    entity->flags &= ~0x3800;
    if (DarkSuccubus_ShouldDodgeAnim29Or30(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, DarkSuccubus_DodgeAnim29Or30, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, DarkSuccubus_Dodge, 1);
        return;
    }
    entity->flags |= 0x200;
    angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    if (ABS(angle) > 0.87266463f || unkE0->unk68 != 0) {
        unkE0->unk5C = 100.0f;
        unkE0->unk60 = 250.0f;
        may_strike = FALSE;
    } else {
        unkE0->unk5C = 500.0f;
        unkE0->unk60 = 800.0f;
        may_strike = TRUE;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, DarkSuccubus_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, DarkSuccubus_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            angle = EntityMath_AngleDifference(D_00413F80.unk14, entity->unk68);
            if (may_strike && entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                EntityAi_StartAction(entity, DarkSuccubus_MoveAlongArc, unknown);
                return;
            }
            if (entity->distance_to_player > unkE0->unk60) {
                unkE0->unk78 = 0;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                unkE0->unk78 = 1;
            } else {
                if (timers->unk68 == 0 && timers->unk6A == 0 && rand() % 100 < 60) {
                    EntityAi_StartAction(entity, DarkSuccubus_FadeOut, unknown);
                    return;
                }
                if (EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                    unkE0->unk78 = 2;
                } else {
                    unkE0->unk78 = 3;
                }
            }
            entity->unkA0 = 30;
            break;
        case 2:
            entity->target = &D_00413F80;
            entity->unkA0 = 30;
            unkE0->unk78 = 4;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->unkC8 |= 0x201;
        } else if (side < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
        }
        break;
    case 1:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f))) {
            unkE0->unk78 = rand() % 3 + 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 2;
        }
        break;
    case 2:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] - 1.5707964f))) {
            unkE0->unk78 = 3;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 4;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 3:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->unkD0[1] + 1.5707964f))) {
            unkE0->unk78 = 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 8;
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 4:
        entity->unkC8 |= EntityAi_TurnCommand(entity);
        break;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: only picks the animation (6 or 7 sideways, 3 forwards, 5 backwards, else 0);
   DarkSuccubus_ApplyMoveCommand moves the entity. */
static void DarkSuccubus_Move(Entity *entity) {
    if (entity == D_00413F80.unk204) {
        if (entity->params->flags & 0x800) {
            entity->unkC8 = 0;
        }
    }
    if (entity->unkC8 & 4) {
        Motion_Start(entity, 6, 0, entity->params->unkC4, 0);
        return;
    }
    if (entity->unkC8 & 8) {
        Motion_Start(entity, 7, 0, entity->params->unkC4, 0);
        return;
    }
    if (entity->unkC8 & 1) {
        Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        return;
    }
    if (entity->unkC8 & 2) {
        Motion_Start(entity, 5, 0, entity->params->unkC4, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
}

/* Helper of DarkSuccubus_Animate: carries out the command in Entity.unkC8. It turns the entity and
   speeds up or slows down along its three axes by EntityParams.unk50 a frame up to unk54 (work
   floats unk44, unk48 and unk4C; in y half the step and half the limit), twice as fast for command
   bit 0x40. */
static void DarkSuccubus_ApplyMoveCommand(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;
    f32 step;
    f32 limit;

    if (entity == D_00413F80.unk204) {
        entity->unkC8 = 0;
    }
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
    limit = entity->params->unk54 * 0.5f;
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

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, but the motion never moves
   the entity and the turn rate is kept. */
static void DarkSuccubus_ReactionLight(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        Motion_Start(entity, 0xE, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags &= ~0x80000;
        entity->flags &= ~0x800000000L;
        entity->flags &= ~0x200;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        EntityAi_SetAttackDelay(entity, entity->params->unkC8);
        EntityAi_SetReaction((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if ((s32)(entity->flags >> 5) & 1) {
            unkE0->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, but the motion never moves
   the entity and the turn rate is kept. */
static void DarkSuccubus_ReactionHeavy(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        Motion_Start(entity, 0xF, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags &= ~0x80000;
        entity->flags &= ~0x800000000L;
        entity->flags &= ~0x200;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            EntityAi_SetAttackDelay(entity, entity->params->unkC8);
            EntityAi_SetReaction((char *)entity, NULL, 0);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death with other flag settings; it first clears
   model flag 0x800 and work word unk68 and puts Entity.unkD6 back to 0x80. */
static void DarkSuccubus_Death(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        entity->model->unk102 &= ~0x800;
        unkE0->unk68 = 0;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1F;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x4000;
        ENTITY_FLAGS(entity) |= 0x100000;
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 4;
        ENTITY_FLAGS(entity) |= 0x8000000;
        ENTITY_FLAGS(entity) |= 0x4000000;
        ENTITY_FLAGS(entity) &= ~0x10000000;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 8;
            ENTITY_FLAGS(entity) |= 0x10;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAGS(entity) |= 0xC;
            ENTITY_FLAGS(entity) &= ~2;
        }
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, but it blends in faster and, where that
   one turns the entity to the player, adds the command of EntityAi_TurnCommand. */
static void DarkSuccubus_MeleeAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 2, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            entity->unkC8 |= EntityAi_TurnCommand(entity);
        }
        return;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= EntityAi_TurnCommand(entity);
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but it blends in faster, adds the
   command of EntityAi_TurnCommand where that one turns the entity, and aims its shots at a point 190 below
   the player's position, within 45 and 10 degrees (EntityShot_LaunchAimed). */
static void DarkSuccubus_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 2, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAGS(entity) &= ~0x3800;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            entity->unkC8 |= EntityAi_TurnCommand(entity);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= EntityAi_TurnCommand(entity);
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(target, D_00413F80.position);
        target[1] -= 190.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.7853982f, 0.17453292f);
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

/* An action (busy bit 0x1000) that moves the entity itself in an arc: forwards along the heading it
   starts with at up to twice EntityParams.unk54, and in y. In animation 8 its speed in y goes to
   -20 (up: EntityCollision_DistanceToPropFloor, the distance to the floor below, grows) until EntityCollision_DistanceToPropFloor reads 400 or
   more (or nothing), then in animation 4 the speed in y goes to 20, turning to the player, until
   flag bit 44 or 50 is set, then animation 9 to its end. */
static void DarkSuccubus_MoveAlongArc(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;
    s32 run = FALSE;

    entity->unkC6 |= 0x1000;
    entity->flags &= ~0x3800;
    entity->flags &= ~0x200;
    height = EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->unkC4, 1);
        unkE0->unk50 = EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f);
        unkE0->unk54 = unkE0->unk48;
        unkE0->unk58 = 0.0f;
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
        unkE0->unk4C = 0.0f;
        unkE0->unk1C = 0.0f;
        entity->action_step++;
    case 1:
        Motion_Start(entity, 8, 0, entity->params->unkC4, 0);
        unkE0->unk58 = EntityMath_Approach(unkE0->unk58, entity->params->unk54, entity->params->unk50);
        unkE0->unk54 = EntityMath_Approach(unkE0->unk54, -20.0f, 2.0f);
        if (height == D_00446820[0] || height >= 400.0f) {
            entity->action_step++;
        }
        break;
    case 2:
        Motion_Start(entity, 4, 0, entity->params->unkC4, 0);
        unkE0->unk58 = EntityMath_Approach(unkE0->unk58, entity->params->unk54, entity->params->unk50);
        unkE0->unk54 = EntityMath_Approach(unkE0->unk54, 20.0f, 2.0f);
        if (unkE0->unk54 > 0.0f) {
            entity->unkC8 |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->unkC8 |= 0x400;
        }
        if (entity->flags & (0x8200L << 35)) {
            Motion_Start(entity, 9, 0, 0, 1);
            unkE0->unk58 = unkE0->unk54 = 0.0f;
            entity->action_step++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
    ENTITY_SCRATCH_VECTOR[1] = unkE0->unk54;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk58 + unkE0->unk58;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, D_00431AF0, unkE0->unk50);
    Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    // match: never taken, but until the compiler works that out the branch separates the
    // scratchpad stores above from the loads below, so their addresses are not kept in registers
    if (run) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
    entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
    entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
}

/* The dodge action (busy bit 8): as EntityAi_Dodge; when it ends it starts
   DarkSuccubus_PlayAnim18Turning one time in two. The think functions start it for the player's swing
   (EntityAi_ShouldDodge). */
static void DarkSuccubus_Dodge(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    entity->flags &= ~0x200;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xA, 0, 5, 1);
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (rand() % 100 < 50) {
                EntityAi_StartAction(entity, DarkSuccubus_PlayAnim18Turning, 1);
            } else if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* A second dodge action (busy bit 8): plays animation 29 or 30, by the side of the player's facing
   the entity is on, to its end. The think functions start it when
   DarkSuccubus_ShouldDodgeAnim29Or30 says so, which is not a test of the player's swing. */
static void DarkSuccubus_DodgeAnim29Or30(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    entity->flags &= ~0x200;
    switch (entity->action_step) {
    case 0:
        if (EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
            Motion_Start(entity, 0x1D, 0, 5, 1);
        } else {
            Motion_Start(entity, 0x1E, 0, 5, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x2000): plays animation 18 to its end with flag bit 9 set and the command of
   EntityAi_TurnCommand. DarkSuccubus_Dodge starts it. */
static void DarkSuccubus_PlayAnim18Turning(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    entity->flags |= 0x200;
    entity->unkC8 |= EntityAi_TurnCommand(entity);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x12, 0, entity->params->unkC4 >> 2, 0);
        entity->flags &= ~0x3800;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x4000) that func_00160630 and func_00160B30 start: plays animation 17 to its
   end with the turn command of EntityAi_TurnCommand; up to frame EntityParams.unk436 it takes Entity.unkD6
   from 0x80 down to 0 (unkD6 is what EntityAi_Death's fade scales), after that it keeps work word
   unk68 at EntityParams.unk434. */
static void DarkSuccubus_FadeOut(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x4000;
    entity->unkC8 |= EntityAi_TurnCommand(entity);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4 >> 2, 0);
        entity->flags &= ~0x3800;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->params->unk436 >= entity->model->unkFC->unk82) {
            func_0019A3F0(entity, 0x11);
            entity->unkD6 = ((entity->params->unk436 - entity->model->unkFC->unk82) << 7) / entity->params->unk436;
        } else {
            unkE0->unk68 = entity->params->unk434;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* Whether to start DarkSuccubus_DodgeAnim29Or30 (the three think functions of state 2 ask): only
   while the counter in work word unk6C runs, the entity is not busy, the player is within reach
   (less the radius and 100) and Entity.unk68 within angle_limit of the heading; then
   EntityParams.unkCA / 5 times in 100. */
static s32 DarkSuccubus_ShouldDodgeAnim29Or30(Entity *entity, f32 reach, f32 angle_limit) {
    f32 distance;
    f32 turn;

    if (entity->unkE0.unk6C <= 0) {
        return 0;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    turn = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return 0;
    }
    if (reach < distance) {
        return 0;
    }
    if (angle_limit < (0.0f <= turn ? turn : -turn)) {
        return 0;
    }
    return rand() % 100 < (s32)(entity->params->unkCA / 5U);
}

/* Helper of DarkSuccubus_Think: counts those of the entities with spawn ids 0x65 to 0x6A whose
   flags' low nibble is 3 and stores (count + 2) / 3 in work byte unk7C, which picks the think
   function of state 2. */
static void DarkSuccubus_CountLiveSpawns(Entity *entity) {
    EntityUnkE0 *sub;
    Entity *object;
    int count;
    int next;
    int i;

    sub = &entity->unkE0;
    count = 0;
    i = 0;
    do {
        object = Entity_GetBySpawnId(i + 0x65);
        if (object != NULL) {
            next = count + 1;
            count = ((object->flags & 0xF) ^ 3) ? count : next;
        }
        i += 1;
    } while (i < 6);
    sub->unk7C = (char)((count + 2) / 3);
}
