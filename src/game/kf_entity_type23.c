/* The behaviour of entity type 23 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_map_object.h"
#include "kf_entity_world.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00182C80(Entity *, s32);
void func_00183570(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
void func_001816A0(Entity *);
int func_00192C98(void *, int, int, u32, int);
float func_001957B8(float, float);
int func_0019A3F0(void *, int);
float func_00195388(float *, float, float);
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
void func_00196870(void *, int);
s32 func_00197078(Entity *);
int func_0018F030(char *);
float func_00194F80(void *, void *);
void func_00216778(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
void func_00181E48(Entity *);
void func_00182240(Entity *);
float func_001954A8(float *, float *);
extern EntityIdTable D_0043E890;

static void func_001384C0(Entity *entity);
static void func_001385B0(Entity *entity);
static void func_00138608(Entity *entity);
static void func_001387C0(Entity *entity);
static void func_00138970(Entity *entity);
static void func_001389C8(Entity *entity, s32 unknown);
static void func_00138A48(Entity *entity, s32 unknown);
static void func_00138D50(Entity *entity);
static void func_00139010(Entity *entity);
static void func_00139538(Entity *entity);
static void func_00139730(Entity *entity, s32 unknown);
static void func_00139960(Entity *entity);
static void func_00139AE8(Entity *entity);
static void func_00139DE0(Entity *entity);
static void func_00139FE0(Entity *entity, s32 unknown);
static void func_0013A210(Entity *entity);
static void func_0013A398(Entity *entity);
static void func_0013A690(Entity *entity);
static void func_0013A890(Entity *entity, s32 unknown);
static void func_0013AAD8(Entity *entity);
static void func_0013AC60(Entity *entity);
static void func_0013AF48(Entity *entity);
static void func_0013B150(Entity *entity, s32 unknown);
static void func_0013B4C8(Entity *entity, s32 unknown);
static void func_0013B568(Entity *entity, s32 unknown);
static void func_0013B608(Entity *entity, s32 unknown);
static void func_0013B828(Entity *entity);
static void func_0013B9E8(Entity *entity, s32 unknown);
static void func_0013BA70(Entity *entity, s32 unknown);
static void func_0013BC60(Entity *entity);
static void func_0013BE18(Entity *entity, s32 unknown);
static s32 func_0013BF48(Entity *entity, s32 value);
static s32 func_0013C018(Entity *entity, f32 limit);

/* Set-up of a type 23 entity: clears its work area and installs the type's handlers. */
void EntityType23_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk04 = (void (*)(void))func_001384C0;
    unkE0->unk08 = (void (*)(void))func_001385B0;
    unkE0->unk18 = (void (*)(void))func_00138608;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    unkE0->unk10 = NULL;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->unk18 = NULL;
        unkE0->unk00 = func_001389C8;
    } else if (entity->spawn_info->unk40 == 2) {
        unkE0->unk00 = func_00138A48;
    } else if (entity->spawn_info->unk40 == 3) {
        unkE0->unk00 = func_00139730;
    } else if (entity->spawn_info->unk40 == 4) {
        unkE0->unk00 = func_00139FE0;
    } else if (entity->spawn_info->unk40 == 5) {
        unkE0->unk00 = func_0013A890;
    } else if (entity->spawn_info->unk40 == 6) {
        unkE0->unk00 = func_0013B150;
        unkE0->unk18 = (void (*)(void))func_00183570;
        entity->model->position[0] = -16200.0f;
        entity->model->position[1] = 3030.0f;
        entity->model->position[2] = 4900.0f;
        entity->unkD6 = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        unkE0->unk7A = 1;
    } else {
        unkE0->unk00 = func_00182C80;
        unkE0->unk18 = (void (*)(void))func_00183570;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
    unkE0->unk40 = 0.5f;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
    unkE0->unk48 = entity->spawn_info->spawn_x;
    unkE0->unk4C = entity->spawn_info->spawn_y;
    unkE0->unk50 = entity->spawn_info->spawn_z;
}

static void func_001384C0(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, (s32)(flags >> 11) & 7);
        }
    }
}

