#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_game.h"
#include "kf_magic_ladder.h"
#include "kf_map_object.h"
#include "kf_effect.h"

#define IS_PLAYER(owner) ((owner) == (Entity *)&player_state)
#define LARGER(first, second) ((first) < (second) ? (second) : (first))
/* rand() scaled to 0-99 (the factor is 2 to the power -31) */
#define ROLL_PERCENT() ((s32)(rand() * 100.0f * 0.0000000004656613f))
/* a chance is never under 5 percent */
#define AT_LEAST_5(chance) ((chance) < 5 ? 5 : (chance))
#define ENTITY_TYPE_LAST_BOSS 99
/* EntityParams.unk238 as six signed bytes: tenths the six magic strengths of an entity's hit are scaled by */
#define ENTITY_HIT_SCALES(entity) ((s8 *)&(entity)->params->unk238)
/* store a strength as a whole number, rounded up when more than a thousandth is left over */
#define STORE_ROUNDED_UP(dest, value) \
    strength = (value); \
    whole = strength; \
    (dest) = whole; \
    if (strength - whole > 0.001f) { \
        (dest) = whole + 1; \
    }
#define SHOT_REQUEST ((Unk1B1470Request *)shot->unk04)

s32 Attack_RunSphere(Unk1B1470Request *, Unk1B1470Extra *);
int func_001B1B80(void);
int func_001B1B88(void);
s32 Attack_RunSwept(Unk1B1470Request *, Unk1B1470Extra *);
void Attack_HitPlayer(Unk1B1470Request *, Unk1B1470Extra *);
void Attack_HitEntity(Unk1B1470Request *, Unk1B1470Extra *, Entity *, f32);
void Attack_HitMapObject(Unk1B1470Request *, Unk1B1470Extra *, MapObject *);
int Attack_HitMap(Unk1B1470Request *, Unk1B1470Extra *);
void Collision_HitSphereObj(MapObject *, f32 *, f32);
void Collision_SphereCylinder(void *, void *, f32, f32, f32);
float sqrtf(float);
void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
void Vector_Copy(void *, void *);
void Collision_CastSegmentAtMap(f32 *, f32 *);
void Collision_HitRayObj(MapObject *, f32 *, f32 *, f32);
void Collision_CylinderArc(void *, void *, f32, f32, f32, f32, f32, f32);
void Collision_CylinderArcPitch(void *, void *, f32, f32, f32, f32, f32, f32, f32, f32, f32);
void Vu0_LoadMatrix(void *);
u32 Collision_GetFloorMaterial(int, u32, u32);
void Attack_RollPlayerAilments(void *);
void Attack_GetEffectiveStrengths(EntityParamsUnk240PartUnk00 *, Unk1B1470Request *, Unk1B1470Extra *);
int Attack_StrengthDamageToPlayer(int, int, int);
void Attack_WearArmorFromHit(int, Unk1B1470Request *);
void Player_TakeDamage(int);
void Player_KnockBackFromHit(void *, int);
void Player_TiltViewFromHit(int);
int rand(void);
int printf(const char *, ...);
float atan2f(float, float);
int Attack_StrengthDamage(int, int, int, int);
void Attack_RollEntityAilments(Entity *, EntityParamsUnk240PartUnk12 *);
void Attack_WearWeapon(int, int);
void Player_KnockBack(Vector4 *, f32);
void Player_CountMagicUse(int);
void Player_CountWeaponUse(int);
void Player_BounceWeapon(void);
void Player_BounceWeaponQuietly(void);
s32 Sound_PlayCommon(s32);
void Player_StrikeSolidObject(int with_sound);
void Attack_WearWornItem(int idx, int amount);
int File_Open(char *, int, int);
int File_Read(int, void *, int);
int File_Close(int);
void *memset(void *, int, u32);
s32 Attack_Run(Unk1B1470Request *, Unk1B1470Extra *);

// match: declared without a size, so it is not addressed through $gp
extern CollisionListPick2 map_hit_floor_strip_lists[];
// match: declared without a size, so it is not addressed through $gp
extern CollisionListPick2 map_hit_wall_strip_lists[];
extern PlayerState player_state;
extern Entity *active_entities_head;
extern MapSlot map_slots[2];
extern s32 map_object_counts[2];
extern MapObject map_slot_objects[][0x200];
extern PlayerArmPoints held_item_arm_points;
extern PlayerArmPoints held_item_arm_points_prev;
extern CollisionListPick4 map_hit_strip_lists;
extern const VuVector4 y_axis_vector;
extern GameStatus game_status;
extern Entity entity_pool[][0x80];
extern u32 frame_counter;
extern Entity entity_pool_last;           /* the last entity of entity_pool */
extern char last_boss_hits_format[];           /* "g_dmgLassBossCnt[%d]\n" */
extern char hp_halved_format[];           /* "HP halved!! %d / 2 = %d" in Japanese */
extern s32 dark_one_hits_left;              /* g_dmgLassBossCnt: weapon hits the last boss still takes */
extern PlayerItemDef weapon_defs[];
extern LevelDef weapon_name_defs[];
extern LevelRecord weapon_level_records[];
extern LevelRecord worn_item_level_records[];
extern LevelDef worn_name_defs[];
extern char magic_param_path[];

int Attack_TestMapModel(ModelInstance *, void *, f32);
int Attack_TestMapModelFloors(ModelInstance *, void *, f32);
int Attack_TestMapModelWalls(ModelInstance *, void *, f32);

/* 0x402D88 the three hit checks of a swing, one per kind of collision shape class */
int (*swing_hit_checks[3])(ModelInstance *, void *, f32) = { Attack_TestMapModel, Attack_TestMapModelFloors, Attack_TestMapModelWalls };
u32 D_00402D98[4] = { 0x18, 8, 0x10, 0 }; /* 0x402D98 four words after the table, which nothing reads */
/* 0x402DA8 the effect table, 0x110 bytes per effect id, which Attack_LoadMagicTable loads from its file */
FxTableEntry fx_table[256] = { 0 };

/* Runs an attack request against the world, by the request's shape (0 to 3). Only shapes
   0 and 3 do anything. */
s32 Attack_Run(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    COLLISION_RESULT->unk18 = 6;
    switch ((u8)((Unk1B1470RequestWord *)request)->flags & 3) {
    case 0:
        return Attack_RunSphere(request, extra);
    case 1:
        return func_001B1B80();
    case 2:
        return func_001B1B88();
    case 3:
        return Attack_RunSwept(request, extra);
    }
    return 0;
}

/* Runs a sphere-shaped attack request against everything it may hit, in this order: the
   map objects (all of them, the first hit is kept aside), the player, the entities and
   the map's own collision meshes. Returns what was hit: 1 the player, 2 an entity, 4 a
   map object, 8, 0x10 or 0x20 the map (by the kind of surface), 0 nothing. */
