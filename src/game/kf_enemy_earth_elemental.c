/* EarthElemental: the behaviour of entity type 49 (Entity.params->unk02, the index into D_003E6E40): its
   set-up function, which the table names, and the handlers it installs. Used by: Earth Elemental. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
int rand(void);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
float EntityMath_WrapAngle(float);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
extern u8 D_004141D8[];
extern EntityModel *D_00446978;
float EntityMath_HeadingTo(float *, float *);
float EntityMath_DistanceXZ(float *a, float *b);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
extern char D_0043E928[];
int printf(const char *, ...);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void EntityAi_SetAttackDelay(Entity *, s32);

static void EarthElemental_Think(Entity *entity, s32 unknown);
static void EarthElemental_ThinkCombat(Entity *entity, s32 unknown);
static void EarthElemental_PlayTurnAnim(Entity *entity);
static s32 func_0015C518(Entity *entity, f32 min_distance, f32 max_angle);
static s32 func_0015C688(Entity *entity, f32 max_distance, f32 max_angle);
static void EarthElemental_Move(Entity *entity);
static s32 func_0015C958(Entity *entity, s32 unused, f32 reach);
static void EarthElemental_AlertSameKind(Entity *entity);
static void EarthElemental_OnHit(Entity *entity);
static void EarthElemental_BackAwayOrRunAtPlayer(Entity *entity);
static void EarthElemental_RangedAttack0(Entity *entity);

/* Set-up of a type 49 entity: clears its work area and installs the type's handlers. */
void EarthElemental_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = EarthElemental_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EarthElemental_OnHit;
    unkE0->move = (void (*)(void))EarthElemental_Move;
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
    unkE0->unk6C = entity->params->unkCB;
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with EarthElemental_ThinkCombat for state 2; walking back to
   the spawn point (flag bit 53) gets command bits 0x440 added. Work byte unk7B is put to 25 to 34
   while D_00413F80.unk259 is set and counts down; work word unk6C is EntityParams.unk438 while work
   word unk6E counts down and EntityParams.unkCB otherwise. While work word unk68 is 1
   (EarthElemental_AlertSameKind of another entity sets it) it only starts EarthElemental_BackAwayOrRunAtPlayer. */
