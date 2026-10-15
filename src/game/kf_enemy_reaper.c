/* Reaper: the behaviour of entity type 2 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Reaper. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_CanMelee(Entity *);
s32 EntityAi_PickMeleeAttack(Entity *);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
s32 EntityAi_CanShoot(Entity *);
s32 EntityAi_PickRangedAttack(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
float EntityMath_WrapAngle(float);
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void EntityAi_SetAttackDelay(Entity *, s32);

static void Reaper_Think(Entity *entity, s32 unknown);
static void Reaper_ThinkCombat(Entity *entity, s32 unknown);
static void Reaper_Move(Entity *entity);
static void Reaper_Dormant(Entity *entity);
static void Reaper_RangedAttack(Entity *entity);

/* Set-up of a type 2 entity: clears its work area and installs the type's handlers. */
void Reaper_Init(Entity *entity) {
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
    unkE0->think = Reaper_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))Reaper_Move;
    unkE0->dormant = (void (*)(void))Reaper_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: every tenth frame updates what the entity knows of the player; while it is not
   busy it steps Entity.state and in state 2 calls Reaper_ThinkCombat. States 0 and 1 do
   nothing, and flag bit 53 is not looked at. Always sets flag bit 9. */
static void Reaper_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_StepState(entity);
        switch (entity->state) {
        case 0:
        case 1:
            break;
        case 2:
            Reaper_ThinkCombat(entity, unknown);
            break;
        }
    }
    entity->flags |= 0x200;
}

/* The think function of state 2: guards the player's swing; starts EntityAi_MeleeAttack when
   EntityAi_CanMelee finds a melee record in reach (EntityAi_PickMeleeAttack picks it) and the pause Entity.unkA2
   is over; else sets flag bit 32 and, each time the timer runs out with EntityAi_PickCombatChoice choosing the
   attack and EntityAi_CanShoot finding a ranged record in reach (EntityAi_PickRangedAttack picks it), starts
   Reaper_RangedAttack; else starts the move action. It gives no movement command. */
static void Reaper_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_CanMelee(entity) && entity->unkA2 == 0) {
        EntityAi_PickMeleeAttack(entity);
        EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
    } else {
        ENTITY_FLAG_BITS(entity)->bit32 = TRUE;
        side = EntityAi_PickCombatChoice(entity, 0);
        if (entity->unkA0 == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = side;
            if (entity->unkCD != entity->unkCF) {
                entity->unk9E = D_00446ED4;
            }
            EntityAi_LowerChoiceWeight(entity, side);
            if (entity->unkCD == 0) {
                if (EntityAi_CanShoot(entity)) {
                    EntityAi_PickRangedAttack(entity);
                    EntityAi_StartAction(entity, Reaper_RangedAttack, unknown);
                    return;
                }
            } else {
                entity->unkA0 = rand() % 60 + 0x3C;
            }
        }
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
    }
}

/* The move handler: this type stays where it is; keeps animation 0. */
static void Reaper_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
}

/* The dormant action (busy bit 0x20): holds animation 1 with flag bit 18 set until the player is
   within EntitySpawnInfo.unk54 or flag bit 43 is set, then goes to state 2 and thinks; unlike
   EntityAi_Dormant it plays no animation 2. */
static void Reaper_Dormant(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but the shots are launched with
   EntityShot_Launch along the reverse of Entity.unk68. */
static void Reaper_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 direction[4];
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
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        direction[0] = direction[2] = direction[3] = 0.0f;
        direction[1] = EntityMath_WrapAngle(entity->unk68 + 3.1415927f);
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
