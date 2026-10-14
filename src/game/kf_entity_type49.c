/* The behaviour of entity type 49 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
int rand(void);
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
float func_00195478(float *, char *);
float func_001957B8(float, float);
float func_00194FB8(float);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
int func_00192C98(void *, int, int, u32, int);
extern u8 D_004141D8[];
extern EntityModel *D_00446978;
float func_001954A8(float *, float *);
float func_00194F80(float *a, float *b);
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
void func_00181378(Entity *);
void func_001814B0(Entity *);
void func_001816A0(Entity *);
extern char D_0043E928[];
int printf(const char *, ...);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_0018D4E8(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void func_0019A290(Entity *, s32);

static void func_0015BA70(Entity *entity, s32 unknown);
static void func_0015BC58(Entity *entity, s32 unknown);
static void func_0015C3B8(Entity *entity);
static s32 func_0015C518(Entity *entity, f32 min_distance, f32 max_angle);
static s32 func_0015C688(Entity *entity, f32 max_distance, f32 max_angle);
static void func_0015C7A8(Entity *entity);
static s32 func_0015C958(Entity *entity, s32 unused, f32 reach);
static void func_0015CA40(Entity *entity);
static void func_0015CB68(Entity *entity);
static void func_0015CC20(Entity *entity);
static void func_0015CFD0(Entity *entity);

/* Set-up of a type 49 entity: clears its work area and installs the type's handlers. */
void EntityType49_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_0015BA70;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_0015CB68;
    unkE0->unk18 = (void (*)(void))func_0015C7A8;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
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
    unkE0->unk6C = entity->params->unkCB;
    func_00193378(entity->model, 0, 0);
}

static void func_0015BA70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
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
            func_001958B8(entity, func_0015CC20, 1);
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
                func_0015BC58(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}

static void func_0015BC58(Entity *entity, s32 unknown) {
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
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    if (func_0015C688(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181AE0, 1);
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
            unkE0->unk6E = entity->params->unk436;
            if (func_0018F030((char *)entity)) {
                func_001958B8(entity, func_00182240, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            player_angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
            if (ABS(player_angle) > 0.61086524f) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 60;
                unkE0->unk78 = 1;
            } else {
                entity->target = &D_00413F80;
                if (1.5707964f < ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1]))) {
                    unkE0->unk7A = (func_00194FB8(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                    func_001958B8(entity, func_0015C3B8, unknown);
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
        && ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 0.5235988f
        && entity->distance_to_player < 1000.0f) {
        unkE0->unk6A = 0;
        ENTITY_FLAG_BITS(entity)->bits58 = 0;
        func_001958B8(entity, func_0015CFD0, 1);
        return;
    }
    if (unkE0->unk6A <= entity->params->unk43A) {
        unkE0->unk6A++;
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
                        func_001958B8(entity, func_0015C3B8, unknown);
                        return;
                    }
                    entity->unkC8 = func_00190F90(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                unkE0->unk6A = 0;
                break;
            case 1:
                if (side > 0) {
                    entity->unkC8 = 0x202;
                } else if (side < 0) {
                    entity->unkC8 = 0x102;
                } else {
                    if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                        && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                        unkE0->unk7A = (func_00194FB8(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                        func_001958B8(entity, func_0015C3B8, unknown);
                        return;
                    }
                    // match: called like func_00190F90, with a mode the callee ignores
                    entity->unkC8 = func_0015C958(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                unkE0->unk6A = 0;
                break;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0015C3B8(Entity *entity) {
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

static s32 func_0015C518(Entity *entity, f32 min_distance, f32 max_angle) {
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

static s32 func_0015C688(Entity *entity, f32 max_distance, f32 max_angle) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 distance;
    f32 angle;

    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
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

static void func_0015C7A8(Entity *entity) {
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
            func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        func_00192C98(entity, 0x12, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        func_00192C98(entity, 8, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        func_00192C98(entity, 9, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        func_00192C98(entity, 6, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 8) {
        func_00192C98(entity, 7, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
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
    heading = func_001954A8(entity->model->position, entity->target);
    turn = func_001957B8(entity->model->unkD0[1], heading);
    limit = entity->params->turn_limit;
    if (0.0f <= turn ? limit < turn : limit < -turn) {
        command = 0x100;
        if (0.0f <= turn) {
            command = 0x200;
        }
    }
    if (reach < func_00194F80(entity->model->position, entity->target)) {
        command |= 2;
    }
    return command;
}

static void func_0015CA40(Entity *entity) {
    Entity *other;

    for (other = D_004467E0; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & 1) && ((s32)(other->flags >> 1) & 1)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            EntityUnkE0 *unkE0 = &other->unkE0;

            if (unkE0->unk68 == 0
                && func_00194F80(entity->model->position, other->model->position)
                       < (s16)entity->params->unk434
                && func_00194F80((float *)other->spawn_info, D_00413F80.position) < other->spawn_info->unk4C) {
                unkE0->unk68 = 1;
            }
        }
    }
}

static void func_0015CB68(Entity *entity) {
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
    func_0015CA40(entity);
}

static void func_0015CC20(Entity *entity) {
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
            func_001958B8(entity, func_00181AE0, 1);
            break;
        }
        if (func_0015C688(entity, 300.0f, 0.5235988f)) {
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
            } else {
                printf(D_0043E928);
            }
            break;
        }
        entity->target = &D_00413F80;
        angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
        side = func_0018EAC0(entity, direction);
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
            entity->unkC8 = func_00190F90(entity, 1, entity->radius + 100.0f + 30.0f);
        }
        ((void (*)(Entity *))unkE0->unk18)(entity);
        break;
    }
}

static void func_0015CFD0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
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
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(direction, entity->model->unkD0);
        direction[1] = func_00194FB8(direction[1] - 3.1415927f);
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
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}
