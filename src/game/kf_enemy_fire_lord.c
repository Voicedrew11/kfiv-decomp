/* FireLord: the behaviour of entity type 24 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Fire Lord. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_effect.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern ModelInstanceBlock D_003E7140[2];
// match: D_003E7140[1], which the set-up function reaches by its own address
extern ModelInstanceBlock D_003E7300;
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
s32 EntityAi_CanShoot(Entity *);
s32 EntityAi_PickRangedAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
float EntityMath_AngleDifference(float, float);
int rand(void);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_MeleeAttack(Entity *);
float EntityMath_HeadingTo(float *, float *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
float sqrtf(float);
float atan2f(float, float);
char *func_0010CAB8(char *, u32);
float EntityMath_WrapAngle(float);
void EntityMath_FacingVector(Vector4 *, float);
void Vector_Scale(float *, float *, float);
int EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
float EntityMath_AngleXZ(float *, char *);
s32 EntityAi_CanUseMelee(Entity *, s32);
extern u8 D_004141D8[];
extern EntityModel *D_00446978;
void func_0010B018(ModelInstance *, s32, s32);

static void FireLord_Animate(Entity *entity);
static void FireLord_Think(Entity *entity, s32 unknown);
static void FireLord_ThinkIdle(Entity *entity, s32 unknown);
static void FireLord_ThinkCombat(Entity *entity, s32 unknown);
static void FireLord_ThinkCombatApproach(Entity *entity, int invoke_now);
static void FireLord_ThinkCombatShootOrApproach(Entity *entity, s32 unknown);
static void FireLord_ThinkCombatSideAnimOrApproach(Entity *entity, s32 unknown);
static void FireLord_ThinkCombatCloseIn(Entity *entity, s32 unknown);
static void FireLord_ThinkCombatBackAway(Entity *entity, s32 unknown);
static void FireLord_PlayAnim6Or7Alternately(Entity *entity);
static void FireLord_MeleeAttack0(Entity *entity);
static void FireLord_MeleeAttack1(Entity *entity);
static void FireLord_PlayAnim6Or7ThenMelee0(Entity *entity);
static void FireLord_Move(Entity *entity);
static s32 func_001335D8(Entity *entity, f32 max_distance, f32 max_angle);
void func_00133708(Entity *entity, s32 unknown);

/* Set-up of a type 24 entity: clears its work area and installs the type's handlers. */
void FireLord_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = FireLord_Think;
    unkE0->animate = (void (*)(void))FireLord_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))FireLord_Move;
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
    D_003E7300 = *(ModelInstanceBlock *)entity->model;
    D_003E7140[0] = *(ModelInstanceBlock *)entity->model;
}

/* The animate handler: as EntityAi_Animate, without that one's test of the EntityParams kind bits
   (0x6000000). */
static void FireLord_Animate(Entity *entity) {
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

/* The think handler: as EntityAi_Think, with FireLord_ThinkIdle for state 0 and
   FireLord_ThinkCombat for states 1 and 2. Before that it keeps the model's last two frames in
   D_003E7140, each at half the alpha of the one before, clears work byte unk7E, and counts down
   work word unk6A, which it sets to EntityParams.unk434 while D_00413F80.unk259 is not 0. Counts
   down work byte unk7D. */
static void FireLord_Think(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    ModelInstanceBlock *block;
    ModelInstanceBlock *previous;

    block = &D_003E7140[1];
    previous = block - 1;
    *block = *previous;
    D_003E7140[0] = *(ModelInstanceBlock *)entity->model;
    D_003E7140[0].model.alpha *= 0.5f;
    D_003E7140[1].model.alpha *= 0.5f;
    unkE0->unk7E = 0;
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk6A = entity->params->unk434;
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                FireLord_ThinkCombat(entity, unknown);
                break;
            case 0:
            default:
                FireLord_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk7D > 0) {
        unkE0->unk7D--;
    }
}