s32 Attack_RunSphere(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    CollisionResultCopy object_hit;
    s32 object_result = 0;
    Entity *entity;
    EntityBodyPoint *body;
    MapObject *object;
    MapRecordBounds *record;
    void *point;
    f32 scale;
    f32 reach;
    s32 index; /* a map, or one of an entity's bodies */
    s32 count;
    s32 item; /* a map object or a map record */
    s32 kind;
    s32 result;

    if (request->unk4D & 4) {
        for (index = 0; index < 2; index++) {
            if (map_slots[index].unk00 == 0) {
                continue;
            }
            count = map_object_counts[index];
            object = map_slot_objects[index];
            for (item = 0; item < count; item++, object++) {
                if (!(object->unk10 & 0x10000)) {
                    continue;
                }
                if ((request->unk4D & 0x80) && request->owner == (Entity *)object) {
                    continue;
                }
                Collision_HitSphereObj(object, request->position, request->unk30);
                if (COLLISION_RESULT->hit != 0) {
                    if (object_result == 0) {
                        object_result = 4;
                        // match: kept aside a doubleword at a time, each through *& (retail forms the address anew for every load)
                        object_hit.doublewords[0] = *&COLLISION_RESULT_COPY->doublewords[0];
                        object_hit.doublewords[1] = *&COLLISION_RESULT_COPY->doublewords[1];
                        object_hit.doublewords[2] = *&COLLISION_RESULT_COPY->doublewords[2];
                        object_hit.doublewords[3] = *&COLLISION_RESULT_COPY->doublewords[3];
                        object_hit.doublewords[4] = *&COLLISION_RESULT_COPY->doublewords[4];
                        object_hit.doublewords[5] = *&COLLISION_RESULT_COPY->doublewords[5];
                    }
                    *&COLLISION_RESULT->hit = 0; // match: the address is formed anew
                    Attack_HitMapObject(request, extra, object);
                }
            }
        }
    }
    if (request->unk4D & 1) {
        if (!((request->unk4D & 0x80) && IS_PLAYER(request->owner))) {
            Collision_SphereCylinder(&player_state, request, 50.0f, 230.0f, request->unk30);
            if (COLLISION_RESULT->hit != 0) {
                Attack_HitPlayer(request, extra);
                return 1;
            }
        }
    }
    if (request->unk4D & 2) {
        for (entity = active_entities_head; entity != NULL; entity = entity->next) {
            if (!((s32)(entity->flags >> 1) & 1)) {
                continue;
            }
            if ((s32)(entity->flags >> 20) & 1) {
                continue;
            }
            if (((s32)(entity->flags >> 2) & 3) >= 2) {
                continue;
            }
            if ((request->unk4D & 0x80) && request->owner == entity) {
                continue;
            }
            if (!(entity->params->flags & 0x400)) {
                if (((EntityParamsWord *)entity->params)->unk00 & 0x8000000) {
                    Collision_SphereSphere(entity->model->position, request, entity->radius, request->unk30);
                } else {
                    Collision_SphereCylinder(entity->model->position, request, entity->radius, entity->height, request->unk30);
                }
                if (COLLISION_RESULT->hit != 0) {
                    Attack_HitEntity(request, extra, entity, 1.0f);
                    return 2;
                }
            } else {
                scale = LARGER(ENTITY_MODEL(entity)->unkE8, ENTITY_MODEL(entity)->unkE0);
                ENTITY_MODEL(entity)->unk1B4(ENTITY_MODEL(entity));
                body = entity->params->unk20;
                for (index = 0; index < 4; index++, body++) {
                    if (!(body->unk00.flags & ENTITY_BODY_USED)) {
                        continue;
                    }
                    point = (void *)ENTITY_MODEL(entity)->get_point(ENTITY_MODEL(entity), body->unk00.point);
                    if (!(body->unk00.flags & ENTITY_BODY_ROUND)) {
                        Collision_SphereCylinder(point, request, body->unk04 * scale, body->unk06 * ENTITY_MODEL(entity)->unkE4,
                                      request->unk30);
                    } else {
                        Collision_SphereSphere(point, request, body->unk04 * scale, request->unk30);
                    }
                    if (COLLISION_RESULT->hit != 0) {
                        Attack_HitEntity(request, extra, entity, body->unk08);
                        return 2;
                    }
                }
            }
        }
    }
    if ((UNK1B1470_REQUEST_UNIT(request) & (0xC000L << 29)) == (0xC000L << 29)) {
        kind = 0;
    } else if (request->unk4D & 8) {
        kind = 1;
    } else {
        kind = -1;
        if (request->unk4D & 0x10) {
            kind = 2;
        }
    }
    if (kind != -1) {
        for (index = 0; index < 2; index++) {
            if (map_slots[index].unk00 == 0) {
                continue;
            }
            record = (MapRecordBounds *)map_slots[index].records;
            count = map_slots[index].header->record_count;
            for (item = 0; item < count; item++, record++) {
                if (record->unk30 == 0xFFFF) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                reach = request->unk30;
                if (request->position[0] < record->lowest[0] - reach || record->highest[0] + reach < request->position[0] ||
                    request->position[1] < record->lowest[1] - reach || record->highest[1] + reach < request->position[1] ||
                    request->position[2] < record->lowest[2] - reach || record->highest[2] + reach < request->position[2]) {
                    continue;
                }
                result = swing_hit_checks[kind](record->model, request, reach);
                if (COLLISION_RESULT->hit != 0) {
                    // match: called as void, retail reuses $2 at once
                    ((void (*)(Unk1B1470Request *, Unk1B1470Extra *))Attack_HitMap)(request, extra);
                    switch (result) {
                    case 1:
                        result = 8;
                        break;
                    case 0:
                    case 2:
                        result = 0x10;
                        break;
                    case 5:
                        result = 0x20;
                        break;
                    default:
                        return result;
                    }
                    return result;
                }
            }
        }
    }
    if (object_result != 0) {
        // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
        *&COLLISION_RESULT_COPY->doublewords[0] = object_hit.doublewords[0];
        *&COLLISION_RESULT_COPY->doublewords[1] = object_hit.doublewords[1];
        *&COLLISION_RESULT_COPY->doublewords[2] = object_hit.doublewords[2];
        *&COLLISION_RESULT_COPY->doublewords[3] = object_hit.doublewords[3];
        *&COLLISION_RESULT_COPY->doublewords[4] = object_hit.doublewords[4];
        *&COLLISION_RESULT_COPY->doublewords[5] = object_hit.doublewords[5];
        return object_result;
    }
    return 0;
}

/* Attack shape 1: does nothing, returns 0. */
int func_001B1B80(void) {
    return 0;
}

/* Attack shape 2: does nothing, returns 0. */
int func_001B1B88(void) {
    return 0;
}

/* Runs a swept attack request (shape 3) against everything it may hit: the player, the
   entities and, for the player's own weapon, the map objects and the map along the two
   edges the weapon moved through. Returns what was hit: 1 the player, 2 an entity, 4 a
   map object, 0x10 the map, 0 nothing. */
