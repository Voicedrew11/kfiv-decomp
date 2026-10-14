/* The behaviour of entity type 50 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
float func_001957B8(float, float);
float func_00194FB8(float);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
int func_00192C98(void *, int, int, u32, int);
float func_00195478(float *, char *);
float func_00194F80(void *, void *);
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
void func_00181378(Entity *);
void func_001814B0(Entity *);
void func_001816A0(Entity *);
void func_00181AE0(Entity *);

static void func_0015D410(Entity *entity, s32 unknown);
static void func_0015D598(Entity *entity, s32 unknown);
static void func_0015DA28(Entity *entity);
static void func_0015DB88(Entity *entity);
static void func_0015DE30(Entity *entity);
static s32 func_0015E000(Entity *entity, f32 min_distance, f32 max_angle);
static void func_0015E170(Entity *entity);
static void func_0015E298(Entity *entity);
static void func_0015E350(Entity *entity);

/* Set-up of a type 50 entity: clears its work area and installs the type's handlers. */
void EntityType50_Init(Entity *entity) {
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
    unkE0->unk00 = func_0015D410;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_0015E298;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~0x10000000;
    if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk10, 1);
    } else {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
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

static void func_0015D410(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7B = 30;
    }
    if (unkE0->unk7B > 0) {
        unkE0->unk7B--;
    }
    if (unkE0->unk68 == 1) {
        if (entity->unkC6 == 0) {
            func_001958B8(entity, func_0015E350, 1);
        }
        return;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if ((s32)(entity->flags >> 53) & 1) {
            // match: called as returning nothing, so $2 is free for the next statement
            ((void (*)(void *, int))func_00182DE0)(entity, unknown);
            entity->unkC8 |= 0x440;
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_0015D598(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}

static void func_0015D598(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & 0x100) {
        entity->flags &= ~0x3800;
    } else {
        entity->flags = (entity->flags & ~0x3800) | 0x800;
    }
    if (func_0015E000(entity, 200.0f, 1.0471976f)) {
        func_001958B8(entity, func_0015DB88, 1);
        return;
    }
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_0015DE30, 1);
        return;
    }
    mode = func_0018FD50(entity, 0);
    entity->flags |= 0x200;
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with the entity alone here
        ((void (*)(Entity *))func_00190B88)(entity);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8(entity, func_00182240, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            if (rand() % 5 == 0) {
                unkE0->unk78 = 1;
            } else {
                unkE0->unk78 = 0;
            }
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
    if (entity->unkCD == 1) {
        func_00196D48(entity);
        if ((s32)(entity->flags >> 31) & 1) {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (unkE0->unk79 == 0) {
                unkE0->unk79 = ABS(angle) > 0.5235988f;
                if (unkE0->unk79 == 0) {
                    goto done;
                }
            }
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                unkE0->unk79 = 0;
            }
        } else {
            side = func_0018EAC0(entity, direction);
            switch (unkE0->unk78) {
            case 0:
                if (side > 0) {
                    entity->unkC8 = 0x241;
                } else if (side < 0) {
                    entity->unkC8 = 0x141;
                } else {
                    if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                        && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                        unkE0->unk7A = (func_00194FB8(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                        func_001958B8(entity, func_0015DA28, unknown);
                        return;
                    }
                    entity->unkC8 = func_00190F90(entity, 1, entity->radius + 100.0f + 30.0f) | 0x400;
                }
                break;
            case 1:
                entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
                break;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0015DA28(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7A > 0) {
            func_00192C98(entity, 0xC, 0, entity->params->unkC4, 1);
        } else {
            func_00192C98(entity, 0xB, 0, entity->params->unkC4, 1);
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
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0015DB88(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        switch (rand() % 3) {
        case 0:
            func_00192C98(entity, 0xA, 0, 5, 1);
            break;
        case 1:
            func_00192C98(entity, 6, 0, 5, 1);
            break;
        case 2:
            func_00192C98(entity, 7, 0, 5, 1);
            break;
        }
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
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (!(rand() & 1) && ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 0.5235988f) {
                ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~(3L << 58)) | (1L << 58);
                func_001958B8(entity, func_00182240, 1);
            } else if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0015DE30(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        switch (rand() % 3) {
        case 0:
            func_00192C98(entity, 0xA, 0, 5, 1);
            break;
        case 1:
            func_00192C98(entity, 6, 0, 5, 1);
            break;
        case 2:
            func_00192C98(entity, 7, 0, 5, 1);
            break;
        }
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
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static s32 func_0015E000(Entity *entity, f32 min_distance, f32 max_angle) {
    f32 distance;
    f32 angle;
    f32 player_angle;

    if ((u8)(entity->unkE0.unk7B - 1) >= 15) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    player_angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
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

static void func_0015E170(Entity *entity) {
    Entity *other;

    for (other = D_004467E0; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & 1) && ((s32)(other->flags >> 1) & 1)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            EntityUnkE0 *unkE0 = &other->unkE0;

            if (unkE0->unk68 == 0
                && func_00194F80(entity->model->position, other->model->position)
                       < (s16)entity->params->unk434
                && func_00194F80(other->spawn_info, D_00413F80.position) < other->spawn_info->unk4C) {
                unkE0->unk68 = 1;
            }
        }
    }
}

static void func_0015E298(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            func_0019A310((char *)entity, (void (*)(void))func_00181378, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_001814B0, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
    func_0015E170(entity);
}

static void func_0015E350(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
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
        if (func_0015E000(entity, 200.0f, 1.0471976f)) {
            func_001958B8(entity, func_0015DB88, 1);
            break;
        }
        if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
            func_001958B8(entity, func_00181C60, 1);
            break;
        }
        if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
            func_001958B8(entity, func_00181AE0, 1);
            break;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk68 = 2;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        side = func_0018FD50(entity, 0);
        ENTITY_FLAGS(entity) |= 0x200;
        if (side == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = 0;
            if (entity->unkCF != 0) {
                entity->unk9E = D_00446ED4;
            }
            func_00190B88(entity, 0);
            if (entity->unkCD == 0) {
                unkE0->unk68 = 2;
                if (func_0018F030((char *)entity)) {
                    func_001958B8(entity, func_00182240, 1);
                } else {
                    func_001958B8(entity, func_00181E48, 1);
                }
            }
            break;
        }
        entity->target = &D_00413F80;
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 = 0x241;
        } else if (side < 0) {
            entity->unkC8 = 0x141;
        } else {
            entity->unkC8 = func_00190F90(entity, 1, entity->radius + 100.0f + 30.0f);
        }
        ((void (*)(Entity *))unkE0->unk18)(entity);
        break;
    }
}
