/* The behaviour of entity type 44 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *entity, void (*handler)(Entity *), int run_now);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
extern PlayerData D_00413F80;
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_0018B778(float, void *, void *, void *);
void func_00196D48(Entity *);
float func_00195478(float *, char *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int); // match: this file only matches with this type; func_00190F90.c defines it differently
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
extern int D_00414184[];
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);
void func_00195588(Vector4 *, float);
/* one frame of the jump: forwards at unk44 along unk30, upwards at unk48; both wear off */
static inline void move_along_arc(Entity *entity, EntityUnkE0 *unkE0) {
    entity->unk84 = unkE0->unk44 * unkE0->unk30[0];
    entity->unk88 = -unkE0->unk48;
    entity->unk8C = unkE0->unk44 * unkE0->unk30[2];
    unkE0->unk48 -= entity->params->unk43E * 0.1f;
    unkE0->unk44 -= 0.5f;
    if (unkE0->unk44 < 0.0f) {
        unkE0->unk44 = 0.0f;
    }
}
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
void func_00181378(Entity *);
void func_001814B0(Entity *);
void func_001816A0(Entity *);
void func_00216C48(void *, void *, void *);
f32 func_00194F48(f32 *, f32 *);
void func_00216D20(void *, void *);
int func_0018AE98(void *, void *, void *, float); // match: the distance is the last argument (set up after the pointers)

static void func_001650A0(Entity *entity, s32 unknown);
static void func_00165280(Entity *entity, s32 unknown);
static void func_001658C8(Entity *entity);
static void func_00165A70(Entity *entity);
static void func_00165E68(Entity *entity);
static void func_00166158(Entity *entity);
static void func_00166800(Entity *entity);
static void func_00166B38(Entity *entity);
static s32 func_00166BF8(Entity *entity);

/* Set-up of a type 44 entity: clears its work area and installs the type's handlers. */
void EntityType44_Init(Entity *entity) {
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
    unkE0->unk00 = func_001650A0;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00166B38;
    unkE0->unk18 = (void (*)(void))func_001658C8;
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

static void func_001650A0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->distance_to_player < entity->radius + 300.0f + 100.0f && unkE0->unk7A == 1) {
        unkE0->unk7A = 0;
    } else if (entity->radius + 1000.0f + 100.0f < entity->distance_to_player && unkE0->unk7A == 0) {
        unkE0->unk7A = 1;
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
                func_00165280(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}

static void func_00165280(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 point[4];
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00166800, 1);
        return;
    }
    if (entity->unkA0 == 0) {
        angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
        if (rand() % 100 < entity->params->unk436 && ABS(angle) < 1.0471976f
            && ABS(entity->unk68 - entity->model->unkD0[1]) < 1.0471976f) {
            unkE0->unk7B = 0;
            func_001958B8(entity, func_00165E68, unknown);
            return;
        }
        if (rand() % 100 < entity->params->unk438 && ABS(angle) < 1.5707964f && entity->spawn_info->unk40 == 0
            && func_00166BF8(entity)) {
            unkE0->unk7B = 0;
            func_001958B8(entity, func_00166158, unknown);
            return;
        }
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
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
                func_001958B8(entity, func_00182240, unknown);
                return;
            }
            angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
            if (rand() % 100 < entity->params->unk436 && ABS(angle) < 0.6981317f) {
                unkE0->unk7B = 1;
                func_001958B8(entity, func_00165E68, unknown);
                return;
            }
            if (rand() % 100 < entity->params->unk438 && ABS(angle) < 1.5707964f && entity->spawn_info->unk40 == 0
                && func_00166BF8(entity)) {
                unkE0->unk7B = 1;
                func_001958B8(entity, func_00166158, unknown);
                return;
            }
            func_001958B8(entity, func_00181E48, unknown);
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 60;
            unkE0->unk78 = 0;
            break;
        case 2:
            func_0018B778(entity->spawn_info->unk4C, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        func_00196D48(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
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
            turn = func_0018EAC0(entity, direction);
            if (turn > 0) {
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
            }
            entity->unkC8 |= 0x400;
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_001658C8(Entity *entity) {
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
    if (entity->unkC8 & 0x440) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        func_00192C98(entity, 5, 0, entity->params->unkC4, 0);
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

static void func_00165A70(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, frame, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00165E68(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 side;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        side = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
        if (unkE0->unk7B == 1) {
            if (side > 0.0f) {
                func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        } else if (side < 0.0f) {
            if ((f32)(rand() % 120) > side * 57.29578f + 60.0f) {
                func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        } else {
            if ((f32)(rand() % 120) > side * 57.29578f + 60.0f) {
                func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            }
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (unkE0->unk7B == 0) {
                if ((s32)(entity->flags >> 5) & 1) {
                    entity->unkE0.unk00(entity, 1);
                } else {
                    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
                }
            } else {
                func_0018F030((char *)entity);
                func_001958B8(entity, func_00165A70, 1);
            }
        }
        break;
    }
}

/* the jump at the player: crouch, leap along the arc func_00166BF8 tried out, land; being hit
   in the air ends it (unk7B: what to go back to afterwards) */
static void func_00166158(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModel *model;
    f32 unused[4]; // match: the frame has room for it, and no call here is a tail call

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit17 = FALSE;
        entity->action_step++;
    case 1:
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        model = entity->model;
        if (model->unkFC->unk88 == 0) {
            unkE0->unk48 = entity->params->unk43A;
            unkE0->unk44 = entity->params->unk43C;
            entity->action_step++;
            func_00195588((Vector4 *)unkE0->unk30, model->unkD0[1]);
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 10) {
            move_along_arc(entity, unkE0);
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 0x11, 0, 1, 1);
            entity->action_step += 2;
        }
        break;
    case 4:
        move_along_arc(entity, unkE0);
        if (unkE0->unk48 < 10.0f) {
            if (ABS(entity->unk68 - entity->model->unkD0[1]) > 1.5707964f) {
                ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
                entity->action_step++;
            } else {
                func_00192C98(entity, 9, 0, 5, 1);
                entity->action_step += 2;
            }
        }
        break;
    case 5:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            func_00192C98(entity, 9, 0, 1, 1);
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step = 8;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
        move_along_arc(entity, unkE0);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 9, 0, 1, 1);
            entity->action_step = 7;
        }
        break;
    case 6:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step = 8;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
    case 3:
        move_along_arc(entity, unkE0);
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            entity->action_step++;
        }
        break;
    case 7:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
        move_along_arc(entity, unkE0);
        break;
    case 8:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
            if (unkE0->unk7B == 0) {
                if (ENTITY_FLAG_BITS(entity)->bit5) {
                    unkE0->unk00(entity, 1);
                } else {
                    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
                }
            } else {
                func_0018F030((char *)entity);
                func_001958B8(entity, func_00165A70, 1);
            }
        }
        break;
    }
}