s32 Attack_RunSwept(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    f32 tip[4];
    f32 direction[4];
    Entity *entity;
    EntityBodyPoint *body;
    MapObject *object;
    void *point;
    f32 width_scale;
    f32 scale;
    f32 length;
    s32 index; /* a map, or one of an entity's bodies */
    s32 count;
    s32 item;

    if (request->unk4D & 1) {
        if (!((request->unk4D & 0x80) && IS_PLAYER(request->owner))) {
            Collision_CylinderArc(&player_state, request, 50.0f, 230.0f, request->unk30, request->unk34, request->unk3C,
                          request->unk38);
            if (COLLISION_RESULT->hit != 0) {
                Attack_HitPlayer(request, extra);
                return 1;
            }
        }
    }
    if (request->unk4D & 2) {
        for (entity = active_entities_head; entity != NULL; entity = entity->next) {
            if (!((s32)(entity->flags >> 1) & 1)) {
                continue;
            }
            if ((s32)(entity->flags >> 20) & 1) {
                continue;
            }
            if (((s32)(entity->flags >> 2) & 3) >= 2) {
                continue;
            }
            if ((request->unk4D & 0x80) && request->owner == entity) {
                continue;
            }
            if (!(entity->params->flags & 0x400)) {
                if (IS_PLAYER(request->owner)) {
                    Collision_CylinderArcPitch(entity->model->position, request, entity->radius, entity->height, request->unk30,
                                  request->unk78, request->unk7C, request->unk3C, request->unk38, request->unk44,
                                  request->unk40);
                } else {
                    Collision_CylinderArc(entity->model->position, request, entity->radius, entity->height, request->unk30,
                                  request->unk34, request->unk3C, request->unk38);
                }
                if (COLLISION_RESULT->hit != 0) {
                    Attack_HitEntity(request, extra, entity, 1.0f);
                    return 2;
                }
            } else {
                width_scale = LARGER(ENTITY_MODEL(entity)->unkE8, ENTITY_MODEL(entity)->unkE0);
                scale = ENTITY_MODEL(entity)->unkE0;
                if (scale < ENTITY_MODEL(entity)->unkE4) {
                    scale = ENTITY_MODEL(entity)->unkE4;
                }
                if (scale < ENTITY_MODEL(entity)->unkE8) {
                    scale = ENTITY_MODEL(entity)->unkE8;
                }
                ENTITY_MODEL(entity)->unk1B4(ENTITY_MODEL(entity));
                body = entity->params->unk20;
                for (index = 0; index < 4; index++, body++) {
                    if (!(body->unk00.flags & ENTITY_BODY_USED)) {
                        continue;
                    }
                    point = (void *)ENTITY_MODEL(entity)->get_point(ENTITY_MODEL(entity), body->unk00.point);
                    if (!(body->unk00.flags & ENTITY_BODY_ROUND)) {
                        if (IS_PLAYER(request->owner)) {
                            Collision_CylinderArcPitch(point, request, body->unk04 * width_scale, body->unk06 * ENTITY_MODEL(entity)->unkE4,
                                          request->unk30, request->unk78, request->unk7C, request->unk3C, request->unk38,
                                          request->unk44, request->unk40);
                        } else {
                            Collision_CylinderArc(point, request, body->unk04 * width_scale, body->unk06 * ENTITY_MODEL(entity)->unkE4,
                                          request->unk30, request->unk34, request->unk3C, request->unk38);
                        }
                    } else {
                        if (IS_PLAYER(request->owner)) {
                            Collision_BallSwingPitch(point, request, body->unk04 * scale, request->unk30, request->unk78, request->unk7C,
                                          request->unk3C, request->unk38, request->unk44, request->unk40);
                        } else {
                            Collision_BallSwing(point, request, body->unk04 * scale, request->unk30, request->unk34,
                                          request->unk3C, request->unk38);
                        }
                    }
                    if (COLLISION_RESULT->hit != 0) {
                        Attack_HitEntity(request, extra, entity, body->unk08);
                        return 2;
                    }
                }
            }
        }
    }
    if (!IS_PLAYER(request->owner)) {
        return 0;
    }
    if (request->unk4D & 4) {
        Vector_Copy(tip, request);
        tip[1] += request->unk34;
        for (index = 0; index < 2; index++) {
            if (map_slots[index].unk00 == 0) {
                continue;
            }
            count = map_object_counts[index];
            object = map_slot_objects[index];
            for (item = 0; item < count; item++, object++) {
                Vector_Subtract(direction, &held_item_arm_points.points[1], &held_item_arm_points.points[0]);
                length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
                Vector_Scale(direction, direction, 1.0f / length);
                Collision_HitRayObj(object, (f32 *)&held_item_arm_points.points[0], direction, length);
                if (COLLISION_RESULT->hit != 0) {
                    Attack_HitMapObject(request, extra, object);
                    return 4;
                }
                Vector_Subtract(direction, &held_item_arm_points.points[1], &held_item_arm_points_prev.points[1]);
                length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
                Vector_Scale(direction, direction, 1.0f / length);
                Collision_HitRayObj(object, (f32 *)&held_item_arm_points_prev.points[1], direction, length);
                if (COLLISION_RESULT->hit != 0) {
                    Attack_HitMapObject(request, extra, object);
                    return 4;
                }
            }
        }
    }
    if (!IS_PLAYER(request->owner)) {
        return 0;
    }
    Collision_CastSegmentAtMap((f32 *)&held_item_arm_points.points[0], (f32 *)&held_item_arm_points.points[1]);
    if (COLLISION_RESULT->hit != 0) {
        Attack_HitMap(request, extra);
        return 0x10;
    }
    Collision_CastSegmentAtMap((f32 *)&held_item_arm_points.points[1], (f32 *)&held_item_arm_points_prev.points[1]);
    if (COLLISION_RESULT->hit != 0) {
        Attack_HitMap(request, extra);
        return 0x10;
    }
    return 0;
}

/* Tests a hit's sphere against four of a map model's strip lists (0, 1, 2 and 5). When it
   touches a triangle the result is left in the scratchpad and the list's number returned. */
int Attack_TestMapModel(ModelInstance *model, void *request, f32 reach) {
    CollisionListPick4 lists = map_hit_strip_lists;
    f32 unused_work[8]; // match: never used, keeps the retail stack frame size
    VuVector4 unused_axis = y_axis_vector; // match: copied and never read
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    Vector4 *world = COLLISION_WORLD_POINTS;
    Vector4 *local;
    u32 count;
    s32 i;
    s32 list;
    s32 points_left;

    mesh = (CollisionMeshLists *)model->strips;
    COLLISION_RESULT->hit = 0;
    if (mesh == NULL) {
        return;
    }
    if (((CollisionMesh *)mesh)->unk0C < 4) {
        return;
    }
    Vu0_LoadMatrix(model->unk40);
    count = ((CollisionMesh *)mesh)->unk0C;
    local = (Vector4 *)((CollisionMesh *)mesh)->unk08;
    i = count / 3;
    do {
        __asm__ volatile("pref 0, 0x30(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%3)\n"
            "lqc2 $vf17, 0x0(%4)\n"
            "lqc2 $vf18, 0x0(%5)\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vmulax.xyzw ACC, $vf1, $vf17x\n"
            "vmadday.xyzw ACC, $vf2, $vf17y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf17z\n"
            "vmaddw.xyzw $vf17, $vf4, $vf17w\n"
            "vmulax.xyzw ACC, $vf1, $vf18x\n"
            "vmadday.xyzw ACC, $vf2, $vf18y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf18z\n"
            "vmaddw.xyzw $vf18, $vf4, $vf18w\n"
            "sqc2 $vf16, 0x0(%0)\n"
            "sqc2 $vf17, 0x0(%1)\n"
            "vnop\n"
            "sqc2 $vf18, 0x0(%2)\n"
            : : "r"(world), "r"(world + 1), "r"(world + 2), "r"(local), "r"(local + 1), "r"(local + 2));
        local += 3;
        world += 3;
    } while (--i != 0);
    for (i = count % 3 - 1; i != -1; i--) {
        __asm__ volatile("pref 0, 0x10(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%0)\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "sqc2 $vf16, 0x0(%1)\n"
            : : "r"(local), "r"(world));
        local++;
        world++;
    }
    for (list = 0; list < 4; list++) {
        strip = mesh->strip_lists[lists.lists[list]];
        i = mesh->strip_counts[lists.lists[list]];
        while (--i != -1) {
            strip_point = strip->points;
            points_left = strip->point_count - 2;
            do {
                Collision_PointTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position, (int)request, reach);
                if (COLLISION_RESULT->hit != 0) {
                    COLLISION_RESULT->unk18 = Collision_GetFloorMaterial(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                    return lists.lists[list];
                }
                strip_point++;
            } while (--points_left != 0);
            strip = (CollisionStrip *)&strip_point[2];
        }
    }
}

/* Attack_TestMapModel for strip lists 1 and 5 only: returns 1 when the sphere touches a triangle
   of them (the result is left in the scratchpad), 0 when it touches none. */
