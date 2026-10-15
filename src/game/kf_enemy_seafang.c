/* Seafang: the behaviour of entity type 28 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Seafang. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
// match: const, so that the store to unk48 can follow the load of it
extern const f32 D_00446804[];
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn3D(char *);
void EntityAi_StepState(void *);
f32 EntityCollision_DistanceToGround(f32 *);
int EntityAi_ThinkReturn(void *, int);
int rand(void);
void EntityPoint_PickNearWithHeight(f32 *, f32, f32, void *, void *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand3D(Entity *, s32, f32);
extern float D_00413F80[];
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void Vector_Copy(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
// match: const, so that the store to unk48 can follow the load of it
extern const f32 D_00446808[];
void EntityPoint_PickNear(float, void *, void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
int Motion_Start(void *, int, int, u32, int);
float EntityMath_EaseAngleToward(float *, float, float);
// match: const, so that the store to unk48 can follow the load of it
extern const f32 D_0044680C[];
void EntityEvent_OnDeath(Entity *);

static void Seafang_Think(Entity *entity, s32 unknown);
static void Seafang_ThinkIdle(Entity *entity, s32 unknown);
static void Seafang_ThinkCombat(Entity *entity, s32 unknown);
static void Seafang_ThinkLowerGradually(Entity *entity, s32 unknown);
static void Seafang_ThinkWander(Entity *entity, s32 unknown);
static void Seafang_ThinkCombatRunAtPlayer(Entity *entity, s32 unknown);
static void Seafang_Move(Entity *entity);
static void Seafang_Death(Entity *entity);
static void Seafang_DeathWithoutDrop(Entity *entity);

/* Set-up of a type 28 entity: clears its work area and installs the type's handlers. */
void Seafang_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    if (entity->spawn_info->unk40 != 0) {
        unkE0->think = Seafang_ThinkLowerGradually;
    } else {
        unkE0->think = Seafang_Think;
    }
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))Seafang_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))Seafang_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think with the type's own functions for the states. Before that it
   reads EntityCollision_DistanceToGround at the entity's position into work float unk48, moves the entity in y by it,
   and starts Seafang_DeathWithoutDrop when it is above 0. */
