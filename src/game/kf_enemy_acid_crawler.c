/* AcidCrawler: the behaviour of entity type 18 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Acid Crawler Large, Acid Crawler
   Small. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to
   the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
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
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
float EntityMath_StepToward(float *, float, float);
extern u32 D_00446ED4;
int rand(void);
char *func_0010CAB8(char *, u32);
int EntityAi_LaunchShot(void *, int, void *, void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
extern PlayerData D_00413F80;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
int EntityAi_StartAttack(Entity *, void (*)(Entity *entity), int, int, int);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int EntityAi_TurnToPoint(Entity *, f32 *, f32 *);
int Motion_Start(void *, int, int, u32, int);

static void AcidCrawler_Animate(Entity *entity);
static void AcidCrawler_TurnParts30To34ToTarget(void *entity_arg);
static void AcidCrawler_Think(Entity *entity, s32 unknown);
static void AcidCrawler_ThinkCombat(Entity *entity, s32 unknown);
static void AcidCrawler_Move(Entity *entity);

/* Set-up of a type 18 entity: clears its work area and installs the type's handlers. */
void AcidCrawler_Init(Entity *entity) {
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
    unkE0->think = AcidCrawler_Think;
    unkE0->animate = (void (*)(void))AcidCrawler_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))AcidCrawler_Move;
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

/* The animate handler: as EntityAi_Animate, with the head turned at rates 8 and 5 where that one
   has 12 and 8; then AcidCrawler_TurnParts30To34ToTarget turns model parts 30 to 34. */
static void AcidCrawler_Animate(Entity *entity) {
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
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 2) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, look_at_target, limits, 8.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, 0, NULL, 5.0f);
        }
        AcidCrawler_TurnParts30To34ToTarget(entity);
    }
}

/* Helper of AcidCrawler_Animate: turns five joints in a row (model parts 30 to 34) towards the
   target, in animations 0, 3, 4, 11 and 12 (in the others it puts them straight). Facing it: each
   joint moves to where the joint before it was, the first back to straight. Otherwise the turn is
   spread over the joints, each taking up to 15 degrees, and the joints past that straighten. */
static void AcidCrawler_TurnParts30To34ToTarget(void *entity_arg) {
    Entity *entity = entity_arg;
    EntityUnkE0Kind12ACA0 *unkE0 = (EntityUnkE0Kind12ACA0 *)&entity->unkE0;
    EntityModelPart80 *part;
    f32 previous[5];
    f32 limit = 0.2617994f;
    f32 angle;
    f32 step;
    f32 remaining;
    f32 moved;
    s32 done;
    s32 i;
    f32 *previous_joint;
    s32 part_index;

    part = (EntityModelPart80 *)entity->model->unkF8;
    if (entity->unkD1 != 0 && entity->unkD1 != 3 && entity->unkD1 != 4 && entity->unkD1 != 0xB &&
        entity->unkD1 != 0xC) {
        unkE0->unk40[2] = unkE0->unk40[3] = unkE0->unk40[4] = unkE0->unk40[5] = unkE0->unk40[6] = 0.0f;
        return;
    }
    if (entity->target != NULL) {
        angle = EntityMath_HeadingTo(entity->model->position, entity->target);
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], angle);
        previous[0] = unkE0->unk40[2];
        previous[1] = unkE0->unk40[3];
        previous[2] = unkE0->unk40[4];
        previous[3] = unkE0->unk40[5];
        previous[4] = unkE0->unk40[6];
        step = entity->params->turn_limit;
        if (angle >= -0.0001f && angle <= 0.0001f) {
            if (unkE0->unk40[2] != 0.0f) {
                moved = EntityMath_StepToward(&unkE0->unk40[2], 0.0f, step);
                moved = ABS(moved);
                if (moved < step) {
                    step -= moved;
                } else {
                    step = 0.0f;
                }
            }
            previous_joint = previous;
            for (i = 0; i < 4; i++, previous_joint++) {
                if (unkE0->unk40[i + 3] != *previous_joint) {
                    moved = EntityMath_StepToward(&unkE0->unk40[i + 3], *previous_joint, step);
                    moved = ABS(moved);
                    if (moved < step) {
                        step -= moved;
                    } else {
                        step = 0.0f;
                    }
                }
            }
        } else {
            remaining = unkE0->unk1C;
            done = FALSE;
            for (i = 2; i < 7; i++) {
                if (!done) {
                    unkE0->unk40[i] += remaining;
                    if (unkE0->unk40[i] > limit) {
                        remaining = unkE0->unk40[i] - limit;
                        unkE0->unk40[i] = limit;
                    } else if (unkE0->unk40[i] < -limit) {
                        remaining = unkE0->unk40[i] + limit;
                        unkE0->unk40[i] = -limit;
                    } else {
                        done = TRUE;
                    }
                } else if (unkE0->unk40[i] != 0.0f) {
                    moved = EntityMath_StepToward(&unkE0->unk40[i], 0.0f, step);
                    moved = ABS(moved);
                    if (moved < step) {
                        step -= moved;
                    } else {
                        step = 0.0f;
                    }
                }
            }
        }
    }
    part_index = 30;
    for (i = 0; i < 5; i++) {
        part[part_index++].unk50[1] = -unkE0->unk40[i + 2];
    }
}