int Attack_TestMapModelFloors(ModelInstance *model, void *request, f32 reach) {
    CollisionListPick2 lists = map_hit_floor_strip_lists[0];
    f32 unused_work[8]; // match: never used, keeps the retail stack frame size
    VuVector4 unused_axis = y_axis_vector; // match: copied and never read
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    Vector4 *world = COLLISION_WORLD_POINTS;
    Vector4 *local;
    u32 count;
    s32 i;
    s32 list;
    s32 points_left;

    mesh = (CollisionMeshLists *)model->strips;
    COLLISION_RESULT->hit = 0;
    if (mesh == NULL) {
        return;
    }
    if (((CollisionMesh *)mesh)->unk0C < 4) {
        return;
    }
    Vu0_LoadMatrix(model->unk40);
    count = ((CollisionMesh *)mesh)->unk0C;
    local = (Vector4 *)((CollisionMesh *)mesh)->unk08;
    i = count / 3;
    do {
        __asm__ volatile("pref 0, 0x30(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%3)\n"
            "lqc2 $vf17, 0x0(%4)\n"
            "lqc2 $vf18, 0x0(%5)\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vmulax.xyzw ACC, $vf1, $vf17x\n"
            "vmadday.xyzw ACC, $vf2, $vf17y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf17z\n"
            "vmaddw.xyzw $vf17, $vf4, $vf17w\n"
            "vmulax.xyzw ACC, $vf1, $vf18x\n"
            "vmadday.xyzw ACC, $vf2, $vf18y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf18z\n"
            "vmaddw.xyzw $vf18, $vf4, $vf18w\n"
            "sqc2 $vf16, 0x0(%0)\n"
            "sqc2 $vf17, 0x0(%1)\n"
            "vnop\n"
            "sqc2 $vf18, 0x0(%2)\n"
            : : "r"(world), "r"(world + 1), "r"(world + 2), "r"(local), "r"(local + 1), "r"(local + 2));
        local += 3;
        world += 3;
    } while (--i != 0);
    for (i = count % 3 - 1; i != -1; i--) {
        __asm__ volatile("pref 0, 0x10(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%0)\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "sqc2 $vf16, 0x0(%1)\n"
            : : "r"(local), "r"(world));
        local++;
        world++;
    }
    for (list = 0; list < 2; list++) {
        strip = mesh->strip_lists[lists.lists[list]];
        i = mesh->strip_counts[lists.lists[list]];
        while (--i != -1) {
            strip_point = strip->points;
            points_left = strip->point_count - 2;
            do {
                Collision_PointTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position, (int)request, reach);
                if (COLLISION_RESULT->hit != 0) {
                    COLLISION_RESULT->unk18 = Collision_GetFloorMaterial(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                    return 1;
                }
                strip_point++;
            } while (--points_left != 0);
            strip = (CollisionStrip *)&strip_point[2];
        }
    }
    return 0;
}

/* Attack_TestMapModel for strip lists 0 and 2 only: returns 0 when the sphere touches a triangle
   of them (the result is left in the scratchpad). */
int Attack_TestMapModelWalls(ModelInstance *model, void *request, f32 reach) {
    CollisionListPick2 lists = map_hit_wall_strip_lists[0];
    f32 unused_work[8]; // match: never used, keeps the retail stack frame size
    VuVector4 unused_axis = y_axis_vector; // match: copied and never read
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    Vector4 *world = COLLISION_WORLD_POINTS;
    Vector4 *local;
    u32 count;
    s32 i;
    s32 list;
    s32 points_left;

    mesh = (CollisionMeshLists *)model->strips;
    COLLISION_RESULT->hit = 0;
    if (mesh == NULL) {
        return;
    }
    if (((CollisionMesh *)mesh)->unk0C < 4) {
        return;
    }
    Vu0_LoadMatrix(model->unk40);
    count = ((CollisionMesh *)mesh)->unk0C;
    local = (Vector4 *)((CollisionMesh *)mesh)->unk08;
    i = count / 3;
    do {
        __asm__ volatile("pref 0, 0x30(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%3)\n"
            "lqc2 $vf17, 0x0(%4)\n"
            "lqc2 $vf18, 0x0(%5)\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vmulax.xyzw ACC, $vf1, $vf17x\n"
            "vmadday.xyzw ACC, $vf2, $vf17y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf17z\n"
            "vmaddw.xyzw $vf17, $vf4, $vf17w\n"
            "vmulax.xyzw ACC, $vf1, $vf18x\n"
            "vmadday.xyzw ACC, $vf2, $vf18y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf18z\n"
            "vmaddw.xyzw $vf18, $vf4, $vf18w\n"
            "sqc2 $vf16, 0x0(%0)\n"
            "sqc2 $vf17, 0x0(%1)\n"
            "vnop\n"
            "sqc2 $vf18, 0x0(%2)\n"
            : : "r"(world), "r"(world + 1), "r"(world + 2), "r"(local), "r"(local + 1), "r"(local + 2));
        local += 3;
        world += 3;
    } while (--i != 0);
    for (i = count % 3 - 1; i != -1; i--) {
        __asm__ volatile("pref 0, 0x10(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%0)\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "sqc2 $vf16, 0x0(%1)\n"
            : : "r"(local), "r"(world));
        local++;
        world++;
    }
    for (list = 0; list < 2; list++) {
        strip = mesh->strip_lists[lists.lists[list]];
        i = mesh->strip_counts[lists.lists[list]];
        while (--i != -1) {
            strip_point = strip->points;
            points_left = strip->point_count - 2;
            do {
                Collision_PointTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position, (int)request, reach);
                if (COLLISION_RESULT->hit != 0) {
                    COLLISION_RESULT->unk18 = Collision_GetFloorMaterial(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                    return 0;
                }
                strip_point++;
            } while (--points_left != 0);
            strip = (CollisionStrip *)&strip_point[2];
        }
    }
}

/* A hit lands on the player: marks the attacking entity, works out the damage from the
   attack's nine strengths against the player's nine defences, takes it off the player's
   health and starts the ailments and the reaction. */
void Attack_HitPlayer(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    EntityParamsUnk240PartUnk00 attack;
    Entity *owner = request->owner;
    s32 blocked;
    s32 percent;
    s32 damage;

    if (owner >= entity_pool[0] && owner < (Entity *)entity_pool_slots) {
        if (extra->unk00 == 0) {
            owner->flags |= 0x8000L << 26;
        } else {
            owner->flags |= 0x8000L << 27;
        }
    }
    if (player_state.hp_now == 0) {
        return;
    }
    if (game_status.unk00 == 0) {
        return;
    }
    if (game_status.unk30 & 1) {
        return;
    }
    blocked = request->unk72;
    if (blocked != 0) {
        return;
    }
    percent = 100;
    if ((u8)(player_state.equipment[EQUIP_SHIELD] - 1) <= 1) {
        percent = 120;
    }
    if ((u8)(player_state.equipment[EQUIP_BOOT] - 0x6A) <= 1) {
        percent += 20;
    }
    Attack_GetEffectiveStrengths(&attack, request, extra); // match: attack is filled here and never read; the terms below use request->unk50
    damage = Attack_StrengthDamageToPlayer(request->unk50.unk00[0], player_state.unk90.values[1], player_state.power); // match: values[1] here and on the next line; values[0] is never used
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[1], player_state.unk90.values[1], player_state.power); // match: values[1] a second time
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[2], player_state.unk90.values[2], player_state.power);
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[3], player_state.unk90.values[3], player_state.magic) * percent / 100;
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[4], player_state.unk90.values[4], player_state.magic);
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[5], player_state.unk90.values[5], player_state.magic);
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[6], player_state.unk90.values[6], player_state.magic);
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[7], player_state.unk90.values[7], player_state.magic);
    damage += Attack_StrengthDamageToPlayer(request->unk50.unk00[8], player_state.unk90.values[8], player_state.magic);
    request->unk74 = 1;
    if (damage <= blocked) { // match: blocked is request->unk72, which is known to be 0 here
        damage = 1;
    }
    Player_TakeDamage(damage);
    Attack_RollPlayerAilments(&request->unk62);
    if (request->unk71 != 0) {
        if (request->unk71 < player_state.mp_now) {
            player_state.mp_now -= request->unk71;
        } else {
            player_state.mp_now = 0;
        }
    }
    if (UNK1B1470_REQUEST_UNIT(request) & UNK1B1470_UNIT_UNK4C_3) {
        Player_KnockBackFromHit(request, damage);
    }
    Player_TiltViewFromHit(damage);
    Attack_WearArmorFromHit(damage, request);
    Ladder_CancelApproach();
}

