/* GoldenStatue: the behaviour of entity type 29 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Golden Statue. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Think(Entity *, s32);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
extern void EntityAi_ReactionHeavy(Entity *);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_DistanceXZ(void *, void *);
s32 EntityAi_CanAttack(Entity *);
int EntitySight_EntityInWay(int *);
int EntityAi_PickAttack(char *);
float EntityMath_AngleDifference(float, float);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int EntityAi_ThinkReturn(void *, int);
void EntityAi_StepState(void *);
void EntityAi_CheckFarFromSpawn(void *);
extern PlayerData D_00413F80;
extern char D_0043E8C0[];
int printf(const char *, ...);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_CanShoot(Entity *);
s32 EntityAi_CanMelee(Entity *);
s32 EntityAi_PickRangedAttack(Entity *);
s32 EntityAi_PickMeleeAttack(Entity *);
void func_00196D48(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
extern int D_00414184[];
int Motion_Start(void *, int, int, u32, int);
double atan2(double, double);
float EntityMath_WrapAngle(float);
float EntityMath_EaseAngleToward(float *, float, float);
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);

static void GoldenStatue_OnHit(Entity *entity);
static void GoldenStatue_ThinkWaitAtPost(Entity *entity, s32 unknown);
static void GoldenStatue_WalkToPost(Entity *entity, s32 unknown);
static void GoldenStatue_TurnAtPost(Entity *entity, s32 unknown);
static void GoldenStatue_StandAtPost(Entity *entity, s32 unknown);
static void GoldenStatue_ThinkNoStates(Entity *entity, s32 unknown);
static void GoldenStatue_ThinkDecideHoldPostOrCloseIn(Entity *entity, s32 unknown);
static void GoldenStatue_Move(Entity *entity);
static void GoldenStatue_AimPart15AbovePlayer(Entity *entity);
static void GoldenStatue_Animate(Entity *entity);

/* Set-up of a type 29 entity: clears its work area and installs the type's handlers. */
void GoldenStatue_Init(Entity *entity) {
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
    unkE0->animate = (void (*)(void))GoldenStatue_Animate;
    unkE0->on_hit = (void (*)(void))GoldenStatue_OnHit;
    unkE0->move = (void (*)(void))GoldenStatue_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->think = (void (*)(Entity *, s32))GoldenStatue_ThinkWaitAtPost;
    } else if (entity->spawn_info->unk40 == 2) {
        unkE0->think = (void (*)(Entity *, s32))GoldenStatue_ThinkNoStates;
    } else {
        unkE0->think = EntityAi_Think;
    }
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

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void GoldenStatue_OnHit(Entity *entity) {
    u64 flags;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    flags = entity->flags;
    if (((flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler of mode 1 (EntitySpawnInfo.unk40 is 1): no states. Once the entity
   Entity_GetBySpawnIdForMap finds for spawn 0x29 has flag bit 0 set and its flags & 0xE are not 2,
   it puts GoldenStatue_ThinkNoStates in the think slot and calls it. Until then, while it is not busy,
   it calls GoldenStatue_WalkToPost, GoldenStatue_TurnAtPost or GoldenStatue_StandAtPost, picked by work
   bytes unk78 and unk79. */
static void GoldenStatue_ThinkWaitAtPost(Entity *entity, s32 unknown) {
    Entity *other;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u32 phase;

    other = Entity_GetBySpawnIdForMap(entity->group_id, 0x29);
    if (other != NULL && (s32)other->flags & 1) {
        if ((other->flags & 0xE) != 2) {
            void (*handler)(Entity *, s32) = GoldenStatue_ThinkNoStates;

            unkE0->think = handler;
            handler(entity, unknown);
            return;
        }
    }
    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        if (unkE0->unk78 == 0) {
            GoldenStatue_WalkToPost(entity, unknown);
        } else {
            if (unkE0->unk79 != 0) {
                GoldenStatue_TurnAtPost(entity, unknown);
            } else {
                GoldenStatue_StandAtPost(entity, unknown);
            }
        }
    }
}

/* Helper of GoldenStatue_ThinkWaitAtPost, called while work byte unk78 is 0: walks to the point (-7900,
   spawn y, 3050), sets unk78 within 50 of it, and starts the move action. */
static void GoldenStatue_WalkToPost(Entity *entity, s32 unknown) {
    f32 point[4];
    f32 limit;
    EntityUnkE0 *unkE0;
    f32 *target;

    limit = 50.0f;
    target = entity->unkE0.unk30;
    unkE0 = &entity->unkE0;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    point[0] = -7900.0f;
    point[1] = entity->spawn_info->spawn_y;
    point[2] = 3050.0f;
    point[3] = 0;
    Vector_Copy(target, point);
    entity->target = target;
    entity->unkC8 = EntityAi_MoveCommand(entity, 0, 25.0f);
    if (EntityMath_DistanceXZ(entity->model->position, point) < limit) {
        unkE0->unk78 = 1;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Helper of GoldenStatue_ThinkWaitAtPost, called while work bytes unk78 and unk79 are set: clears unk78
   when the entity is more than 100 from the point (-7900, spawn y, 3050); starts the attack
   EntityAi_PickAttack picks when EntityAi_CanAttack says so and EntitySight_EntityInWay does not; else turns on the spot
   to the heading in work float unk48, clears unk79 once it is within the turn limit of it, and
   starts the move action. */
static void GoldenStatue_TurnAtPost(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range = 100.0f;

    home[0] = -7900.0f;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = 3050.0f;
    home[3] = 0.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) > range) {
        unkE0->unk78 = 0;
    }
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity) && !EntitySight_EntityInWay((int *)entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], unkE0->unk48);
    if (ABS(angle) > entity->params->turn_limit) {
        if (angle >= 0.0f) {
            entity->unkC8 |= 0x200;
        } else {
            entity->unkC8 |= 0x100;
        }
    } else {
        unkE0->unk79 = 0;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Helper of GoldenStatue_ThinkWaitAtPost, called while work byte unk78 is set and unk79 is 0: attacks as
   GoldenStatue_TurnAtPost does; else, when Entity.unk68 is more than 30 degrees off its
   heading, keeps it in work float unk48 and sets unk79; starts the move action. */
static void GoldenStatue_StandAtPost(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity) && !EntitySight_EntityInWay((int *)entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (ABS(angle) > 0.5235988f) {
        unkE0->unk48 = entity->unk68;
        unkE0->unk79 = 1;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think handler of mode 2 (EntitySpawnInfo.unk40 is 2), which GoldenStatue_ThinkWaitAtPost also
   installs: as EntityAi_Think without the states; while flag bit 53 is clear it calls
   GoldenStatue_ThinkDecideHoldPostOrCloseIn. */
static void GoldenStatue_ThinkNoStates(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) != 0x06000000) {
        EntityAi_CheckFarFromSpawn(entity);
    }
    EntityAi_StepState(entity);
    if ((s32)(entity->flags >> 53) & 1) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    GoldenStatue_ThinkDecideHoldPostOrCloseIn(entity, unknown);
}

/* Decides for GoldenStatue_ThinkNoStates: guards or dodges the player's swing; while flag bit 32 is
   set starts EntityAi_RangedAttack (EntityAi_CanShoot, EntityAi_PickRangedAttack) with the player farther from the
   spawn point than EntitySpawnInfo.unk4C and EntityAi_MeleeAttack (EntityAi_CanMelee, EntityAi_PickMeleeAttack)
   with the player nearer. While the player's x is below -7900 it goes to one of three posts at x
   -8000, picked by the player's z, and from there turns to the player and starts the ranged attack
   only (work byte unk78 says it is at the post; on arriving it prints D_0043E8C0, "tuitayo",
   Japanese for "arrived"); else it closes in on the player as EntityAi_ThinkCombat does. */
static void GoldenStatue_ThinkDecideHoldPostOrCloseIn(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    f32 direction[4];
    f32 angle;
    f32 chase_angle;
    f32 margin = 0.0f;
    Entity *other;
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
    if (EntityMath_DistanceXZ(entity->spawn_info, &D_00413F80) > entity->spawn_info->unk4C) {
        if (ENTITY_FLAG_BITS(entity)->bit32) {
            if (EntityAi_CanShoot(entity) && !EntitySight_EntityInWay((int *)entity)) {
                EntityAi_PickRangedAttack(entity);
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                return;
            }
        }
    } else if (ENTITY_FLAG_BITS(entity)->bit32) {
        if (EntityAi_CanMelee(entity) && !EntitySight_EntityInWay((int *)entity)) {
            EntityAi_PickMeleeAttack(entity);
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            return;
        }
    }
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (D_00413F80.position[0] < -7900.0f) {
        Vector_Copy(post, &D_00413F80);
        post[0] = -8000.0f;
        if (post[2] > 3350.0f) {
            post[2] = 3550.0f;
        } else if (post[2] < 2750.0f) {
            post[2] = 2550.0f;
        } else {
            post[2] = 3050.0f;
            other = Entity_GetBySpawnId(0x28);
            if (ENTITY_FLAG_BITS(other)->bits2 == 0) {
                other = Entity_GetBySpawnId(0x28);
                if (ENTITY_FLAG_BITS(other)->bit1) {
                    margin = entity->radius + entity->radius;
                }
            }
        }
        if (unkE0->unk78 == 0) {
            if (EntityMath_DistanceXZ(entity->model->position, post) < margin + 60.0f) {
                unkE0->unk78 = 1;
                printf(D_0043E8C0);
            } else {
                entity->target = post;
                turn = EntityAi_TurnFromPush(entity, direction);
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
                }
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
                return;
            }
        } else {
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (EntityMath_DistanceXZ(entity->model->position, post) > margin + 150.0f) {
                entity->target = post;
                unkE0->unk78 = 0;
                return;
            }
            if (ENTITY_FLAG_BITS(entity)->bit32 && EntityAi_CanShoot(entity) && !EntitySight_EntityInWay((int *)entity)) {
                EntityAi_PickRangedAttack(entity);
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                return;
            }
            if (ABS(angle) > 0.5235988f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x201;
                } else {
                    entity->unkC8 = 0x101;
                }
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
                return;
            }
        }
    } else {
        entity->target = &D_00413F80;
    }
    func_00196D48(entity);
    if (ENTITY_FLAG_BITS(entity)->bit31) {
        chase_angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
        if (unkE0->unk79 == 0) {
            unkE0->unk79 = ABS(chase_angle) > 0.5235988f;
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
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: turns as the command says and does not move the entity itself; picks animation
   4 or 3 going forward, 11 or 12 for a turn on the spot, else 0. */
static void GoldenStatue_Move(Entity *entity) {
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
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x100) {
        Motion_Start(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x200) {
        Motion_Start(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}

/* Helper of GoldenStatue_Animate: adds work float unk4C to the x rotation of model part 15. Before
   frame 43 unk4C goes towards the pitch to a point 400 above the player (EntityMath_EaseAngleToward, rate 10);
   from frame 54 it is scaled by (67 - frame) / 14. */
static void GoldenStatue_AimPart15AbovePlayer(Entity *entity) {
    EntityModel *model;
    EntityUnkE0 *unkE0;
    EntityModelPart80 *part;
    s16 frame;
    f32 point[2];
    f32 pitch;

    model = entity->model;
    part = &((EntityModelPart80 *)model->unkF8)[15];
    unkE0 = &entity->unkE0;
    if (part != NULL) {
        frame = model->unkFC->unk82;
        if (frame < 43) {
            Vector_Copy(point, D_00413F80.position);
            point[1] += 400.0f;
            pitch = EntityMath_WrapAngle(atan2(point[1] - model->position[1], entity->distance_to_player));
            EntityMath_EaseAngleToward(&unkE0->unk4C, pitch, 10.0f);
        } else if (frame >= 54) {
            unkE0->unk4C *= (f32)(67 - frame) / 14.0f;
        }
        part->unk50[0] += unkE0->unk4C;
    }
}

/* The animate handler: as EntityAi_Animate; during animation 23 of an attack (busy bit 1) it also
   calls GoldenStatue_AimPart15AbovePlayer, and otherwise clears work float unk4C. */
static void GoldenStatue_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

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
        if ((entity->unkC6 & 1) && entity->unkD1 == 0x17) {
            GoldenStatue_AimPart15AbovePlayer(entity);
        } else {
            unkE0->unk4C = 0.0f;
        }
    }
}