/* The think handler: as EntityAi_Think, with AcidCrawler_ThinkCombat for states 1 and 2; after
   EntityAi_ThinkIdle it sets work byte unk7A to 1. Before that, while Entity.unk2A is 0, it
   launches shot EntityParams.unk43C from near the point func_0010CAB8 gives for 0, on two timers:
   work word unk68 (chance EntityParams.unk434 in 100 a frame, then a pause of unk436 frames) and
   work word unk6A (chance unk438, pause unk43A). */
static void AcidCrawler_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *part = func_0010CAB8((char *)entity->model, 0);
    f32 target[4];
    f32 direction[4];
    f32 offset;

    if (part != NULL && entity->unk2A == 0) {
        direction[0] = 0.0f;
        direction[1] = 0.0f;
        direction[2] = 0.0f;
        direction[3] = 0.0f;
        if (unkE0->unk68 == 0) {
            if (rand() % 100 < ENTITY_CONTROLLER(entity)->unk434) {
                Vector_Copy(target, part);
                offset = rand() % 10 - 5;
                target[0] += offset;
                target[1] += offset;
                target[2] += offset;
                EntityAi_LaunchShot(entity, entity->params->unk43C, target, direction);
                unkE0->unk68 = entity->params->unk436;
            }
        }
        if (unkE0->unk68 != 0) {
            unkE0->unk68--;
        }
        if (unkE0->unk6A == 0) {
            if (rand() % 100 < ENTITY_CONTROLLER(entity)->unk438) {
                Vector_Copy(target, part);
                offset = rand() % 10 - 5;
                target[0] += offset;
                target[1] += offset;
                target[2] += offset;
                EntityAi_LaunchShot(entity, entity->params->unk43C, target, direction);
                unkE0->unk6A = entity->params->unk43A;
            }
        }
        if (unkE0->unk6A != 0) {
            unkE0->unk6A--;
        }
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
        break;
    case 1:
    case 2:
        AcidCrawler_ThinkCombat(entity, unknown);
        return;
    }
    ((void (*)(void *, int))EntityAi_ThinkIdle)(entity, unknown); // match: called as void, $2 is reused at once
    unkE0->unk7A = 1;
}

/* The think function of states 1 and 2: guards or dodges the player's swing as EntityAi_ThinkCombat
   does; attacks with what EntityAi_PickAttack draws once the pause Entity.unkA2 is over and EntityAi_CanAttack
   finds the player in reach; each time flag bit 32 comes on it starts EntityAi_RangedAttack with
   record 0, 40 times in 100 (work byte unk7A); else it goes for the player with command 0x841, or
   0x241 / 0x141 while it is pushed. */
static void AcidCrawler_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
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
    if (entity->unkA2 == 0 && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) { // match: the entity is passed
        entity->target = NULL;
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
        }
        return;
    }
    entity->flags |= 0x200;
    if ((s32)(entity->flags >> 32) & 1) {
        if (unkE0->unk7A == 1) {
            unkE0->unk7A = 0;
            if (rand() % 100 < 40) {
                EntityAi_StartAttack(entity, EntityAi_RangedAttack, unknown, 1, 0);
                return;
            }
        }
    } else {
        unkE0->unk7A = 1;
    }
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
    } else if (unkE0->unk78 == 1) {
        entity->target = &D_00413F80;
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x241;
        } else if (turn < 0) {
            entity->unkC8 = 0x141;
        } else {
            entity->unkC8 = 0x841;
        }
    } else {
        unkE0->unk78 = 1;
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: turns as the command says, or towards the target with command bit 0x800
   (EntityAi_TurnToPoint); picks animation 4 or 3 walking, 3 turning, 17 for command bit 0x8000, 4 for bit
   0x800, else 0. It adds no movement of its own. */
static void AcidCrawler_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 vector[4];
    f32 turn;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    vector[0] = vector[1] = vector[2] = vector[3] = turn = 0.0f;
    if (entity->unkC8 & 0x800) {
        if (entity->target != NULL) {
            EntityAi_TurnToPoint(entity, &unkE0->unk1C, entity->target);
        }
    } else if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
        turn = unkE0->unk1C;
        if (entity->unkC8 & 0x400) {
            turn += turn;
        }
        entity->unk94 += turn;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
        turn = unkE0->unk1C;
        if (entity->unkC8 & 0x400) {
            turn += turn;
        }
        entity->unk94 += turn;
    } else {
        unkE0->unk1C = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, (s32)((f32)entity->params->unkC4 * 0.5f), 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x300) {
        Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x8000) {
        Motion_Start(entity, 0x11, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x800) {
        Motion_Start(entity, 4, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}