/* A hit may start the player's six timed ailments: each has a chance in percent in the
   attack, less the player's resistance to it but never under 5. One that is already
   running is left alone. */
void Attack_RollPlayerAilments(void *attack_chances) {
    EntityParamsUnk240PartUnk12 *chances = attack_chances;

    if ((u16)chances->unk00[0] != 0 && player_state.flag_timers[0] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[0] - player_state.unkDC[0])) {
            player_state.timed_flags[0] = 1800;
            player_state.unk2C4 = frame_counter;
        }
    }
    if ((u16)chances->unk00[1] != 0 && player_state.flag_timers[1] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[1] - player_state.unkDC[1])) {
            player_state.timed_flags[1] = 150;
            player_state.unk2C8 = frame_counter;
        }
    }
    if ((u16)chances->unk00[2] != 0 && player_state.flag_timers[2] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[2] - player_state.unkDC[2])) {
            player_state.timed_flags[2] = 3600;
            player_state.unk2CC = frame_counter;
        }
    }
    if ((u16)chances->unk00[3] != 0 && player_state.flag_timers[3] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[3] - player_state.unkDC[3])) {
            player_state.timed_flags[3] = 5400;
            player_state.unk2D0 = frame_counter;
        }
    }
    if ((u16)chances->unk00[4] != 0 && player_state.flag_timers[4] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[4] - player_state.unkDC[4])) {
            player_state.timed_flags[4] = 1800;
            player_state.unk2D4 = frame_counter;
        }
    }
    if ((u16)chances->unk00[5] != 0 && player_state.flag_timers[5] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[5] - player_state.unkDC[5])) {
            player_state.timed_flags[5] = 18000;
            player_state.unk2D8 = frame_counter;
        }
    }
}

/* A hit lands on an entity: work out the damage from the attack's nine strengths and the
   entity's nine resistances, take it off the entity's health and start its reaction.
   The last boss takes no damage here: a weapon hit counts dark_one_hits_left down, throws the
   player back and halves the player's health (PlayerState.hp_now). */
void Attack_HitEntity(Unk1B1470Request *request, Unk1B1470Extra *extra, Entity *entity, f32 scale) {
    Vector4 position;
    EntityParamsUnk240PartUnk00 attack;
    Entity *owner;
    s32 by_weapon;
    s32 by_magic;
    s32 damage;
    s32 physical_bonus;
    s32 magic_bonus;
    s32 chance;
    f32 percent;

    owner = request->owner;
    by_weapon = FALSE;
    if (IS_PLAYER(owner)) {
        by_weapon = extra->unk00 == 0;
    }
    by_magic = FALSE;
    if (IS_PLAYER(owner)) {
        by_magic = extra->unk00 == 1;
    }

    if (entity->spawn_info->type_id == ENTITY_TYPE_LAST_BOSS) {
        if (!IS_PLAYER(owner)) {
            return;
        }
        if (entity->unk80 != 0) {
            return;
        }
        entity->flags |= 0x8000L << 28;
        entity->unk80 = 15;
        printf(last_boss_hits_format, dark_one_hits_left);
        if (!by_weapon) {
            return;
        }
        if (extra->unk04 != 0) {
            return;
        }
        if (--dark_one_hits_left <= 0) {
            return;
        }
        Attack_WearWeapon(extra->unk24, 4);
        Player_BounceWeaponQuietly();
        Vector_Copy(&position, owner);
        if (player_state.position.x < entity->model->position[0]) {
            position.x += 100.0f;
            position.z += 100.0f;
        } else {
            position.x -= 100.0f;
            position.z += 100.0f;
        }
        Player_KnockBack(&position, 200.0f);
        if (player_state.hp_now < 2) {
            return;
        }
        printf(hp_halved_format, player_state.hp_now, player_state.hp_now / 2);
        Player_TakeDamage(player_state.hp_now / 2);
        return;
    }

    // match: the range test (not one of the entities) is always true for the player
    if (IS_PLAYER(owner) && (owner < entity_pool[0] || owner > &entity_pool_last)) {
        entity->flags |= 0x8000L << 28;
    }
    if (extra->unk00 == 1) {
        entity->unk82 = request->unk4F;
    }
    if ((s32)(entity->flags >> 18) & 1) {
        return;
    }
    if (!((s32)(entity->flags >> 1) & 1)) {
        return;
    }
    if (entity->unk2C == 0) {
        return;
    }
    if (entity->unk80 != 0) {
        return;
    }

    Attack_GetEffectiveStrengths(&attack, request, extra);
    if (ENTITY_FLAG_BITS(entity)->bit35) {
        physical_bonus = entity->params->unkD0;
        magic_bonus = entity->params->unkD1;
    } else {
        physical_bonus = 0;
        magic_bonus = 0;
    }

    damage = 0;
    if (entity->params->unkD4 < 100) {
        damage = Attack_StrengthDamage(attack.unk00[0], request->unk50.unk00[0], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                               entity->params->unkE6 + physical_bonus + entity->params->unkD4);
    }
    if (entity->params->unkD6 < 100) {
        damage += Attack_StrengthDamage(attack.unk00[1], request->unk50.unk00[1], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                entity->params->unkE6 + physical_bonus + entity->params->unkD6);
    }
    if (entity->params->unkD8 < 100) {
        damage += Attack_StrengthDamage(attack.unk00[2], request->unk50.unk00[2], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                entity->params->unkE6 + physical_bonus + entity->params->unkD8);
    }
    if (entity->params->unkDA < 100) {
        damage += Attack_StrengthDamage(attack.unk00[3], request->unk50.unk00[3], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkDA);
    }
    if (entity->params->unkDC < 100) {
        damage += Attack_StrengthDamage(attack.unk00[4], request->unk50.unk00[4], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkDC);
    }
    if (entity->params->unkDE < 100) {
        damage += Attack_StrengthDamage(attack.unk00[5], request->unk50.unk00[5], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkDE);
    }
    if (entity->params->unkE0 < 100) {
        damage += Attack_StrengthDamage(attack.unk00[6], request->unk50.unk00[6], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkE0);
    }
    if (entity->params->unkE2 < 100) {
        damage += Attack_StrengthDamage(attack.unk00[7], request->unk50.unk00[7], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkE2);
    }
    if (entity->params->unkE4 < 100) {
        damage += Attack_StrengthDamage(attack.unk00[8], request->unk50.unk00[8], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkE4);
    }

    if (by_weapon) {
        damage = damage * extra->unk02 / 5000;
    }
    damage *= scale;

    if (by_weapon) {
        if (player_state.weapon == 0x11) {
            if ((entity->spawn_info->type_id >= 0x32 && entity->spawn_info->type_id <= 0x42) ||
                (entity->spawn_info->type_id >= 0x5C && entity->spawn_info->type_id <= 0x61) ||
                entity->spawn_info->type_id == 0x2F || entity->spawn_info->type_id == 0xA4 ||
                entity->spawn_info->type_id == 0xA7 || entity->spawn_info->type_id == 0xA8) {
                damage *= 2;
            }
        }
    }
    if (by_weapon && request->unk6F != 0) {
        chance = request->unk6F - entity->params->unkC9;
        if (ROLL_PERCENT() < chance) {
            damage *= 2;
        }
    }

    if (damage > 0) {
        if (by_weapon) {
            Player_CountWeaponUse(extra->unk24);
        }
        if (by_magic && extra->unk1C == 0) {
            Player_CountMagicUse(extra->unk26);
            extra->unk1C = 1;
        }
    }

    if (by_weapon && extra->unk04 == 0) {
        percent = damage * 100.0f / entity->params->unk0A;
        if (damage <= 0) {
            Attack_WearWeapon(extra->unk24, 4);
            Player_BounceWeapon();
            return;
        }
        if (percent < 10.0f) {
            Attack_WearWeapon(extra->unk24, 2);
        } else if (percent < 100.0f) {
            Attack_WearWeapon(extra->unk24, 1);
        }
    }

    if (damage <= 0) {
        return;
    }
    if (damage < entity->unk2C) {
        entity->unk2C -= damage;
    } else {
        entity->unk2C = 0;
    }
    entity->unk2E = damage;
    entity->unk80 = 15;

    percent = damage * 100.0f / entity->params->unk0A;
    ENTITY_FLAG_HIT_BITS(entity)->hit_strength = percent < 33.3334f ? 1 : 2;
    ENTITY_FLAG_HIT_BITS(entity)->hit_source = by_weapon ? 1 : by_magic ? 2 : 0; // match: a shift of by_magic in retail too

    // match: three stores to unk6C, one per arm, give retail's single shared call and store
    if (by_weapon && extra->unk04 == 0) {
        entity->unk6C = request->unk38;
    } else if (request->position[0] != request->unk10[0] || request->position[1] != request->unk10[1] ||
               request->position[2] != request->unk10[2]) {
        entity->unk6C = atan2f(request->position[0] - request->unk10[0], request->position[2] - request->unk10[2]);
    } else {
        entity->unk6C = atan2f(entity->model->position[0] - request->position[0], entity->model->position[2] - request->position[2]);
    }

    if (by_weapon || by_magic) {
        if (player_state.unk100 != 0 && player_state.hp_now > 0) {
            player_state.hp_now += damage * player_state.unk100 / 100;
            if (player_state.hp_max < player_state.hp_now) {
                player_state.hp_now = player_state.hp_max;
            }
        }
        if (player_state.unk104 != 0) {
            player_state.mp_now += damage * player_state.unk104 / 100;
            if (player_state.mp_max < player_state.mp_now) {
                player_state.mp_now = player_state.mp_max;
            }
        }
    }

    Attack_RollEntityAilments(entity, &request->unk62);
    // match: flag bit 40 (still named pad40); the source bit is read as a u8 field
    ENTITY_FLAG_BITS(entity)->pad40 = ((Unk1B1470RequestBits *)request)->unk4C_3;

    if (by_magic && request->unk70 != 0 && damage > 0) {
        chance = request->unk70 - entity->params->unkC9;
        if (ROLL_PERCENT() < chance) {
            entity->unk2C = 0;
        }
    }

    if (by_weapon && extra->unk04 == 0) {
        if (attack.unk00[0] >= attack.unk00[1] && attack.unk00[0] >= attack.unk00[2]) {
            Sound_PlayCommon(0x32);
        } else if (attack.unk00[1] >= attack.unk00[2]) {
            Sound_PlayCommon(0x33);
        } else {
            Sound_PlayCommon(0x34);
        }
    }
}

