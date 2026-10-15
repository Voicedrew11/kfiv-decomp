/* DarkArcher: the behaviour of entity type 55 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Archer. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_map.h"
#include "kf_effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
void EntityAi_Move(Entity *);
// match: read as pointers, so that the int store to target is not taken to alias the table
extern void *D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern PlayerData D_00413F80;
extern f32 D_00431AF0[16];
double atan2(double, double);
float EntityMath_WrapAngle(float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0TransposeMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
s32 Bone_MultiplyPathTo(void *, void *, void *);
void Bone_MatrixToAngles(void *, void *);
void Matrix_Multiply(void *, void *, void *);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
extern u32 D_00446ED4;
int EntityAi_TurnCommand(Entity *);
int EntityAi_StartAttack(Entity *, void (*)(Entity *entity), int, int, int);
int EntityAi_CanUseRanged(Entity *, int);
void EntityAi_SensePlayer(void *, int);
int Motion_Start(void *, int, int, u32, int);
// match: the size is unknown; a complete small type would be read through $gp
extern u8 D_0034A018[];
extern MapSlot D_0038CD20[];
extern EntitySpawnInfo *D_004467D0[2];
void func_001035A8(u32, int);
void EntityAi_CheckFarFromSpawn(void *);
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
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityAi_SpawnBoneEffect(Entity *, s32, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
extern const f32 D_0044682C[];
f32 EntityCollision_DistanceToPropFloor(void *, void *);
f32 EntityMath_Approach(f32, f32, f32);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);

static void DarkArcher_Animate(Entity *entity);
static void DarkArcher_PitchPart2ToPlayer(Entity *entity, s32 mode);
static void DarkArcher_OnHit(Entity *entity);
static void DarkArcher_ThinkShootOrTurn(Entity *entity, s32 unknown);
static void DarkArcher_MoveTurnOnly(Entity *entity);
static void DarkArcher_DormantFadeIn(Entity *entity);
static void func_0016B228(Entity *entity);
static void DarkArcher_Think(Entity *entity, s32 unknown);
static void DarkArcher_ThinkCombat(Entity *entity, s32 unknown);
static void DarkArcher_RangedAttack(Entity *entity);
static void DarkArcher_MoveAlongArc(Entity *entity);

/* Set-up of a type 55 entity: clears its work area and installs the type's handlers. */
void DarkArcher_Init(Entity *entity) {
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
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->flags &= ~0x10000000;
    *(s32 *)&entity->target = 0; // match: stored as an int, so it can move ahead of the params load
    EntityAi_SetUpdate((char *)entity, (s32)D_003E6E40[entity->params->unk02][1]);
    if (entity->spawn_info->unk40 != 0) {
        unkE0->think = DarkArcher_ThinkShootOrTurn;
        unkE0->animate = (void (*)(void))DarkArcher_Animate;
        unkE0->on_hit = (void (*)(void))DarkArcher_OnHit;
        unkE0->dormant = (void (*)(void))DarkArcher_DormantFadeIn;
        unkE0->move = (void (*)(void))DarkArcher_MoveTurnOnly;
        unkE0->death = (void (*)(void))EntityAi_Death;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        unkE0->think = DarkArcher_Think;
        unkE0->animate = (void (*)(void))DarkArcher_Animate;
        unkE0->on_hit = (void (*)(void))DarkArcher_OnHit;
        unkE0->death = (void (*)(void))EntityAi_Death;
        unkE0->dormant = (void (*)(void))EntityAi_Dormant;
        unkE0->move = (void (*)(void))EntityAi_Move;
        if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
        }
        entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
        entity->model->unk15C = entity->unkD6;
    }
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler (installed in both modes): as EntityAi_Animate, and
   DarkArcher_PitchPart2ToPlayer turns model part 2, with flag bit 10 as its mode. */