/* The think function of state 0: no command, flag bit 9 clear; starts the move action. */
static void FireLord_ThinkIdle(Entity *entity, s32 unknown) {
    entity->unkC8 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->unkE0.move, unknown);
}

/* The think function of states 1 and 2: every 10 frames (work byte unk7D) sorts the distance to the
   player, less the radius and 100, into a band kept in work byte unk7C (0: above 1200, 1: above
   800, 2: above 500, 3: above 100, 4: nearer), and calls the function of the band:
   FireLord_ThinkCombatApproach, FireLord_ThinkCombatShootOrApproach,
   FireLord_ThinkCombatSideAnimOrApproach, FireLord_ThinkCombatCloseIn or
   FireLord_ThinkCombatBackAway. */
static void FireLord_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    f32 distance;

    if (unkE0->unk7D <= 0) {
        distance = entity->distance_to_player - entity->radius - 100.0f;
        if (distance > 1200.0f) {
            unkE0->unk7C = 0;
            unkE0->unk7D = 10;
        } else if (distance <= 1200.0f && distance > 800.0f) {
            unkE0->unk7C = 1;
            unkE0->unk7D = 10;
        } else if (distance <= 800.0f && distance > 500.0f) {
            unkE0->unk7C = 2;
            unkE0->unk7D = 10;
        } else if (distance <= 500.0f && distance > 100.0f) {
            unkE0->unk7C = 3;
            unkE0->unk7D = 10;
        } else {
            unkE0->unk7C = 4;
            unkE0->unk7D = 10;
        }
    }
    switch (unkE0->unk7C) {
    case 0:
        FireLord_ThinkCombatApproach(entity, unknown);
        break;
    case 1:
        FireLord_ThinkCombatShootOrApproach(entity, unknown);
        break;
    case 2:
        FireLord_ThinkCombatSideAnimOrApproach(entity, unknown);
        break;
    case 3:
        FireLord_ThinkCombatCloseIn(entity, unknown);
        break;
    case 4:
        FireLord_ThinkCombatBackAway(entity, unknown);
        break;
    default:
        FireLord_ThinkCombatApproach(entity, unknown);
        break;
    }
}

/* Band 0 of FireLord_ThinkCombat (and its default): goes for the player, with the command of
   EntityAi_MoveCommand (range 50, far marked). */
static void FireLord_ThinkCombatApproach(Entity *entity, int invoke_now) {
    EntityUnkE0 *sub = &entity->unkE0;

    entity->target = &D_00413F80;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f);
    EntityAi_StartAction(entity, (void (*)(Entity *))sub->move, invoke_now);
}

/* Band 1 of FireLord_ThinkCombat: when EntityAi_CanShoot finds the player in reach of a ranged
   attack, starts EntityAi_RangedAttack with the record EntityAi_PickRangedAttack draws; else goes for the
   player as band 0 does. */