/* A hit may start the entity's four ailments: each has a chance in percent in the attack,
   less the entity's resistance to it. The last one started is the one that tints the model. */
void Attack_RollEntityAilments(Entity *entity, EntityParamsUnk240PartUnk12 *chances) {
    if ((u16)chances->unk00[0] != 0 && ROLL_PERCENT() < (u16)chances->unk00[0] - entity->params->unkEA) {
        entity->unk20 = 1800;
        entity->unk26 = 1;
    }
    if ((u16)chances->unk00[2] != 0 && ROLL_PERCENT() < (u16)chances->unk00[2] - entity->params->unkEB) {
        entity->unk24 = 3600;
        entity->unk26 = 2;
    }
    if ((u16)chances->unk00[4] != 0 && ROLL_PERCENT() < (u16)chances->unk00[4] - entity->params->unkEC) {
        entity->unk28 = 1800;
        entity->unk26 = 3;
    }
    if ((u16)chances->unk00[5] != 0 && ROLL_PERCENT() < (u16)chances->unk00[5] - entity->params->unkED) {
        entity->unk2A = 3600;
        entity->unk26 = 4;
    }
}

/* A hit lands on a map object: one that can break does when the damage reaches its toughness (a map's own
   hit always breaks it); otherwise the player's weapon bounces off. */
void Attack_HitMapObject(Unk1B1470Request *request, Unk1B1470Extra *extra, MapObject *object) {
    EntityParamsUnk240PartUnk00 attack;
    s32 by_weapon;
    s32 damage;

    if (object->unk08->unk30 == 0x201) {
        if (!IS_PLAYER(request->owner)) {
            return;
        }
        if (extra->unk00 != 0) {
            return;
        }
        if (extra->unk04 != 0) {
            return;
        }
        if (player_state.weapon == 0x2E) {
            MapObject_Break(object);
            return;
        }
        Attack_WearWeapon(extra->unk24, 4);
        Player_BounceWeapon();
        return;
    }
    if (request->owner == (Entity *)&map_slots[0] || request->owner == (Entity *)&map_slots[1]) {
        MapObject_Break(object);
        return;
    }
    if (!IS_PLAYER(request->owner)) {
        return;
    }
    by_weapon = extra->unk00 == 0;
    if (!(MAP_OBJECT_DEF_WORD(object) & 0x800)) {
        if (extra->unk00 != 0) {
            return;
        }
        if (extra->unk04 != 0) {
            return;
        }
        Attack_WearWeapon(extra->unk24, 4);
        Player_BounceWeapon();
        return;
    }

    Attack_GetEffectiveStrengths(&attack, request, extra);
    damage = 0;
    if (object->unk04->unk52[0] < 200) {
        damage = Attack_StrengthDamage(attack.unk00[0], request->unk50.unk00[0], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                               object->unk04->unk52[0]);
    }
    if (object->unk04->unk52[1] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[1], request->unk50.unk00[1], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                object->unk04->unk52[1]);
    }
    if (object->unk04->unk52[2] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[2], request->unk50.unk00[2], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                object->unk04->unk52[2]);
    }
    if (object->unk04->unk52[3] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[3], request->unk50.unk00[3], extra->unk22, object->unk04->unk52[3]);
    }
    if (object->unk04->unk52[4] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[4], request->unk50.unk00[4], extra->unk22, object->unk04->unk52[4]);
    }
    if (object->unk04->unk52[5] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[5], request->unk50.unk00[5], extra->unk22, object->unk04->unk52[5]);
    }
    if (object->unk04->unk52[6] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[6], request->unk50.unk00[6], extra->unk22, object->unk04->unk52[6]);
    }
    if (object->unk04->unk52[7] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[7], request->unk50.unk00[7], extra->unk22, object->unk04->unk52[7]);
    }
    if (object->unk04->unk52[8] < 200) {
        damage += Attack_StrengthDamage(attack.unk00[8], request->unk50.unk00[8], extra->unk22, object->unk04->unk52[8]);
    }

    if (by_weapon) {
        damage = damage * extra->unk02 / 5000;
    }
    if ((MAP_OBJECT_DEF_WORD(object) & 0x2000) && by_weapon) {
        damage *= request->unk6E;
    }
    if (damage >= MAP_OBJECT_DEF_TOUGHNESS(object->unk04)) {
        MapObject_Break(object);
        return;
    }
    if (player_state.unk262 != 0) {
        return;
    }
    if (extra->unk00 != 0) {
        return;
    }
    if (extra->unk04 != 0) {
        return;
    }
    Attack_WearWeapon(extra->unk24, 4);
    Player_StrikeSolidObject(object->unk08->unk30 != 0x1C && object->unk08->unk30 != 0xFF);
}