static void EarthElemental_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7B = rand() % 10 + 25;
    }
    if (unkE0->unk7B > 0) {
        unkE0->unk7B--;
    }
    if (unkE0->unk6E > 0) {
        unkE0->unk6C = entity->params->unk438;
        unkE0->unk6E--;
    } else {
        unkE0->unk6C = entity->params->unkCB;
    }
    if (unkE0->unk68 == 1) {
        if (entity->unkC6 == 0) {
            EntityAi_StartAction(entity, EarthElemental_BackAwayOrRunAtPlayer, 1);
        }
        return;
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if ((s32)(entity->flags >> 53) & 1) {
            // match: called as returning nothing, so $2 is free for the next statement
            ((void (*)(void *, int))EntityAi_ThinkReturn)(entity, unknown);
            entity->unkC8 |= 0x440;
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                EarthElemental_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but it first starts EntityAi_Dodge when
   func_0015C518 says so and guards when func_0015C688 says so; an attack sets work word unk6E to
   EntityParams.unk436. Closing in is one of three things: with the entity more than 35 degrees off
   the player's facing it goes backwards for 60 to 89 frames (work byte unk78 is 1, command from
   func_0015C958); else it runs at the player for 60 to 119 frames (unk78 is 0); with the player
   more than 90 degrees off its heading it starts EarthElemental_PlayTurnAnim. Work word unk6A counts
   the frames it is not going for the player; above EntityParams.unk43A, with the player within 30
   degrees ahead and nearer than 1000, it starts EarthElemental_RangedAttack0. */
static void EarthElemental_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    f32 player_angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & 0x100) {
        entity->flags &= ~0x3800;
    } else {
        entity->flags = (entity->flags & ~0x3800) | 0x800;
    }
    if (func_0015C518(entity, 200.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (func_0015C688(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    entity->flags |= 0x200;
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with the entity alone here
        ((void (*)(Entity *))EntityAi_LowerChoiceWeight)(entity);
        switch (entity->unkCD) {
        case 0:
            unkE0->unk6E = entity->params->unk436;
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            player_angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
            if (ABS(player_angle) > 0.61086524f) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 60;
                unkE0->unk78 = 1;
            } else {
                entity->target = &D_00413F80;
                if (1.5707964f < ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1]))) {
                    unkE0->unk7A = (EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, EarthElemental_PlayTurnAnim, unknown);
                    return;
                }
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 60;
                unkE0->unk78 = 0;
            }
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 2;
            break;
        default:
            unkE0->unk78 = 2;
            break;
        }
    }
    if (unkE0->unk6A > entity->params->unk43A
        && ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) < 0.5235988f
        && entity->distance_to_player < 1000.0f) {
        unkE0->unk6A = 0;
        ENTITY_FLAG_BITS(entity)->bits58 = 0;
        EntityAi_StartAction(entity, EarthElemental_RangedAttack0, 1);
        return;
    }
    if (unkE0->unk6A <= entity->params->unk43A) {
        unkE0->unk6A++;
    }
    if (entity->unkCD == 1) {
        func_00196D48(entity);
        if ((s32)(entity->flags >> 31) & 1) {
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
            side = EntityAi_TurnFromPush(entity, direction);
            switch (unkE0->unk78) {
            case 0:
                if (side > 0) {
                    entity->unkC8 = 0x241;
                } else if (side < 0) {
                    entity->unkC8 = 0x141;
                } else {
                    if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                        && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                        unkE0->unk7A = (EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                        EntityAi_StartAction(entity, EarthElemental_PlayTurnAnim, unknown);
                        return;
                    }
                    entity->unkC8 = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                unkE0->unk6A = 0;
                break;
            case 1:
                if (side > 0) {
                    entity->unkC8 = 0x202;
                } else if (side < 0) {
                    entity->unkC8 = 0x102;
                } else {
                    if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                        && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                        unkE0->unk7A = (EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                        EntityAi_StartAction(entity, EarthElemental_PlayTurnAnim, unknown);
                        return;
                    }
                    // match: called like EntityAi_MoveCommand, with a mode the callee ignores
                    entity->unkC8 = func_0015C958(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                unkE0->unk6A = 0;
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x2000): plays animation 12 or, with work byte unk7A below 1, animation 11 to
   its end. EarthElemental_ThinkCombat starts it when the player is more than 90 degrees off the
   heading. */
static void EarthElemental_PlayTurnAnim(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7A > 0) {
            Motion_Start(entity, 0xC, 0, entity->params->unkC4, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
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

static s32 func_0015C518(Entity *entity, f32 min_distance, f32 max_angle) {
    f32 distance;
    f32 angle;
    f32 player_angle;

    if ((u8)(entity->unkE0.unk7B - 1) >= 15) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    player_angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (ABS(player_angle) > 0.5235988f) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCA / 10;
}

static s32 func_0015C688(Entity *entity, f32 max_distance, f32 max_angle) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 distance;
    f32 angle;

    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (distance > max_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        return FALSE;
    }
    return rand() % 100 < unkE0->unk6C / 5;
}

/* The move handler: as EntityAi_Move, but it only turns the entity and picks the animation, it does
   not move it; going backwards is animation 18. */
static void EarthElemental_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
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
    unkE0->unk44 = 0.0f;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        Motion_Start(entity, 0x12, 0, entity->params->unkC4, 0);
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

/* The movement command for walking to the target: a turn bit when the target is off the heading by more than
   the turn limit, and bit 1 while it is farther away than reach. */
static s32 func_0015C958(Entity *entity, s32 unused, f32 reach) {
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

/* Helper of EarthElemental_OnHit: sets work word unk68 to 1 in every other live entity with the same
   EntityParams that is within EntityParams.unk434 of this one, has unk68 at 0 and has the player
   within EntitySpawnInfo.unk4C of its spawn point. */
static void EarthElemental_AlertSameKind(Entity *entity) {
    Entity *other;

    for (other = D_004467E0; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & 1) && ((s32)(other->flags >> 1) & 1)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            EntityUnkE0 *unkE0 = &other->unkE0;

            if (unkE0->unk68 == 0
                && EntityMath_DistanceXZ(entity->model->position, other->model->position)
                       < (s16)entity->params->unk434
                && EntityMath_DistanceXZ((float *)other->spawn_info, D_00413F80.position) < other->spawn_info->unk4C) {
                unkE0->unk68 = 1;
            }
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, then EarthElemental_AlertSameKind. */
static void EarthElemental_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
    EarthElemental_AlertSameKind(entity);
}

/* An action (busy bit 0x3000) that EarthElemental_Think starts while work word unk68 is 1
   (EarthElemental_AlertSameKind of another entity sets it). It starts no animation of its own. Each
   frame it dodges or guards (func_0015C518, func_0015C688, EntityAi_ShouldDodge), starts the attack
   EntityAi_PickAttack picks once EntityAi_PickCombatChoice chooses attacking (unk68 becomes 2), and else gives a
   command with the player as target and calls the move handler itself: backwards (bit 2) while the
   entity is more than 45 degrees off the player's facing, forwards at the run (0x41) while it is
   within. Flag bit 53 ends it with unk68 at 2. */
static void EarthElemental_BackAwayOrRunAtPlayer(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (func_0015C518(entity, 200.0f, 1.0471976f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if (func_0015C688(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Guard, 1);
            break;
        }
        if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk68 = 2;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        side = EntityAi_PickCombatChoice(entity, 0);
        ENTITY_FLAGS(entity) |= 0x200;
        if (side == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = 0;
            if (entity->unkCF != 0) {
                entity->unk9E = D_00446ED4;
            }
            EntityAi_LowerChoiceWeight(entity, 0);
            if (entity->unkCD == 0) {
                unkE0->unk68 = 2;
                if (EntityAi_PickAttack((char *)entity)) {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
                }
            } else {
                printf(D_0043E928);
            }
            break;
        }
        entity->target = &D_00413F80;
        angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
        side = EntityAi_TurnFromPush(entity, direction);
        if (ABS(angle) > 0.7853982f) {
            entity->unkC8 = 2;
        } else {
            entity->unkC8 = 0x41;
        }
        if (side > 0) {
            entity->unkC8 |= 0x200;
        } else if (side < 0) {
            entity->unkC8 |= 0x100;
        } else if (ABS(angle) > 0.7853982f) {
            // match: the caller passes a second integer argument the callee does not take
            entity->unkC8 = func_0015C958(entity, 1, entity->radius + 100.0f + 30.0f);
        } else {
            entity->unkC8 = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
        ((void (*)(Entity *))unkE0->move)(entity);
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23, as EntityAi_RangedAttack, but the
   shots are launched with EntityShot_Launch along the reverse of its heading. EarthElemental_ThinkCombat
   starts it. */
static void EarthElemental_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 direction[4];
    s32 elapsed;

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
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(direction, entity->model->unkD0);
        direction[1] = EntityMath_WrapAngle(direction[1] - 3.1415927f);
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction);
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