static void func_001385B0(Entity *entity) {
    func_00190A30((char *)entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (((entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}

static void func_00138608(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn = 0.0f;

    if (entity->unkC8 & 0x41) {
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
            unkE0->unk1C = turn;
        }
    } else {
        unkE0->unk1C = turn;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkD1 != 3 && entity->unkD1 != 4) {
            if (entity->unkC8 & 0x40) {
                func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 3, 0, entity->params->unkC4, 1);
            }
        } else if (entity->unkC8 & 0x40) {
            entity->unkD1 = 4;
            entity->model->unkFC->unk8C = entity->params->unk80[entity->unkD1];
        } else {
            entity->unkD1 = 3;
            entity->model->unkFC->unk8C = entity->params->unk80[entity->unkD1];
        }
    } else if (entity->unkC8 & 0x100) {
        func_00192C98(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x200) {
        func_00192C98(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
    entity->unkC6 = 0;
    entity->unkC8 = 0;
    entity->unkA0 = 0;
}

static void func_001387C0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 frames;

    entity->unkC6 = 0x1000;
    switch (entity->action_step) {
    case 0:
        if (func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14) >= 0.0f) {
            func_00192C98(entity, 0xC, 0, entity->params->unkC4, 1);
        } else {
            func_00192C98(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            frames = func_0019A3F0(entity, entity->unkD1);
            entity->unkA0 = frames;
            if (entity->unk28 != 0) {
                entity->unkA0 = frames * 2;
            }
            entity->action_step++;
        }
        break;
    case 2:
        func_00195388(&entity->model->unkD0[1], entity->spawn_info->unk14, entity->unkA0);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk48 = entity->model->position[0];
            unkE0->unk4C = entity->model->position[1];
            unkE0->unk50 = entity->model->position[2];
            entity->prev_state = entity->state;
            entity->state = 0;
            unkE0->unk00(entity, 1);
        }
        break;
    }
}

static void func_00138970(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00192C98(entity, 1, 0, entity->params->unkC4, 0);
    unkE0->unk1C = 0.0f;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
}

static void func_001389C8(Entity *entity, s32 unknown) {
    s64 flags;

    func_00192C98(entity, 0x11, 0, 0, unknown);
    flags = entity->flags & ~0x3800;
    flags |= 0x400000;
    flags |= 0x800000;
    flags |= 0x01000000;
    flags |= 0x40000;
    flags |= 0x200000;
    flags |= 0x10000000;
    flags |= 0x10000;
    entity->flags = flags;
}

static void func_00138A48(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[0] > -9700.0f && D_00413F80.position[0] < -700.0f &&
            D_00413F80.position[2] > -4800.0f && D_00413F80.position[2] < -3600.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            func_001958B8(entity, func_00138970, unknown);
            unkE0->unk68 = entity->params->unk430;
            break;
        case 1:
            if (unkE0->unk68 == 0) {
                func_001958B8(entity, func_00138D50, unknown);
                unkE0->unk6A = entity->params->unk432;
            }
            break;
        case 2:
            if (unkE0->unk68 == 0) {
                func_001958B8(entity, func_00139010, unknown);
                unkE0->unk6A = entity->params->unk432;
            }
            break;
        case 3:
            if (unkE0->unk68 == 0) {
                func_001958B8(entity, func_00139538, unknown);
            }
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk68 > 0) {
            unkE0->unk68--;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < -8400.0f) {
        entity->model->position[0] = -8400.0f;
    }
    if (entity->model->position[0] > -1950.0f) {
        entity->model->position[0] = -1950.0f;
    }
    if (entity->model->position[2] > -4000.0f - entity->unk30) {
        entity->model->position[2] = -4000.0f - entity->unk30;
    }
    if (entity->model->position[0] < -3150.0f) {
        if (entity->model->position[2] < entity->unk30 + -4800.0f) {
            entity->model->position[2] = entity->unk30 + -4800.0f;
        }
    }
}

static void func_00138D50(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    f32 facing;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    if (D_00413F80.position[0] < -6000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = 1.5707964f;
    } else if (D_00413F80.position[0] > -4350.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = -1.5707964f;
    } else if (D_00413F80.position[2] < -4000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = entity->unk30 + -4800.0f;
        post[3] = 0.0f;
        facing = 0.0f;
    } else {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4000.0f - entity->unk30;
        post[3] = 0.0f;
        facing = 3.1415927f;
    }
    if (entity->model->position[2] < -4800.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(entity, 1, 50.0f);
    } else if (func_00194F80(entity->model->position, post) < 100.0f) {
        angle = func_001957B8(entity->model->unkD0[1], facing);
        if (ABS(angle) > 0.5235988f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        func_00216778(unkE0->unk30, post);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_00139010(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < -8400.0f) {
        unkE0->unk30[0] = entity->unk30 + -8400.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > -1950.0f) {
        unkE0->unk30[0] = -1950.0f - entity->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[2] > -4000.0f) {
        unkE0->unk30[0] = -5200.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -4000.0f - entity->unk30;
        unkE0->unk30[3] = 0.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[2] < -4800.0f && D_00413F80.position[0] > -5600.0f &&
               D_00413F80.position[0] < -4800.0f) {
        unkE0->unk30[0] = -5200.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = entity->unk30 + -4800.0f;
        unkE0->unk30[3] = 0.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_00139538(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->unk18();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    if (entity->model->position[0] < -3150.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    } else if (func_00194F80(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            func_001958B8(entity, func_001387C0, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_00139730(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[2] > -6000.0f && D_00413F80.position[2] < -2800.0f &&
            entity->model->position[2] >= -4800.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            func_001958B8(entity, func_00138970, unknown);
            break;
        case 1:
            func_001958B8(entity, func_00139960, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 2:
            func_001958B8(entity, func_00139AE8, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 3:
            func_001958B8(entity, func_00139DE0, unknown);
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < 4800.0f) {
        entity->model->position[0] = 4800.0f;
    }
    if (entity->model->position[0] > 6650.0f - entity->unk30) {
        entity->model->position[0] = 6650.0f - entity->unk30;
    }
    if (entity->model->position[2] > -4000.0f - entity->unk30) {
        entity->model->position[2] = -4000.0f - entity->unk30;
    }
}

static void func_00139960(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = -4400.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (func_00194F80(entity->model->position, home) < range) {
        angle = func_001957B8(entity->model->unkD0[1], 3.1415927f);
        if (ABS(angle) > 1.5707964f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        func_00216778(unkE0->unk30, home);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_00139AE8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < 4800.0f) {
        unkE0->unk30[0] = 4800.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > 6650.0f) {
        unkE0->unk30[0] = 6650.0f - entity->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = func_00190F90(entity, 0, entity->radius + range + 30.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_00139DE0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->unk18();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    if (entity->model->position[0] < 5450.0f || entity->model->position[0] > 6150.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    } else if (func_00194F80(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            func_001958B8(entity, func_001387C0, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_00139FE0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[2] > -6000.0f && D_00413F80.position[2] < -2800.0f &&
            entity->model->position[2] <= -4000.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            func_001958B8(entity, func_00138970, unknown);
            break;
        case 1:
            func_001958B8(entity, func_0013A210, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 2:
            func_001958B8(entity, func_0013A398, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 3:
            func_001958B8(entity, func_0013A690, unknown);
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < entity->unk30 + 6150.0f) {
        entity->model->position[0] = entity->unk30 + 6150.0f;
    }
    if (entity->model->position[0] > 8000.0f) {
        entity->model->position[0] = 8000.0f;
    }
    if (entity->model->position[2] < entity->unk30 + -4800.0f) {
        entity->model->position[2] = entity->unk30 + -4800.0f;
    }
}

static void func_0013A210(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = -4400.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (func_00194F80(entity->model->position, home) < range) {
        angle = func_001957B8(entity->model->unkD0[1], 3.1415927f);
        if (ABS(angle) > 1.5707964f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        func_00216778(unkE0->unk30, home);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_0013A398(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < 6150.0f) {
        unkE0->unk30[0] = entity->unk30 + 6150.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > 8000.0f) {
        unkE0->unk30[0] = 8000.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = func_00190F90(entity, 0, entity->radius + range + 30.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_0013A690(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->unk18();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    if (entity->model->position[0] < 6650.0f || entity->model->position[0] > 7350.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    } else if (func_00194F80(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            func_001958B8(entity, func_001387C0, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_0013A890(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[0] > -20300.0f && D_00413F80.position[0] < -13350.0f &&
            D_00413F80.position[2] > -250.0f && D_00413F80.position[2] < 1800.0f &&
            entity->model->position[2] <= 950.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            func_001958B8(entity, func_00138970, unknown);
            break;
        case 1:
            func_001958B8(entity, func_0013AAD8, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 2:
            func_001958B8(entity, func_0013AC60, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 3:
            func_001958B8(entity, func_0013AF48, unknown);
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < -18300.0f) {
        entity->model->position[0] = -18300.0f;
    }
    if (entity->model->position[0] > -16050.0f) {
        entity->model->position[0] = -16050.0f;
    }
    if (entity->model->position[2] < 600.0f) {
        entity->model->position[2] = 600.0f;
    }
}

static void func_0013AAD8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->model->position[1];
    home[2] = 600.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (func_00194F80(entity->model->position, home) < range) {
        angle = func_001957B8(entity->model->unkD0[1], 0.0f);
        if (ABS(angle) > 1.5707964f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        func_00216778(unkE0->unk30, home);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_0013AC60(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < -18300.0f) {
        unkE0->unk30[0] = -18300.0f;
        unkE0->unk30[1] = entity->model->position[1];
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = 600.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > -16050.0f) {
        unkE0->unk30[0] = -16050.0f;
        unkE0->unk30[1] = entity->model->position[1];
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = 600.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = func_00190F90(entity, 0, entity->radius + range + 30.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_0013AF48(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->unk18();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    if (entity->model->position[0] < -17900.0f || entity->model->position[0] > -17200.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->model->position[1];
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = 600.0f;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    } else if (func_00194F80(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            func_001958B8(entity, func_001387C0, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}

static void func_0013B150(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    MapObject *map_object;
    s32 first_open;
    s32 second_open;
    s32 both_open;

    first_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
    if (map_object != NULL) {
        first_open = (map_object->unk10 >> 17) & 1;
    }
    second_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        second_open = (map_object->unk10 >> 17) & 1;
    }
    both_open = first_open && second_open;
    if (entity->unkC6 != 0) {
        return;
    }
    if (!both_open && unkE0->unk7A != 0) {
        if (func_00194F80(entity->spawn_info, &D_00413F80) > 5400.0f) {
            func_00216778(entity->model->position, entity->spawn_info);
            unkE0->unk7A = 0;
        }
    }
    switch (unkE0->unk7A) {
    case 1:
        if (!both_open) {
            unkE0->unk7A++;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        } else {
            func_0013B4C8(entity, unknown);
        }
        break;
    case 2:
        if (!both_open) {
            if (!func_0013BF48(entity, 3) || !func_0013BF48(entity, 6) || !func_0013BF48(entity, 7)) {
                func_0013B568(entity, unknown);
            } else {
                unkE0->unk7A++;
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
            }
        } else {
            unkE0->unk7A = 1;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        }
        break;
    case 3:
        if (!both_open) {
            func_0013B608(entity, unknown);
        } else {
            unkE0->unk7A = 7;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        }
        break;
    case 4:
        post[0] = -18450.0f;
        post[1] = 3190.0f;
        post[2] = 4900.0f;
        unkE0->unk54 = func_00194F80(entity->model->position, post);
        unkE0->unk7A++;
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        break;
    case 5:
        if (!both_open) {
            unkE0->unk7A = 0;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        } else if (func_0013BF48(entity, 3) && func_0013BF48(entity, 6) && func_0013BF48(entity, 7) &&
                   func_0013C018(entity, unkE0->unk54)) {
            unkE0->unk7A++;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        } else {
            func_0013B9E8(entity, unknown);
        }
        break;
    case 6:
        if (!both_open) {
            unkE0->unk7A = 0;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        } else {
            func_0013BA70(entity, unknown);
        }
        break;
    case 7:
        if (!both_open) {
            unkE0->unk7A = 3;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        } else {
            func_0013BE18(entity, unknown);
        }
        break;
    default:
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        entity->unkD6 = 0x80;
        if (both_open) {
            unkE0->unk7A = 4;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        } else {
            func_00182C80(entity, unknown);
        }
        break;
    }
}

static void func_0013B4C8(Entity *entity, s32 unknown) {
    EntityModel *model;

    model = entity->model;
    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->unkD0[1] = 1.5707964f;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    func_001958B8(entity, (void (*)(Entity *))entity->unkE0.unk18, unknown);
}

static void func_0013B568(Entity *entity, s32 unknown) {
    EntityModel *model = entity->model;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->unkD0[1] = 1.5707964f;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    func_001958B8(entity, (void (*)(Entity *))entity->unkE0.unk18, unknown);
}

static void func_0013B608(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -18050.0f;
    unkE0->unk30[1] = 3030.0f;
    unkE0->unk30[2] = 4900.0f;
    if (entity->model->position[1] > 3031.0f) {
        unkE0->unk7A = 0;
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    } else {
        point = unkE0->unk30;
        if (func_00194F80(entity->model->position, point) < range) {
            ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
            angle = func_001957B8(entity->model->unkD0[1], 1.5707964f);
            if (ABS(angle) > 0.17453292f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                func_001958B8(entity, func_0013B828, unknown);
                return;
            }
        } else {
            entity->target = point;
            entity->unkC8 = func_00190F90(entity, 0, range);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0013B828(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    entity->unkD6 = 0x80;
    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 9, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            unkE0->unk7A = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0013B9E8(Entity *entity, s32 unknown) {
    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0x80;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    func_001958B8(entity, (void (*)(Entity *))entity->unkE0.unk18, unknown);
}

static void func_0013BA70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -18450.0f;
    unkE0->unk30[1] = 3190.0f;
    unkE0->unk30[2] = 4900.0f;
    if (entity->model->position[1] <= 3030.0f) {
        unkE0->unk7A = 7;
    } else {
        point = unkE0->unk30;
        if (func_00194F80(entity->model->position, point) < range) {
            angle = func_001957B8(entity->model->unkD0[1], -1.5707964f);
            if (ABS(angle) > 0.17453292f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                func_001958B8(entity, func_0013BC60, unknown);
                return;
            }
        } else {
            entity->target = point;
            entity->unkC8 = func_00190F90(entity, 0, range);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0013BC60(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    entity->unkD6 = 0x80;
    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            unkE0->unk7A = 7;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0013BE18(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point = unkE0->unk30;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -16200.0f;
    unkE0->unk30[1] = 3030.0f;
    unkE0->unk30[2] = 4900.0f;
    if (func_00194F80(entity->model->position, point) < range) {
        unkE0->unk7A = 1;
    } else {
        entity->target = point;
        entity->unkC8 = func_00190F90(entity, 0, range);
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static s32 func_0013BF48(Entity *entity, s32 value) {
    EntityIdTable table = D_0043E890;
    s32 i;
    Entity *other;

    for (i = 0; i < 5; i++) {
        other = Entity_GetBySpawnId(table.ids[i]);
        if (other != NULL && (other->flags & 0xD) == 1 && other != entity && other->unkE0.unk7A == value) {
            return FALSE;
        }
    }
    return TRUE;
}

static s32 func_0013C018(Entity *entity, f32 limit) {
    EntityIdTable table = D_0043E890;
    s32 i;
    Entity *other;
    EntityUnkE0 *other_unkE0;

    for (i = 0; i < 5; i++) {
        other = Entity_GetBySpawnId(table.ids[i]);
        if (other != NULL && (other->flags & 0xD) == 1 && other != entity) {
            other_unkE0 = &other->unkE0;
            if (other_unkE0->unk7A == 5 && other_unkE0->unk54 < limit) {
                return FALSE;
            }
        }
    }
    return TRUE;
}