/* An attack struck the map: the player's weapon wears and bounces off. */
int Attack_HitMap(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    if (IS_PLAYER(request->owner)) {
        if (extra->unk00 == 0) {
            Attack_WearWeapon(extra->unk24, 4);
            if (extra->unk04 == 0) {
                return ((int (*)(void))Player_BounceWeapon)(); // match: a tail call, as if it returned int
            }
        }
    }
}

/* The nine strengths of a hit as they count against the target (Attack_HitEntity). The player's are
   raised by the weapon bonus (extra->unk20) or the magic bonus (unk22, plus unk0E[] per kind) in
   fiftieths. A hit of kind 0 from anything else counts as it is; otherwise the three physical
   strengths are dropped and the six magic ones scaled, by EntityParams.unk238 for an entity.
   The rows are spelled out as retail has them. */
void Attack_GetEffectiveStrengths(EntityParamsUnk240PartUnk00 *out, Unk1B1470Request *request, Unk1B1470Extra *extra) {
    f32 strength;
    s32 whole;

    if (request->owner == (Entity *)&player_state) {
        if (extra->unk00 == 0) {
            STORE_ROUNDED_UP(out->unk00[0], (f32)request->unk50.unk00[0] + (f32)extra->unk20 * (f32)request->unk50.unk00[0] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[1], (f32)request->unk50.unk00[1] + (f32)extra->unk20 * (f32)request->unk50.unk00[1] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[2], (f32)request->unk50.unk00[2] + (f32)extra->unk20 * (f32)request->unk50.unk00[2] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[3], (f32)request->unk50.unk00[3] + (f32)extra->unk22 * (f32)request->unk50.unk00[3] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[4], (f32)request->unk50.unk00[4] + (f32)extra->unk22 * (f32)request->unk50.unk00[4] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[5], (f32)request->unk50.unk00[5] + (f32)extra->unk22 * (f32)request->unk50.unk00[5] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[6], (f32)request->unk50.unk00[6] + (f32)extra->unk22 * (f32)request->unk50.unk00[6] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[7], (f32)request->unk50.unk00[7] + (f32)extra->unk22 * (f32)request->unk50.unk00[7] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[8], (f32)request->unk50.unk00[8] + (f32)extra->unk22 * (f32)request->unk50.unk00[8] / 50.0f);
        } else {
            STORE_ROUNDED_UP(out->unk00[0], (f32)request->unk50.unk00[0] + (f32)extra->unk22 * (f32)request->unk50.unk00[0] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[1], (f32)request->unk50.unk00[1] + (f32)extra->unk22 * (f32)request->unk50.unk00[1] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[2], (f32)request->unk50.unk00[2] + (f32)extra->unk22 * (f32)request->unk50.unk00[2] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[3], request->unk50.unk00[3] + (f32)extra->unk22 * request->unk50.unk00[3] / 50.0f + extra->unk0E[0] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[4], request->unk50.unk00[4] + (f32)extra->unk22 * request->unk50.unk00[4] / 50.0f + extra->unk0E[1] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[5], request->unk50.unk00[5] + (f32)extra->unk22 * request->unk50.unk00[5] / 50.0f + extra->unk0E[2] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[6], request->unk50.unk00[6] + (f32)extra->unk22 * request->unk50.unk00[6] / 50.0f + extra->unk0E[3] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[7], request->unk50.unk00[7] + (f32)extra->unk22 * request->unk50.unk00[7] / 50.0f + extra->unk0E[4] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[8], request->unk50.unk00[8] + (f32)extra->unk22 * request->unk50.unk00[8] / 50.0f + extra->unk0E[5] / 50.0f);
        }
    } else if (request->owner >= entity_pool[0] && request->owner <= &entity_pool_last) {
        if (extra->unk00 == 0) {
            *out = request->unk50;
        } else {
            Entity *owner = request->owner;

            out->unk00[0] = 0;
            out->unk00[2] = 0;
            out->unk00[1] = 0;
            STORE_ROUNDED_UP(out->unk00[3], request->unk50.unk00[3] * (f32)ENTITY_HIT_SCALES(owner)[0] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[4], request->unk50.unk00[4] * (f32)ENTITY_HIT_SCALES(owner)[1] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[5], request->unk50.unk00[5] * (f32)ENTITY_HIT_SCALES(owner)[2] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[6], request->unk50.unk00[6] * (f32)ENTITY_HIT_SCALES(owner)[3] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[7], request->unk50.unk00[7] * (f32)ENTITY_HIT_SCALES(owner)[4] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[8], request->unk50.unk00[8] * (f32)ENTITY_HIT_SCALES(owner)[5] / 10.0f);
        }
    } else {
        if (extra->unk00 == 0) {
            *out = request->unk50;
        } else {
            out->unk00[0] = 0;
            out->unk00[2] = 0;
            out->unk00[1] = 0;
            STORE_ROUNDED_UP(out->unk00[3], request->unk50.unk00[3] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[4], request->unk50.unk00[4] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[5], request->unk50.unk00[5] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[6], request->unk50.unk00[6] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[7], request->unk50.unk00[7] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[8], request->unk50.unk00[8] * 100.0f / 10.0f);
        }
    }
}

/* Damage from one strength of a hit: the strength cut by the resistance (out of 200), plus
   what the base strength, and the base raised by the bonus in fiftieths, exceed half of it. */
int Attack_StrengthDamage(int strength, int base, int bonus, int resistance) {
    float base_strength;
    float resist;
    float base_excess;
    float boosted_excess;
    float boosted;
    float damage;

    base_strength = (float)base;
    resist = (float)resistance;
    base_excess = ((base_strength + base_strength) - resist) * 0.5f;
    boosted = base_strength + ((base_strength * (float)bonus) / 50.0f);
    boosted_excess = ((boosted + boosted) - resist) * 0.5f;
    damage = ((float)strength * (200.0f - resist)) / 200.0f;
    if (base_excess > 0.0f) {
        damage += base_excess * 0.6f;
    }
    if (boosted_excess > 0.0f) {
        damage += boosted_excess;
    }
    return (int)damage;
}

/* Damage to the player from one strength of a hit, against the player's resistance and stat. */
int Attack_StrengthDamageToPlayer(int strength, int resistance, int stat) {
    float value = (float)strength;
    return (int)((value * value) / (value + (float)resistance * 1.3f + ((float)stat + 1.0f) / 3.0f));
}

/* Wears a weapon: amount is added to its wear, and every exp_per_level of it takes one off its level, the
   percent its values count at (down to 50). Weapons whose unk63 is 2 or more do not wear. */
void Attack_WearWeapon(int idx, int amount) {
    PlayerItemDef *item;
    LevelRecord *rec;
    LevelDef *def;

    if (idx != 0) {
        item = &weapon_defs[idx];
        def = &weapon_name_defs[idx];
        rec = &weapon_level_records[idx];
        if (item->unk63 < 2 && rec->level >= 0x33) {
            rec->exp += amount;
            while (rec->exp >= def->exp_per_level) {
                rec->exp -= def->exp_per_level;
                rec->level--;
                if (rec->level < 0x33) {
                    rec->level = 0x32;
                    rec->exp = 0;
                    break;
                }
            }
        }
    }
}

/* The player was hit for `damage`: what he wears wears by how much of his health that was. Without a shield
   everything wears double, and the piece the hit landed on (or one picked by chance) four times. */