static void Seafang_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn3D((char *)entity);
        EntityAi_StepState(entity);
        unkE0->unk48 = EntityCollision_DistanceToGround(entity->model->position);
        if (unkE0->unk48 != D_00446804[0] && -entity->unk34 < unkE0->unk48) {
            entity->model->position[1] += entity->unk34 + unkE0->unk48;
            if (unkE0->unk48 > 0.0f) {
                EntityAi_StartAction(entity, Seafang_DeathWithoutDrop, 1);
                return;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                Seafang_ThinkCombat(entity, unknown);
                break;
            case 0:
            default:
                Seafang_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 0: as EntityAi_ThinkIdle, with EntityPoint_PickNearWithHeight picking the point to go
   to and EntityAi_MoveCommand3D the command. */
static void Seafang_ThinkIdle(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNearWithHeight(unkE0->unk30, entity->spawn_info->unk4C, entity->spawn_info->unk56, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of states 1 and 2: as EntityAi_ThinkCombat, but it closes in, at the run, on a
   point off the player in y (by work float unk4C), and its melee attack is always record 0. */
static void Seafang_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
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
            } else {
                ENTITY_FLAG_BITS(entity)->bit57 = 0;
                ENTITY_FLAG_BITS(entity)->bits58 = 0;
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = D_00413F80;
            unkE0->unk4C = entity->unk30 + rand() % 150;
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
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x241;
        } else if (turn < 0) {
            entity->unkC8 = 0x141;
        } else {
            Vector_Copy(unkE0->unk30, D_00413F80);
            unkE0->unk30[1] -= unkE0->unk4C;
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand3D(entity, 1, entity->radius + 100.0f + 30.0f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think handler of a spawn whose EntitySpawnInfo.unk40 is set: as Seafang_Think, but where
   that one puts the entity at once at Entity.unk34 past the surface EntityCollision_DistanceToGround finds, this one
   moves it 5 a frame in y (y grows downward: EntityCollision_DistanceToPropFloor measures the floor below as a larger y);
   and it has its own functions for the states (Seafang_ThinkWander,
   Seafang_ThinkCombatRunAtPlayer). Starts Seafang_DeathWithoutDrop when EntityCollision_DistanceToGround
   reads above 0, as Seafang_Think does. */
static void Seafang_ThinkLowerGradually(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn3D((char *)entity);
        EntityAi_StepState(entity);
        unkE0->unk48 = EntityCollision_DistanceToGround(entity->model->position);
        if (unkE0->unk48 != D_00446808[0] && -entity->unk34 < unkE0->unk48) {
            entity->model->position[1] += 5.0f;
            if (unkE0->unk48 > 0.0f) {
                EntityAi_StartAction(entity, Seafang_DeathWithoutDrop, 1);
                return;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                Seafang_ThinkCombatRunAtPlayer(entity, unknown);
                break;
            case 0:
            default:
                Seafang_ThinkWander(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 0 for Seafang_ThinkLowerGradually: wanders, as
   EntityAi_ThinkIdle. Each time the timer runs out it picks a point near the spawn point
   (EntityPoint_PickNear; 79 times in 100, for 120 to 179 frames) or no target (60 to 89 frames), takes the
   command of EntityAi_MoveCommand towards it (or the turn away from a push, EntityAi_TurnFromPush) and starts the
   move action. */
static void Seafang_ThinkWander(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNear(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of states 1 and 2 for Seafang_ThinkLowerGradually: as
   EntityAi_ThinkCombat, closing in at the run (command bit 0x40) on the player's own position,
   where Seafang_ThinkCombat goes for a point off the player in y; the melee attack is record 0
   while work float unk48 is below -80. */
static void Seafang_ThinkCombatRunAtPlayer(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
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
            } else {
                if (unkE0->unk48 < -80.0f) {
                    ENTITY_FLAG_BITS(entity)->bit57 = 0;
                    ENTITY_FLAG_BITS(entity)->bits58 = 0;
                }
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = D_00413F80;
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
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x241;
        } else if (turn < 0) {
            entity->unkC8 = 0x141;
        } else {
            entity->unkC8 = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: turns the entity and pitches it down or up as the command says,
   and picks the animation. */
static void Seafang_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 value;
    f32 pitch;
    f32 rate;

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
    value = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        value += value;
    }
    entity->unk94 += value;
    if (entity->unkC8 & 0x10) {
        pitch = -0.5235988f;
    } else if (entity->unkC8 & 0x20) {
        pitch = 0.5235988f;
    } else {
        pitch = 0.0f;
    }
    if (ABS(pitch - entity->model->unkD0[0]) > 0.5235988f) {
        rate = 60.0f;
    } else {
        rate = 30.0f;
    }
    value = entity->model->unkD0[0];
    EntityMath_EaseAngleToward(&value, pitch, rate);
    entity->unk90 += value - entity->model->unkD0[0];
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x10) {
        Motion_Start(entity, 8, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        Motion_Start(entity, 9, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death; until the fade starts it also moves the
   entity 2 a frame in y while EntityCollision_DistanceToGround reads below 0. */
static void Seafang_Death(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 fall = 0.0f;

    entity->unkC6 = 0x40;
    unkE0->unk48 = EntityCollision_DistanceToGround(entity->model->position);
    if (unkE0->unk48 != D_0044680C[0] && unkE0->unk48 < fall) {
        fall = 2.0f;
    }
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        fall = 0.0f;
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
    entity->unk88 -= fall;
}

/* A second death action (busy bit 0x40), which the think handlers start when EntityCollision_DistanceToGround reads
   above 0: animation 16, the fade and the removal as in Seafang_Death, but with no movement in
   y and without EntityEvent_OnDeath, which is what drops the gold (Gold_DropFromEntity), frees the
   effects and starts the spawn's script when an entity dies. */
static void Seafang_DeathWithoutDrop(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkA0 = 20;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}