static void func_00166800(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 landed;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        if (func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
        }
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            unkE0->unk48 = entity->params->unk43A * 0.8f;
            entity->action_step = 5;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 4) {
            entity->unk88 = -unkE0->unk48;
            unkE0->unk48 -= entity->params->unk43E * 0.1f;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            func_00192C98(entity, 9, 0, entity->params->unkC4 >> 1, 1);
            entity->action_step++;
        }
        break;
    case 3:
        entity->unk88 = -unkE0->unk48;
        unkE0->unk48 -= entity->params->unk43E * 0.1f;
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            entity->action_step++;
        }
        break;
    case 4:
        entity->unk88 = -unkE0->unk48;
        unkE0->unk48 -= entity->params->unk43E * 0.1f;
        landed = (s32)(ENTITY_FLAGS(entity) >> 44) & 1;
        if (landed == 1) {
            entity->unk84 = 0.0f;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            ENTITY_FLAGS(entity) |= 0x4000;
            ENTITY_FLAGS(entity) &= ~0x10000000;
            entity->action_step++;
        }
        break;
    case 5:
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

static void func_00166B38(Entity *entity) {
    func_00190A30((char *)entity);
    entity->flags &= ~0x400;
    ENTITY_UNK5C(entity)->as_s32 = 0;
    entity->flags &= ~0x200;
    entity->flags &= ~0x10000000;
    if (((entity->flags >> 36) & 3) == 1) {
        if ((s32)((s64)(entity->flags << 13) >> 32) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00181378, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_001814B0, 0);
        }
    } else if (((entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}

/* follows the arc of a jump from the entity's feet and asks func_0018AE98 about the straight
   line to where it ends */
static s32 func_00166BF8(Entity *entity) {
    f32 direction[4];
    f32 from[4];
    f32 target[4];
    f32 rise;
    f32 advance;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 distance;

    func_00195588((Vector4 *)direction, entity->model->unkD0[1]);
    rise = entity->params->unk43A;
    advance = entity->params->unk43C;
    delta_x = delta_y = delta_z = 0.0f;
    while (rise > 0.0f) {
        delta_x += advance * direction[0];
        delta_z += advance * direction[2];
        delta_y -= rise;
        rise -= entity->params->unk43E * 0.1f;
        advance -= 0.5f;
        if (advance < 0.0f) {
            advance = 0.0f;
        }
    }
    direction[0] = delta_x;
    direction[1] = delta_y - 100.0f;
    direction[2] = delta_z;
    func_00216778(from, entity->model->position);
    from[1] -= entity->unk34;
    func_00216C48(target, from, direction);
    distance = func_00194F48(from, target);
    func_00216D20(direction, direction);
    return func_0018AE98(from, direction, NULL, distance) == 0;
}
