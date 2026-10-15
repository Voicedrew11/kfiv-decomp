/* LivingArmor: the behaviour of entity type 36 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Living Armor. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
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
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
extern Qword D_0043E8B0;
float EntityMath_WrapAngle(float);
int Bone_TurnPartToLook(void *, int, int, int, void *, int, float);
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetAttackDelay(Entity *, s32);
s32 EntitySight_PropInWay(Entity *);
s32 EntitySight_ObjectInWay(Entity *);
float sqrtf(float);
float atan2f(float, float);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
char *func_0010CAB8(char *, u32);
s32 EntityAi_CheckMeleeHitAt(Entity *, void *, s32, s32, s32, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);

static void LivingArmor_Think(Entity *entity, s32 unknown);
static void LivingArmor_ThinkCombat(Entity *entity, s32 unknown);
static void LivingArmor_AimPart3AtPlayer(Entity *actor);
static void LivingArmor_Animate(Entity *entity);
static void LivingArmor_OnHit(Entity *entity);
static void LivingArmor_ReactionHeavy(Entity *entity);
static void func_0013D9D0(Entity *entity);
static void LivingArmor_MeleeAttack1(Entity *entity);
static void LivingArmor_Dormant(Entity *entity);
static void LivingArmor_MeleeAttack0(Entity *entity);

/* Set-up of a type 36 entity: clears its work area and installs the type's handlers. */
void LivingArmor_Init(Entity *entity) {
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
    unkE0->think = LivingArmor_Think;
    unkE0->animate = (void (*)(void))LivingArmor_Animate;
    unkE0->on_hit = (void (*)(void))LivingArmor_OnHit;
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
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    unkE0->unk68 = 0;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with LivingArmor_ThinkCombat for state 2. */
static void LivingArmor_Think(Entity *entity, s32 unknown) {
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
        LivingArmor_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but melee record 0 has the type's own
   action, LivingArmor_MeleeAttack0, and the head does not follow the target (flag bit 9 clear). */
static void LivingArmor_ThinkCombat(Entity *entity, s32 unknown) {
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
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
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
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                EntityAi_StartAction(entity, LivingArmor_MeleeAttack0, unknown);
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
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
            if ((s16)entity->params->unk434 != 0) {
                entity->unkC8 |= 0x40;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Helper of LivingArmor_Animate: in animation 17 turns model part 3 to look at the player
   (Bone_TurnPartToLook, the angles kept in work area unk30) and then takes 120 degrees off the part's
   angle unk50. */
static void LivingArmor_AimPart3AtPlayer(Entity *actor) {
    char pad[0x10]; /* unused, but retail reserves the 0x10 bytes */
    Qword pair_copy;
    ModelInstance *model;
    ModelPart *joint;
    float angle;

    pair_copy = D_0043E8B0;
    model = ENTITY_MODEL(actor);
    joint = &model->parts[3];
    if (joint != NULL && actor->unkD1 == 0x11) {
        Bone_TurnPartToLook(model, (int)joint, (int)actor->unkE0.unk30, 1, D_00413F80.position, 0, 10.0f);
        angle = joint->unk50 - 2.0943952f;
        joint->unk50 = angle;
        joint->unk50 = EntityMath_WrapAngle(angle);
    }
}

/* The animate handler: as EntityAi_Animate without the head turn; during an attack (busy bit 1) in
   animation 17 LivingArmor_AimPart3AtPlayer turns model part 3, otherwise work float unk48 is put to
   0. */
static void LivingArmor_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;
    char unused[0x10]; /* retail reserves the 0x10 bytes */

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
        if ((entity->unkC6 & 1) && entity->unkD1 == 0x11) {
            LivingArmor_AimPart3AtPlayer(entity);
            return;
        }
        unkE0->unk48 = 0.0f;
    }
}

/* The on_hit handler: every hit starts LivingArmor_ReactionHeavy, whatever its strength. Sets flag
   bits 18, 20 and 21, and work word unk68 to half of EntityParams.unk430 plus a random number below
   unk430. */
static void LivingArmor_OnHit(Entity *entity) {
    EntityUnkE0 *sub = &entity->unkE0;
    int rnd = rand();

    sub->unk68 = ENTITY_CONTROLLER(entity)->unk430 / 2 + rnd % ENTITY_CONTROLLER(entity)->unk430;
    ENTITY_FLAG_BITS(entity)->bit20 = 1;
    ENTITY_FLAG_BITS(entity)->bit18 = 1;
    ENTITY_FLAG_BITS(entity)->bit21 = 1;
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    EntityAi_SetReaction((char *)entity, (void (*)(void))LivingArmor_ReactionHeavy, 0);
    EntityAi_SetState((char *)entity, 2);
}

/* The reaction to a hit of either strength (busy bit 4): as EntityAi_ReactionHeavy with animation
   16; when it has ended it starts animation 1 and the action func_0013D9D0. */
static void LivingArmor_ReactionHeavy(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        }
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetAttackDelay(entity, entity->params->unkC8);
            EntityAi_SetReaction((char *)entity, NULL, 0);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            Motion_Start(entity, 1, 0, 5, 1);
            EntityAi_StartAction(entity, func_0013D9D0, 1);
        }
        break;
    }
}

