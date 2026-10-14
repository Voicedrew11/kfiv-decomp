/* The behaviour of entity type 30 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"
#include "kf_effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to its file
void EntityAi_Move(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
void func_00190A30(char *);
int EntityAi_SetReaction(char *, void (*)(void), int);
void func_00195898(char *, u8);
void EntityAi_ReactionHeavy(Entity *);
extern PlayerData D_00413F80;
int func_00192C98(void *, int, int, u32, int);
char *func_0010CAB8(char *, u32);
void func_0018DCD8(Entity *);
int func_0019A3F0(void *, int);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_0018E9C0(void *);
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_00190BE0(Entity *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);

static void EntityType30_Animate(Entity *entity);
static void EntityType30_OnHit(Entity *entity);
static void EntityType30_Death(Entity *entity);
static void EntityType30_Dormant(Entity *entity);
static void EntityType30_ReturnToDormant(Entity *entity);
static void EntityType30_Think(Entity *entity, s32 unknown);
static void EntityType30_ThinkCombat(Entity *entity, s32 unknown);
static void EntityType30_RangedAttack0(Entity *entity);

/* Set-up of a type 30 entity: clears its work area and installs the type's handlers. */
void EntityType30_Init(Entity *entity) {
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
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = EntityType30_Think;
    unkE0->animate = (void (*)(void))EntityType30_Animate;
    unkE0->on_hit = (void (*)(void))EntityType30_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityType30_Dormant;
    unkE0->death = (void (*)(void))EntityType30_Death;
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
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate with flag bits 11 to 13 put to 0 (the motion never moves
   the entity) and without the head turn. */
static void EntityType30_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void EntityType30_OnHit(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    func_00195898((char *)entity, 2);
}

/* The death action (busy bit 0x40): animation 16; when it has ended it starts effect 0x3BC at the
   points func_0010CAB8 gives for 20 to 22, sets work byte unk7A and waits until the entity whose
   spawn id is EntitySpawnInfo.unk40 has its unk7A set too; then the fade over 20 frames and the
   removal, as EntityAi_Death. */
static void EntityType30_Death(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *other;
    char *part;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            part = func_0010CAB8((char *)entity->model, 0x14);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80.position);
            }
            part = func_0010CAB8((char *)entity->model, 0x15);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80.position);
            }
            part = func_0010CAB8((char *)entity->model, 0x16);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80.position);
            }
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            unkE0->unk7A = 1;
            entity->action_step++;
        }
        break;
    case 3:
        other = Entity_GetBySpawnIdForMap(entity->group_id, entity->spawn_info->unk40);
        if (other != NULL && ((s32)other->flags & 1) && other->unkE0.unk7A != 0) {
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 4:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, but it gets up only while the player is
   inside a fixed rectangle (x -4900 to -2500, z -5350 to -3350), and it leaves flag bit 28 alone.
   */
static void EntityType30_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, 0, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (D_00413F80.position[0] > -4900.0f && D_00413F80.position[0] < -2500.0f &&
            D_00413F80.position[2] > -5350.0f && D_00413F80.position[2] < -3350.0f) {
            if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
                func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
                if (entity->params->flags & 0x100) {
                    ENTITY_FLAG_BITS(entity)->bits11 = 0;
                } else {
                    ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
                }
                ENTITY_FLAG_BITS(entity)->bits36 = 0;
                entity->action_step++;
            }
        }
        break;
    case 2:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000) that EntityType30_Think starts while the player is outside the
   rectangle: starts animation 2 at frame func_0019A3F0(entity, 2) - 1 with flag bits 15 and 18 set;
   when the motion has ended it copies EntitySpawnInfo.unk10 to the model's angles and starts the
   dormant action. */
static void EntityType30_ReturnToDormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 2, func_0019A3F0(entity, 2) - 1, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit15 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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
            ENTITY_FLAG_BITS(entity)->bit15 = FALSE;
            func_00216778(entity->model->unkD0, &entity->spawn_info->unk10);
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
        }
        break;
    }
}

/* The think handler: has no switch on the state. While the entity is not busy: with the player
   outside a fixed rectangle (x -4900 to -2500, z -5350 to -3350) it starts
   EntityType30_ReturnToDormant; else in state 2 it calls EntityType30_ThinkCombat and in the other
   states starts the move action. */
static void EntityType30_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 2);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_0018E9C0(entity);
    if (D_00413F80.position[0] < -4900.0f || D_00413F80.position[0] > -2500.0f ||
        D_00413F80.position[2] < -5350.0f || D_00413F80.position[2] > -3350.0f) {
        EntityAi_StartAction(entity, EntityType30_ReturnToDormant, unknown);
        return;
    }
    if (entity->state == 2) {
        EntityType30_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 2 (called under `entity->state == 2`, not from a switch): as
   EntityAi_ThinkCombat with func_0018FD50 in mode 3, the root movement and the head turn off, and
   EntityType30_RangedAttack0 as the ranged attack; where that one closes in, this one only turns on
   the spot (func_00190BE0). */
static void EntityType30_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = func_0018FD50(entity, 3);
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                EntityAi_StartAction(entity, EntityType30_RangedAttack0, unknown);
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
        entity->unkC8 = func_00190BE0(entity);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame 12 to 76, on two frames in
   three, launches the record's shots at a point 100 below the player (func_0018CF30). */
static void EntityType30_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 direction[4];

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        ENTITY_FLAGS(entity) &= ~0x200;
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
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (entity->model->unkFC->unk82 >= 12 && entity->model->unkFC->unk82 <= 76 && D_00446ED4 % 3 != 0) {
            func_00216778(direction, &D_00413F80);
            direction[1] -= 100.0f;
            func_0018CF30(entity, unk3D8, 12, 76, direction, 0.3926991f, 0.17453294f);
        }
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