void Attack_WearArmorFromHit(int damage, Unk1B1470Request *request) {
    f32 percent = (f32)damage * 100.0f / (f32)player_state.hp_max;
    int amount = damage * 10 / player_state.hp_max;
    int part;
    int shield_exp;
    int helmet_exp;
    int armor_exp;
    int gloves_exp;
    int boots_exp;

    if (amount <= 0) {
        amount = 1;
    }
    // match: cleared after the amount is worked out (declared with initialisers, the divide check comes out differently)
    shield_exp = 0;
    helmet_exp = 0;
    armor_exp = 0;
    gloves_exp = 0;
    boots_exp = 0;
    if (request->unk4E != 0) {
        part = request->unk4E;
    } else {
        part = (int)(rand() * 4.0f * 4.656613e-10f) + 1;
    }
    if (!((s32)(player_state.unk1C8 >> 35) & 1) && player_state.equipment[EQUIP_SHIELD] != ITEM_WORN_INDEX(ITEM_SHIELD_NONE)) {
        shield_exp = 1;
        if (!(percent < 5.0f)) {
            shield_exp = amount * 2;
            helmet_exp = amount;
            armor_exp = amount;
            gloves_exp = amount;
            boots_exp = amount;
        }
    } else {
        helmet_exp = amount * 2;
        if (part == 1) {
            helmet_exp = amount * 4;
        }
        armor_exp = amount * 2;
        if (part == 2) {
            armor_exp = amount * 4;
        }
        gloves_exp = amount * 2;
        if (part == 3) {
            gloves_exp = amount * 4;
        }
        boots_exp = amount * 2;
        if (part == 4) {
            boots_exp = amount * 4;
        }
    }
    if (player_state.equipment[EQUIP_SHIELD] != ITEM_WORN_INDEX(ITEM_SHIELD_NONE)) {
        Attack_WearWornItem(player_state.equipment[EQUIP_SHIELD], shield_exp);
    }
    if (player_state.equipment[EQUIP_HELM] != ITEM_WORN_INDEX(ITEM_HELM_NONE)) {
        Attack_WearWornItem(player_state.equipment[EQUIP_HELM], helmet_exp);
    }
    if (player_state.equipment[EQUIP_ARMOR] != ITEM_WORN_INDEX(ITEM_ARMOR_NONE)) {
        Attack_WearWornItem(player_state.equipment[EQUIP_ARMOR], armor_exp);
    }
    if (player_state.equipment[EQUIP_GAUNTLET] != ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE)) {
        Attack_WearWornItem(player_state.equipment[EQUIP_GAUNTLET], gloves_exp);
    }
    if (player_state.equipment[EQUIP_BOOT] != ITEM_WORN_INDEX(ITEM_BOOT_NONE)) {
        Attack_WearWornItem(player_state.equipment[EQUIP_BOOT], boots_exp);
    }
}

/* Wears a worn item as Attack_WearWeapon wears a weapon; the "none" items do not wear. */
void Attack_WearWornItem(int idx, int amount) {
    LevelRecord *rec = &worn_item_level_records[idx];
    LevelDef *def = &worn_name_defs[idx];

    if (rec->level >= 0x33 && idx != ITEM_WORN_INDEX(ITEM_SHIELD_NONE) && idx != ITEM_WORN_INDEX(ITEM_HELM_NONE)
        && idx != ITEM_WORN_INDEX(ITEM_ARMOR_NONE) && idx != ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE)
        && idx != ITEM_WORN_INDEX(ITEM_BOOT_NONE) && idx != ITEM_WORN_INDEX(ITEM_RING_NONE)
        && idx != ITEM_WORN_INDEX(ITEM_BRACELET_NONE) && idx != ITEM_WORN_INDEX(ITEM_AMULET_NONE)) {
        rec->exp += amount;
        while (rec->exp >= def->exp_per_level) {
            rec->exp -= def->exp_per_level;
            rec->level--;
            if (rec->level < 0x33) {
                rec->level = 0x32;
                rec->exp = 0;
                break;
            }
        }
    }
}

/* Drains amount of the player's MP: Player_Update takes it off next frame and tints the screen. */
void Attack_DrainPlayerMp(int amount) {
    player_state.unk2A0 += amount;
}

/* Loads the magic table fx_table from param/magic.prm. */
void Attack_LoadMagicTable(void) {
    int file_handle = File_Open(magic_param_path, 0, 1);

    File_Read(file_handle, fx_table, -1);
    File_Close(file_handle);
}

/* Launches the effect of a magic table row (its unk100) from `from` along `angles`; returns Effect_Spawn's
   result, 0 for an id past the table or a row that cannot be launched (unk102). */
int Attack_LaunchMagicEffect(u16 effect_id, void *shot, void *from, void *angles, void *entity) {
    FxTableEntry *entry;

    if (effect_id >= 0x100) {
        return 0;
    }
    entry = &fx_table[effect_id];
    if (entry->unk102 != 0) {
        return 0;
    }
    return Effect_Spawn(entry->unk100, from, angles, entity, effect_id, shot);
}

/* The hit check of an effect: builds the attack request from the effect table's row
   (or, for a weapon's shot, from the request the weapon left) and runs it. Returns what
   Attack_Run found. */
int Attack_RunEffectHitCheck(void *effect_actor) {
    Unk1B1470Request request;
    EffectActor *effect = effect_actor;
    FxTableHitCheck *check;
    Vector4 *position;
    Unk1B1470Extra *shot;
    s32 hits;

    if (effect == NULL) {
        return 0;
    }
    if (effect->effect_param < 0x100 && effect->def->unk10B < 3) {
        check = &fx_table[effect->effect_param].hit_checks[effect->def->unk10B];
        memset(&request, 0, sizeof(request));
        switch (check->shape) {
        case 0:
            position = &effect->unk70;
            request.unk30 = check->unk1E;
            request.unk4C_0 = 0;
            break;
        case 1:
            position = &effect->unk70;
            request.unk30 = check->unk1E;
            request.unk4C_0 = 1;
            request.unk34 = check->unk20;
            break;
        case 2:
            position = &effect->unk70;
            request.unk4C_0 = 2;
            Vector_Copy(request.unk20, position);
            break;
        default:
            return 0;
        }
    } else {
        return 0;
    }
    Vector_Copy(request.position, &effect->model->position);
    Vector_Copy(request.unk10, position);
    request.owner = effect->owner_entity;
    request.unk4C_2 = 0;
    request.unk4D = 0;
    if (check->unk23 & 1) {
        request.unk4D = 1;
    }
    if (check->unk23 & 2) {
        request.unk4D |= 2;
    }
    if (check->unk23 & 4) {
        request.unk4D |= 4;
    }
    if (check->unk23 & 8) {
        request.unk4D |= 8;
    }
    if (check->unk23 & 0x10) {
        request.unk4D |= 0x10;
    }
    if (check->unk23 & 0x20) {
        request.unk4D |= 0x20;
    }
    if (check->unk23 & 0x80) {
        request.unk4D |= 0x80;
    }
    shot = effect->owner;
    if (shot->unk04 == 0) {
        request.unk50 = check->strengths;
        request.unk62 = check->unk12;
        request.unk4C_3 = check->unk26 != 0;
        request.unk4F = check->unk25;
        request.unk70 = check->unk27;
        request.unk72 = EFFECT_FLAG_PARTS(effect)->unk40;
        request.unk6F = 0;
        hits = Attack_Run(&request, shot);
        if (hits != 0 && (s8)request.unk74 != 0 && EFFECT_FLAG_PARTS(effect)->unk40 == 0) {
            EFFECT_FLAG_PARTS(effect)->unk40 = check->unk24;
        }
    } else {
        request.unk50 = SHOT_REQUEST->unk50;
        request.unk62 = SHOT_REQUEST->unk62;
        request.unk4C_3 = ((Unk1B1470RequestBits *)SHOT_REQUEST)->unk4C_3;
        request.unk70 = 0;
        request.unk4F = 0;
        request.unk6F = SHOT_REQUEST->unk6F;
        hits = Attack_Run(&request, shot);
    }
    return hits;
}