static void func_0013D9D0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    if (entity->distance_to_player < (s16)entity->params->unk434 + entity->radius + 100.0f) {
        if (!EntitySight_PropInWay(entity) && !EntitySight_ObjectInWay(entity)) {
            EntityAi_StartAction(entity, LivingArmor_MeleeAttack1, 1);
        }
    }
    if (unkE0->unk68-- <= 0 && entity->action_step == 0) {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        EntityAi_StartAction(entity, LivingArmor_Dormant, 1);
    }
}

/* The melee attack with record 1 (busy bit 1), which func_0013D9D0 starts: animation 17, its hits
   checked at the angle Entity.unk68 (EntityAi_CheckMeleeHitAt); on frames 10 to 22 it starts effect 0x222
   along the path of the point func_0010CAB8 gives for 9. At its end, EntityParams.unk432 times in
   100, it goes back to animation 1 and func_0013D9D0 with a new count in work word unk68; else it
   starts LivingArmor_Dormant. */
static void LivingArmor_MeleeAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[1];
    f32 position[4];
    f32 angles[4];
    f32 *tip;
    f32 *base;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        tip = (f32 *)func_0010CAB8((char *)entity->model, 9);
        base = (f32 *)func_0010CAB8((char *)entity->model, 8);
        if (tip != NULL && base != NULL) {
            if (entity->model->unkFC->unk82 >= 10 && entity->model->unkFC->unk82 <= 22) {
                delta_x = base[0] - tip[0];
                delta_y = base[1] - tip[1];
                delta_z = base[2] - tip[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = tip[0] - unkE0->unk4C;
                delta_y = tip[1] - unkE0->unk50;
                delta_z = tip[2] - unkE0->unk54;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk4C + delta_x * i / count;
                    position[1] = unkE0->unk50 + delta_y * i / count;
                    position[2] = unkE0->unk54 + delta_z * i / count;
                    Effect_StartSpawner(0x222, position, angles, entity);
                }
            }
            unkE0->unk4C = tip[0];
            unkE0->unk50 = tip[1];
            unkE0->unk54 = tip[2];
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->unk68));
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->unk68));
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->unk68));
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (rand() % 100 < entity->params->unk432) {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                unkE0->unk68 = entity->params->unk430 / 2 + rand() % entity->params->unk430;
                ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
                ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
                Motion_Start(entity, 1, 0, 5, 1);
                EntityAi_StartAction(entity, func_0013D9D0, 1);
            } else {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                EntityAi_SetAttackDelay(entity, unk240->unk02);
                ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
                ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
                ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
                EntityAi_StartAction(entity, LivingArmor_Dormant, 1);
            }
        }
        break;
    }
}

/* A dormant action (busy bit 0x20) that func_0013D9D0 and LivingArmor_MeleeAttack1 start; the
   dormant slot holds EntityAi_Dormant. It is the second half of that one: plays animation 2 to its
   end, goes to state 2, clears flag bits 18, 20 and 21 and thinks. */
static void LivingArmor_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 2, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            }
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The melee attack with record 0 (busy bit 1): animation 20, as EntityAi_MeleeAttack; on frames 10
   to 22 it starts effect 0x222 along the path of the point func_0010CAB8 gives for 9. */
static void LivingArmor_MeleeAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    f32 position[4];
    f32 angles[4];
    f32 *tip;
    f32 *base;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unk240->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        tip = (f32 *)func_0010CAB8((char *)entity->model, 9);
        base = (f32 *)func_0010CAB8((char *)entity->model, 8);
        if (tip != NULL && base != NULL) {
            if (entity->model->unkFC->unk82 >= 10 && entity->model->unkFC->unk82 <= 22) {
                delta_x = base[0] - tip[0];
                delta_y = base[1] - tip[1];
                delta_z = base[2] - tip[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = tip[0] - unkE0->unk4C;
                delta_y = tip[1] - unkE0->unk50;
                delta_z = tip[2] - unkE0->unk54;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk4C + delta_x * i / count;
                    position[1] = unkE0->unk50 + delta_y * i / count;
                    position[2] = unkE0->unk54 + delta_z * i / count;
                    Effect_StartSpawner(0x222, position, angles, entity);
                }
            }
            unkE0->unk4C = tip[0];
            unkE0->unk50 = tip[1];
            unkE0->unk54 = tip[2];
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
