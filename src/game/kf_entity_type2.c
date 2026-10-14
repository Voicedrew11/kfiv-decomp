/* The behaviour of entity type 2 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void func_0018E9C0(void *);
void func_00196870(void *, int);
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_00197F00(Entity *);
s32 func_0018F648(Entity *);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
s32 func_00198D10(Entity *);
s32 func_0018F9C0(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
s32 func_00199EB8(Entity *, f32 *, f32);
float func_00194FB8(float);
int func_0018D4E8(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void func_0019A290(Entity *, s32);

static void EntityType2_Think(Entity *entity, s32 unknown);
static void EntityType2_ThinkCombat(Entity *entity, s32 unknown);
static void EntityType2_Move(Entity *entity);
static void EntityType2_Dormant(Entity *entity);
static void EntityType2_RangedAttack(Entity *entity);

/* Set-up of a type 2 entity: clears its work area and installs the type's handlers. */
void EntityType2_Init(Entity *entity) {
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
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = EntityType2_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityType2_Move;
    unkE0->dormant = (void (*)(void))EntityType2_Dormant;
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
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

/* The think handler: every tenth frame updates what the entity knows of the player; while it is not
   busy it steps Entity.state and in state 2 calls EntityType2_ThinkCombat. States 0 and 1 do
   nothing, and flag bit 53 is not looked at. Always sets flag bit 9. */
static void EntityType2_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_0018E9C0(entity);
        switch (entity->state) {
        case 0:
        case 1:
            break;
        case 2:
            EntityType2_ThinkCombat(entity, unknown);
            break;
        }
    }
    entity->flags |= 0x200;
}

/* The think function of state 2: guards the player's swing; starts EntityAi_MeleeAttack when
   func_00197F00 finds a melee record in reach (func_0018F648 picks it) and the pause Entity.unkA2
   is over; else sets flag bit 32 and, each time the timer runs out with func_0018FD50 choosing the
   attack and func_00198D10 finding a ranged record in reach (func_0018F9C0 picks it), starts
   EntityType2_RangedAttack; else starts the move action. It gives no movement command. */
static void EntityType2_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (func_00197F00(entity) && entity->unkA2 == 0) {
        func_0018F648(entity);
        EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
    } else {
        ENTITY_FLAG_BITS(entity)->bit32 = TRUE;
        side = func_0018FD50(entity, 0);
        if (entity->unkA0 == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = side;
            if (entity->unkCD != entity->unkCF) {
                entity->unk9E = D_00446ED4;
            }
            func_00190B88(entity, side);
            if (entity->unkCD == 0) {
                if (func_00198D10(entity)) {
                    func_0018F9C0(entity);
                    EntityAi_StartAction(entity, EntityType2_RangedAttack, unknown);
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
static void EntityType2_Move(Entity *entity) {
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
}

/* The dormant action (busy bit 0x20): holds animation 1 with flag bit 18 set until the player is
   within EntitySpawnInfo.unk54 or flag bit 43 is set, then goes to state 2 and thinks; unlike
   EntityAi_Dormant it plays no animation 2. */
static void EntityType2_Dormant(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, 0, 1);
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
            func_00195898((char *)entity, 2);
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
   func_0018D4E8 along the reverse of Entity.unk68. */
static void EntityType2_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 1, 1);
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
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        direction[0] = direction[2] = direction[3] = 0.0f;
        direction[1] = func_00194FB8(entity->unk68 + 3.1415927f);
        func_0018D4E8(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