static void DarkArcher_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityParams *params;
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
        DarkArcher_PitchPart2ToPlayer(entity, (s32)((s64)(entity->flags << 22) >> 32) & 1);
        params = entity->params;
        limits[0] = -params->unk430 * 0.017453292f;
        limits[1] = params->unk430 * 0.017453292f;
        limits[3] = params->unk432 * 0.017453292f;
        limits[2] = -params->unk432 * 0.017453292f;
        if ((s32)(entity->flags >> 9) & 1) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, (s32)(entity->flags >> 9) & 1, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, -1, NULL, 8.0f);
        }
    }
}

/* Helper of DarkArcher_Animate: with mode above 0 tilts model part 2 (EntityModel.unkF8[2]) by
   the pitch from the entity to the player, at most 60 degrees up or down, easing the part's three
   angles in work floats unk48 to unk50; with mode 0 eases them back to the part's own angles; else
   copies the part's angles into them. */
static void DarkArcher_PitchPart2ToPlayer(Entity *entity, s32 mode) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelUnkF8 *bone = &entity->model->unkF8[2];
    f32 parent[16];
    f32 rotation[16];
    f32 tilt[16];
    f32 angles[4];
    f32 pitch;
    f32 rate;

    if (bone != NULL) {
        pitch = EntityMath_WrapAngle(atan2(D_00413F80.position[1] - entity->model->position[1], entity->distance_to_player));
        if (pitch > 1.0471976f) {
            pitch = 1.0471976f;
        } else if (pitch < -1.0471976f) {
            pitch = -1.0471976f;
        }
        if (mode > 0) {
            rate = 10.0f;
            sceVu0CopyMatrix(parent, D_00431AF0);
            Bone_MultiplyPathTo(entity->model->unkF8, bone, parent);
            parent[12] = 0.0f;
            parent[13] = 0.0f;
            parent[14] = 0.0f;
            parent[15] = 1.0f;
            sceVu0RotMatrixX(rotation, D_00431AF0, bone->unk50[0]);
            sceVu0RotMatrixZ(rotation, rotation, bone->unk50[2]);
            sceVu0RotMatrixY(rotation, rotation, bone->unk50[1]);
            Matrix_Multiply(rotation, parent, rotation);
            sceVu0RotMatrixX(tilt, D_00431AF0, pitch);
            Matrix_Multiply(rotation, tilt, rotation);
            sceVu0TransposeMatrix(parent, parent);
            Matrix_Multiply(rotation, parent, rotation);
            Bone_MatrixToAngles(angles, rotation);
            EntityMath_EaseAngleToward(&unkE0->unk48, angles[0], rate);
            EntityMath_EaseAngleToward(&unkE0->unk4C, angles[1], rate);
            EntityMath_EaseAngleToward(&unkE0->unk50, angles[2], rate);
            bone->unk50[0] = unkE0->unk48;
            bone->unk50[1] = unkE0->unk4C;
            bone->unk50[2] = unkE0->unk50;
        } else if (mode == 0) {
            rate = 10.0f;
            EntityMath_EaseAngleToward(&unkE0->unk48, bone->unk50[0], rate);
            EntityMath_EaseAngleToward(&unkE0->unk4C, bone->unk50[1], rate);
            EntityMath_EaseAngleToward(&unkE0->unk50, bone->unk50[2], rate);
            bone->unk50[0] = unkE0->unk48;
            bone->unk50[1] = unkE0->unk4C;
            bone->unk50[2] = unkE0->unk50;
        } else {
            unkE0->unk48 = bone->unk50[0];
            unkE0->unk4C = bone->unk50[1];
            unkE0->unk50 = bone->unk50[2];
        }
    }
}