static void FireLord_ThinkCombatShootOrApproach(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (EntityAi_CanShoot(entity)) {
        EntityAi_PickRangedAttack(entity);
        EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    entity->target = &D_00413F80;
    entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f);
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Band 2 of FireLord_ThinkCombat: when the heading is within 20 degrees of Entity.unk68, flips
   work byte unk7B and starts FireLord_PlayAnim6Or7Alternately; else goes for the player as band 0 does. */
static void FireLord_ThinkCombatSideAnimOrApproach(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (!(ABS(angle) > 0.34906584f)) {
        unkE0->unk7B = unkE0->unk7B == 0;
        EntityAi_StartAction(entity, FireLord_PlayAnim6Or7Alternately, unknown);
        return;
    }
    entity->target = &D_00413F80;
    entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f);
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Band 3 of FireLord_ThinkCombat (the player 100 to 500 away): as EntityAi_ThinkCombat, closing
   in on the player with the forward command; but in place of the guard it starts
   FireLord_PlayAnim6Or7ThenMelee0 when func_001335D8 says so, and melee records 0 and 1 have
   the type's own actions (FireLord_MeleeAttack0, FireLord_MeleeAttack1). */
static void FireLord_ThinkCombatCloseIn(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_001335D8(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, FireLord_PlayAnim6Or7ThenMelee0, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack0, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack1, unknown);
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
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Band 4 of FireLord_ThinkCombat (the player nearer than 100): as
   FireLord_ThinkCombatCloseIn, but where that one closes in this one gives command bit 2
   (backwards), with a turn bit when the target is off the heading by more than the turn limit. */
static void FireLord_ThinkCombatBackAway(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_001335D8(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, FireLord_PlayAnim6Or7ThenMelee0, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
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
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack0, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack1, unknown);
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
                direction[0] = EntityMath_HeadingTo(entity->model->position, entity->target);
                angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction[0]);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->unkC8 |= 0x200;
                    } else {
                        entity->unkC8 |= 0x100;
                    }
                }
                entity->unkC8 |= 2;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x1000) that FireLord_ThinkCombatSideAnimOrApproach starts: plays animation 6 or 7
   (work byte unk7B picks) to its end, turning to Entity.unk68. Sets work byte unk7E. */
static void FireLord_PlayAnim6Or7Alternately(Entity *entity) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;

    entity->unkC6 |= 0x1000;
    unkE0->unk7E = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7B != 0) {
            Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
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

/* The melee attack with record 0 (busy bit 1): animation 20 to frame 8; then, the motion stopped,
   it moves 30 a frame along the heading for up to 15 frames or until the player is within 200; then
   plays on with the hit checks of EntityAi_MeleeAttack. On frames 20 to 26 it starts effect 0x3A0
   along the path of the point func_0010CAB8 gives for 8, pointing to the one it gives for 9. */
static void FireLord_MeleeAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    f32 position[4];
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 elapsed;

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
        if (entity->model->unkFC->unk82 >= 8) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            unkE0->unk68 = 15;
            entity->action_step++;
        }
        break;
    case 3:
        EntityMath_FacingVector((Vector4 *)position, entity->model->unkD0[1]);
        Vector_Scale(position, position, 30.0f);
        entity->unk84 += position[0];
        entity->unk88 += position[1];
        entity->unk8C += position[2];
        unkE0->unk68--;
        if (entity->distance_to_player - entity->radius - 100.0f < 200.0f || unkE0->unk68 < 0) {
            entity->action_step++;
        }
        break;
    case 4:
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        from = (f32 *)func_0010CAB8((char *)entity->model, 8);
        tip = (f32 *)func_0010CAB8((char *)entity->model, 9);
        if (from != NULL && tip != NULL) {
            if (entity->model->unkFC->unk82 >= 0x14 && entity->model->unkFC->unk82 <= 0x1A) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = from[0] - unkE0->unk48;
                delta_y = from[1] - unkE0->unk4C;
                delta_z = from[2] - unkE0->unk50;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk48 + delta_x * i / count;
                    position[1] = unkE0->unk4C + delta_y * i / count;
                    position[2] = unkE0->unk50 + delta_z * i / count;
                    Effect_StartSpawner(0x3A0, position, angles, entity);
                }
            }
            unkE0->unk48 = from[0];
            unkE0->unk4C = from[1];
            unkE0->unk50 = from[2];
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!((s32)(entity->flags >> 54) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 55) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 56) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The melee attack with record 1 (busy bit 1): animation 21, as EntityAi_MeleeAttack. On frames 23
   to 27 it starts effect 0x3A0 along the path of the point func_0010CAB8 gives for 16, pointing to
   the one it gives for 17. */