/* The on_hit handler (installed in both modes): as EntityAi_OnHit, and it clears flag bit 28. */
static void DarkArcher_OnHit(Entity *entity) {
    u64 flags;
    u64 hit_strength;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    entity->flags &= ~0x10000000;
    flags = entity->flags;
    hit_strength = (flags >> 36) & 3;
    if (hit_strength == 1) {
        if ((s32)(flags >> 19) & 1) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (hit_strength == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler of a spawn whose EntitySpawnInfo.unk40 is set: no states. While the entity is
   not busy it starts DarkArcher_RangedAttack with ranged record 2 when flag bit 63 is clear, the
   pause Entity.unkA2 is over and EntityAi_CanUseRanged finds the player in reach of that record; else it
   starts the move action with the command of EntityAi_TurnCommand, the player as target. Keeps flag bit 10
   clear and bits 11 to 13 at 0. */
static void DarkArcher_ThinkShootOrTurn(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    entity->flags &= ~0x400;
    if (entity->unkC6 == 0) {
        u64 flags = entity->flags | 0x200;
        u32 busy;

        entity->flags = flags;
        busy = (u32)(flags >> 63);
        if (busy == 0 && entity->unkA2 == 0 && EntityAi_CanUseRanged(entity, 2) != 0) {
            EntityAi_StartAttack(entity, DarkArcher_RangedAttack, unknown, 1, 2);
        } else {
            entity->target = &D_00413F80;
            entity->unkC8 = EntityAi_TurnCommand(entity);
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        }
    }
    entity->flags &= ~0x3800;
}

/* The move handler of a spawn whose EntitySpawnInfo.unk40 is set: turns as the command says, as
   EntityAi_Move does, moves nowhere and keeps animation 0. */
static void DarkArcher_MoveTurnOnly(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u16 command = entity->unkC8;
    f32 turn;
    f32 limit;

    if (command & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        limit = -entity->params->turn_limit;
        if (unkE0->unk1C < limit) {
            unkE0->unk1C = limit;
        }
    } else if (command & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        limit = entity->params->turn_limit;
        if (limit < unkE0->unk1C) {
            unkE0->unk1C = limit;
        }
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (command & 0x400) {
        turn *= 2.0f;
    }
    entity->unk94 += turn;
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
}

/* The dormant action of a spawn whose EntitySpawnInfo.unk40 is set (busy bit 0x20): animation 0,
   unseen (Entity.unkD6 0) with flag bits 18, 20 and 21 set until D_0034A018[0] is not 0, then fades
   the model in over 20 frames (counted in work byte unk7B), clears the three bits and thinks. */
static void DarkArcher_DormantFadeIn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, 0, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x200000;
        entity->flags |= 0x100000;
        entity->flags |= 0x40000;
        entity->unkD6 = 0;
        unkE0->unk7B = 0;
        entity->action_step++;
    case 1:
        if (D_0034A018[0] != 0) {
            entity->action_step++;
        }
        break;
    case 2:
        entity->unkD6 = (unkE0->unk7B << 7) / 20;
        unkE0->unk7B++;
        if (unkE0->unk7B > 20) {
            entity->flags &= ~0x200000;
            entity->flags &= ~0x100000;
            entity->flags &= ~0x40000;
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
    entity->model->unk15C = entity->unkD6;
}

static void func_0016B228(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, entity->params->unkC4, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x40000;
        entity->flags |= 0x100000;
        entity->flags |= 0x200000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            ENTITY_FLAG_BITS(entity)->bit26 = 1;
            entity->unkA0 = 20;
            func_001035A8(D_0038CD20[entity->group_id].tag, entity->spawn_info - D_004467D0[entity->group_id]);
            Effect_FreeForOwner(entity);
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}

/* The think handler: as EntityAi_Think, with DarkArcher_ThinkCombat for state 2. Sets work byte
   unk7A to whether EntityParams.unk434 is not 0 and flag bit 44 or 50 is set. */
static void DarkArcher_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if ((s32)(entity->flags >> 53) & 1) {
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
                DarkArcher_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if ((s16)entity->params->unk434 != 0 && (entity->flags & (0x8200L << 35))) {
        unkE0->unk7A = 1;
    } else {
        unkE0->unk7A = 0;
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but the ranged attack is
   DarkArcher_RangedAttack (with flag bit 10 set), and instead of closing in it starts
   DarkArcher_MoveAlongArc when work byte unk7A is set, the player is nearer than
   EntityParams.unk436 and D_00413F80.unk14 is within 40 degrees of Entity.unk68. */
static void DarkArcher_ThinkCombat(Entity *entity, s32 unknown) {
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
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
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
            if (EntityAi_PickAttack((char *)entity)) {
                ENTITY_FLAG_BITS(entity)->bit10 = 1;
                EntityAi_StartAction(entity, DarkArcher_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            angle = EntityMath_AngleDifference(D_00413F80.unk14, entity->unk68);
            if (unkE0->unk7A != 0 && entity->distance_to_player < (f32)entity->params->unk436 && ABS(angle) < 0.6981317f) {
                EntityAi_StartAction(entity, DarkArcher_MoveAlongArc, unknown);
                return;
            }
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but it keeps flag bit 10 set and starts
   effect 0x379 at model point 8 (EntityAi_SpawnBoneEffect) on frames 21 to 31 with record 0, on frames 36 to
   55 with the others. */
static void DarkArcher_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
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
        ENTITY_FLAG_BITS(entity)->bit10 = 1;
        if (((s32)(entity->flags >> 58) & 3) == 0) {
            if (entity->model->unkFC->unk82 >= 0x15 && entity->model->unkFC->unk82 < 0x20) {
                EntityAi_SpawnBoneEffect(entity, 0x379, 8);
            }
        } else {
            if (entity->model->unkFC->unk82 >= 0x24 && entity->model->unkFC->unk82 < 0x38) {
                EntityAi_SpawnBoneEffect(entity, 0x379, 8);
            }
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
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

/* An action (busy bit 0x1000) that DarkArcher_ThinkCombat starts and that moves the entity itself
   in an arc: plays animation 8 to frame 17, then animation 4 while it moves the entity forwards
   along the heading it had (work float unk54 holds that heading plus 180 degrees, and the speed is
   put on the other end of the axis, which gives the same direction as the forward command), the
   speeds eased in work floats unk58 (y) and unk5C. The speed in y goes to minus EntityParams.unk43C
   (up) until EntityCollision_DistanceToPropFloor, the distance to the floor below, reads EntityParams.unk43E or more,
   then to plus unk43C until flag bit 44 or 50 is set; then it plays animation 9 to its end. */
static void DarkArcher_MoveAlongArc(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;
    s32 run = FALSE;

    entity->unkC6 |= 0x1000;
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    height = EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->unkC4, 1);
        unkE0->unk58 = 0.0f;
        unkE0->unk5C = 0.0f;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 0x11) {
            unkE0->unk54 = EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f);
            unkE0->unk58 = -entity->params->unk438;
            unkE0->unk5C = 0.0f;
            unkE0->unk1C = 0.0f;
            entity->action_step++;
        }
        break;
    case 3:
        Motion_Start(entity, 4, 0, 0, 0);
        unkE0->unk5C = EntityMath_Approach(unkE0->unk5C, entity->params->unk54, entity->params->unk50);
        unkE0->unk58 = EntityMath_Approach(unkE0->unk58, -entity->params->unk43C, -entity->params->unk43A);
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        if (height == D_0044682C[0] || height >= entity->params->unk43E) {
            entity->action_step++;
        }
        break;
    case 4:
        unkE0->unk5C = EntityMath_Approach(unkE0->unk5C, entity->params->unk54, entity->params->unk50);
        unkE0->unk58 = EntityMath_Approach(unkE0->unk58, entity->params->unk43C, entity->params->unk43A);
        if (ENTITY_FLAGS(entity) & (0x8200L << 35)) {
            Motion_Start(entity, 9, 0, 0, 1);
            unkE0->unk58 = 0.0f;
            unkE0->unk5C = 0.0f;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
        }
        break;
    case 5:
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
    ENTITY_SCRATCH_VECTOR[1] = unkE0->unk58;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk5C;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, D_00431AF0, unkE0->unk54);
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