static void FireLord_MeleeAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[1];
    f32 position[4];
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x15, 0, entity->params->unkC4 >> 1, 1);
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
        from = (f32 *)func_0010CAB8((char *)entity->model, 0x10);
        tip = (f32 *)func_0010CAB8((char *)entity->model, 0x11);
        if (from != NULL && tip != NULL) {
            if (entity->model->unkFC->unk82 >= 0x17 && entity->model->unkFC->unk82 <= 0x1B) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = from[0] - unkE0->unk48;
                delta_y = from[1] - unkE0->unk4C;
                delta_z = from[2] - unkE0->unk50;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk48 + delta_x * i / count;
                    position[1] = unkE0->unk4C + delta_y * i / count;
                    position[2] = unkE0->unk50 + delta_z * i / count;
                    Effect_StartSpawner(0x3A0, position, angles, entity);
                }
            }
            unkE0->unk48 = from[0];
            unkE0->unk4C = from[1];
            unkE0->unk50 = from[2];
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!((s32)(entity->flags >> 54) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 55) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 56) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* A second action with animations 6 and 7 (busy bit 0x1000), which bands 3 and 4 start in place of
   the guard: plays 6 or 7, by the side of the player's facing the entity is on, to its end, turning
   to Entity.unk68; then starts FireLord_MeleeAttack0 when EntityAi_CanUseMelee finds the player in
   reach of record 0. Sets work byte unk7E. */
static void FireLord_PlayAnim6Or7ThenMelee0(Entity *entity) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    PlayerData *player;

    entity->unkC6 |= 0x1000;
    unkE0->unk7E = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
    switch (entity->action_step) {
    case 0:
        EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
        player = &D_00413F80;
        if (EntityMath_AngleDifference(player->unk14, EntityMath_AngleXZ(player->position, (char *)entity->model->position)) >= 0.0f) {
            Motion_Start(entity, 6, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 7, 0, entity->params->unkC4 >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (EntityAi_CanUseMelee(entity, 0)) {
                EntityAi_StartAction(entity, FireLord_MeleeAttack0, 1);
            } else if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The move handler: turns as the command says (twice as fast with command bit 0x40 or 0x400) and
   picks the animation as EntityAi_Move does, but adds no movement of its own. */
static void FireLord_Move(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    EntityParams *params;
    f32 turn;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    params = entity->params;
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= params->unk58;
        if (unkE0->unk1C < -params->turn_limit) {
            unkE0->unk1C = -params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += params->unk58;
        if (params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = params->turn_limit;
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
            Motion_Start(entity, 4, 0, params->unkC4, 0);
        } else {
            Motion_Start(entity, 3, 0, params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        Motion_Start(entity, 5, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        Motion_Start(entity, 8, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        Motion_Start(entity, 9, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        Motion_Start(entity, 6, 0, params->unkC4 >> 1, 0);
    } else if (entity->unkC8 & 8) {
        Motion_Start(entity, 7, 0, params->unkC4 >> 1, 0);
    } else if (entity->unkC8 & 0x300) {
        Motion_Start(entity, 3, 0, params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, params->unkC4, 0);
    }
}

static s32 func_001335D8(Entity *entity, f32 max_distance, f32 max_angle) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 distance;
    f32 angle;

    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        if (unkE0->unk6A <= 0) {
            return FALSE;
        }
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (max_distance < distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCA / 5;
}

void func_00133708(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;

    if (unkE0->unk7E != 0 && ENTITY_FLAG_BITS(entity)->bits2 == 0) {
        D_003E7140[0].model.unk14C = ENTITY_MODEL(entity)->unk14C;
        D_003E7140[0].model.unk148 = ENTITY_MODEL(entity)->unk148;
        D_003E7140[1].model.unk14C = ENTITY_MODEL(entity)->unk14C;
        D_003E7140[1].model.unk148 = ENTITY_MODEL(entity)->unk148;
        func_0010B018(&D_003E7140[0].model, 0, unknown);
        func_0010B018(&D_003E7140[1].model, 0, unknown);
    }
}
