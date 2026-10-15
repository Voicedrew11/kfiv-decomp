#include "kf_map.h"
#include "kf_collision.h"
#include "kf_player.h"
#include "kf_entity.h"
#include "kf_effect.h"
#include "kf_vu0.h"

/* moves the player out of the body just hit: to `push` from it, but by no more than 50 a frame */
#define PUSH_PLAYER_OUT()                                                                          \
    Vector_Scale(offset, COLLISION_RESULT->normal, push);                                       \
    Vector_Add(offset, COLLISION_RESULT, offset);                                               \
    Vector_Subtract(offset, offset, &player_state);                                               \
    length = sqrtf(offset[0] * offset[0] + offset[2] * offset[2]);                                 \
    if (50.0f < length) {                                                                          \
        offset[1] = 0.0f;                                                                          \
        Vector_Scale(offset, offset, 50.0f / length);                                           \
        Vector_Add(&player_state, &player_state, offset);                                           \
    } else {                                                                                       \
        player_state.position.x = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;   \
        player_state.position.z = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;   \
    }

int Collision_TransformMeshPoints(void *);
void Collision_DropOnTriangle(void *, void *, void *, void *);
void Collision_DropOnTriangleWithin(void *, void *, void *, void *, f32);
u32 Collision_GetFloorMaterial(int, u32, u32);
u32 Collision_GetSurfaceEffect(u32, u32, int);
void Map_MakeCurrent(int);
float sqrtf(float);
void Collision_CylinderCylinder(void *, f32, f32, void *, f32, f32);
void Collision_PushOutOfObjects(f32 *, s32, f32, f32);
s32 Collision_StandOnObjects(f32 *, s32, f32, f32);
void Player_FlashDamage(void);

extern PlayerState player_state;
extern PlayerState player_previous;      /* the player as of the frame before */
extern int D_00446770;
extern MapSlot map_slots[2];
extern MapSlotMemory D_004321F0[2];
extern f32 D_00446968[];
extern f32 D_0044696C[];    /* the largest float; an array so that it is not read through $gp */
extern Entity *D_004467E0;
extern u32 D_00446ED4;
extern u8 spell_levels[6][5];
extern FxTableEntry fx_table[];

/* Tests the player against the collision meshes of the records around him: what he touches, the walls that
   push him back, the water surface he is in and the floor he stands on. Returns TRUE when a floor was found. */
int Player_CollideWithMap(void) {
    // match: the scratchpad points through pointers (retail forms each address anew), and the order of these
    // declarations decides which stack slot each value that does not fit a register gets
    Vector4 *wall_point = &COLLISION_SCRATCH->unk90;
    Vector4 *floor_point = &COLLISION_SCRATCH->point;
    Vector4 raised_point;
    s32 step_bit;
    s32 map;
    s32 record_count;
    s32 found;
    u8 stepping;
    s32 floor_map;
    f32 floor_height;
    f32 reach;
    MapSlot *slot;
    MapRecordBounds *record;
    s32 i;
    ModelInstance *model;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 strips_left;
    s32 points_left;
    u32 surface;
    f32 drop;

    step_bit = (s32)(player_state.unk1C8 >> 33) & 1;
    stepping = step_bit; // match: tested as the bit, kept as a byte
    if (step_bit) {
        if (player_state.unk1D0 <= player_state.unk1D4) {
            player_state.position.y -= player_state.unk1D0;
            player_state.unk1C8 &= ~(0x8000L << 18);
            player_state.unk1D4 = 0.0f;
            player_state.unk1D0 = 0.0f;
        } else {
            player_state.unk1D0 -= player_state.unk1D4;
            player_state.position.y -= player_state.unk1D4;
        }
    }
    floor_height = D_00446968[0];
    Vector_Copy(wall_point, &player_state.position);
    found = FALSE;
    wall_point->y -= 1.0f;
    Vector_Copy(floor_point, &player_state.position);
    floor_point->y -= 75.0f;
    reach = player_state.unk1AC + 75.0f;
    Vector_Copy(&raised_point, &player_state.position);
    floor_map = D_00446770;
    player_state.unk211 = player_state.unk210 = 6;
    player_state.unk214 = 0.0f;
    raised_point.y += 3.0f; // match: the raised point is never used
    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &map_slots[0] : &map_slots[1];
        if (slot->unk00 != 0) {
            record_count = slot->header->record_count;
            model = (ModelInstance *)D_004321F0[D_00446770 == map ? 0 : 1].unk34;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++, model++) {
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (player_state.position.x < record->lowest[0] - 100.0f
                    || record->highest[0] + 100.0f < player_state.position.x
                    || player_state.position.z < record->lowest[2] - 100.0f
                    || record->highest[2] + 100.0f < player_state.position.z) {
                    continue;
                }
                mesh = (CollisionMesh *)model->strips;
                ((void (*)(void *))Collision_TransformMeshPoints)(model); // match: called as if it returned nothing
                if (player_state.unk1E0 == 0) {
                    strips_left = mesh->unk40;
                    strip = mesh->unk28;
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        do {
                            Collision_DropOnTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                          &player_state.position);
                            if (COLLISION_RESULT->hit != 0 && player_state.position.y - 230.0f <= COLLISION_RESULT->point[1]
                                && COLLISION_RESULT->point[1] <= player_state.position.y) {
                                surface = Collision_GetSurfaceEffect(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                                if (surface != 0) {
                                    player_state.unk1E4 = surface;
                                    player_state.unk1E0 = 1;
                                } else {
                                    player_state.unk1EC = 1;
                                }
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
                strips_left = mesh->unk38;
                strip = mesh->unk18;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        Collision_CapsuleTriangle((f32 *)strip_point[0].position, (f32 *)strip_point[1].position,
                                      (f32 *)strip_point[2].position, (f32 *)&COLLISION_SCRATCH->unk90, 100.0f, 230.0f);
                        if (COLLISION_RESULT->hit != 0) {
                            COLLISION_SCRATCH->push.x = COLLISION_SCRATCH->unk90.x - COLLISION_RESULT->point[0];
                            COLLISION_SCRATCH->push.y = 0.0f;
                            COLLISION_SCRATCH->push.z = COLLISION_SCRATCH->unk90.z - COLLISION_RESULT->point[2];
                            Vector_Normalize(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push);
                            Vector_Scale(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push, 100.0f);
                            COLLISION_SCRATCH->unk90.x = COLLISION_RESULT->point[0] + COLLISION_SCRATCH->push.x;
                            COLLISION_SCRATCH->unk90.z = COLLISION_RESULT->point[2] + COLLISION_SCRATCH->push.z;
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
                if (map == 0) {
                    strips_left = mesh->unk42;
                    strip = mesh->unk2C;
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        do {
                            Collision_DropOnTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                          &player_state.position);
                            if (COLLISION_RESULT->hit != 0
                                && COLLISION_RESULT->point[1] <= player_state.position.y + player_state.unk1AC
                                && (!((s32)(player_state.unk1C8 >> 38) & 1) || COLLISION_RESULT->point[1] < player_state.unk1DC)) {
                                player_state.unk1DC = COLLISION_RESULT->point[1];
                                player_state.unk1C8 |= 0x8000L << 23;
                                if (COLLISION_RESULT->point[1] <= player_state.position.y - 220.0f) {
                                    player_state.unk1C8 |= 0x8000L << 24;
                                } else {
                                    player_state.unk1C8 &= ~(0x8000L << 24);
                                }
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
                if (stepping) {
                    continue;
                }
                if (player_state.unk208 != 0
                    && (MAP_OBJECT_DEF_FLAGS(((MapObject *)player_state.unk208)->unk04) & MAP_OBJECT_DEF_CARRIES)) {
                    continue;
                }
                strips_left = mesh->floor_strip_count;
                strip = mesh->floor_strips;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        Collision_DropOnTriangleWithin(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                      &COLLISION_SCRATCH->point, reach);
                        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < floor_height) {
                            found = TRUE;
                            floor_height = COLLISION_RESULT->point[1];
                            player_state.unk250 = record->unk38;
                            player_state.map_index = i;
                            player_state.unk210 = Collision_GetFloorMaterial(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                            floor_map = D_00446770 != map;
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    player_state.position.x = wall_point->x;
    player_state.position.z = wall_point->z;
    if (!stepping) {
        if (found) {
            drop = player_state.position.y - floor_height;
            if (drop < 16.666666f) {
                player_state.position.y = floor_height;
            } else {
                player_state.unk1C8 |= 0x8000L << 18;
                player_state.unk1D4 = 16.666666f;
                player_state.position.y -= 16.666666f;
                player_state.unk1D0 = drop - 16.666666f + 5.0f;
            }
            player_state.unk1AC = 0.0f;
        } else if (player_state.unk1A8 == 0) {
            player_state.position.y += player_state.unk1AC;
        } else {
            player_state.position.y += player_state.unk1AC * (f32)(60 - player_state.unk1A8) / 60.0f;
        }
    }
    if (((s32)(player_state.unk1C8 >> 33) & 1) || found) {
        player_state.unk1C8 &= ~(0x8000L << 19);
    }
    if (!((s32)(player_previous.unk1C8 >> 38) & 1) && ((s32)(player_state.unk1C8 >> 38) & 1)) {
        player_state.unk1AC = 0.0f;
        player_state.unk1C8 &= ~(0x8000L << 19);
    }
    if (player_previous.unk1E4 != 4 && player_state.unk1E4 == 4) {
        player_state.unk1AC = 0.0f;
        player_state.unk1C8 &= ~(0x8000L << 19);
    }
    if (player_state.unk1E0 == 0) {
        player_state.unk1E8 = 0;
    } else if (player_previous.unk1E0 != 0 && player_previous.unk1E4 == player_state.unk1E4) {
        player_state.unk1E8++;
    } else {
        player_state.unk1E8 = 0;
    }
    Map_MakeCurrent(floor_map);
    return found;
}

/* Keeps the player 100 units away from the wall triangles (CollisionMesh.unk18) of the records around him:
   each one that is hit pushes him straight out, level with the ground. */
void Player_PushOutOfWalls(void) {
    Vector4 *point = &COLLISION_SCRATCH->point;
    MapSlot *slot;
    MapRecordBounds *record;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 strips_left;
    s32 points_left;

    Vector_Copy(&COLLISION_SCRATCH->point, &player_state.position);
    point->y -= 1.0f;
    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &map_slots[0] : &map_slots[1];
        if (slot->unk00 != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (player_state.position.x < record->lowest[0] - 100.0f
                    || record->highest[0] + 100.0f < player_state.position.x
                    || player_state.position.z < record->lowest[2] - 100.0f
                    || record->highest[2] + 100.0f < player_state.position.z) {
                    continue;
                }
                Collision_TransformMeshPoints(record->model);
                mesh = (CollisionMesh *)record->model->strips;
                strips_left = mesh->unk38;
                strip = mesh->unk18;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        Collision_CapsuleTriangle((f32 *)strip_point[0].position, (f32 *)strip_point[1].position,
                                      (f32 *)strip_point[2].position, (f32 *)&COLLISION_SCRATCH->point, 100.0f, 230.0f);
                        if (COLLISION_RESULT->hit != 0) {
                            COLLISION_SCRATCH->push.x = COLLISION_SCRATCH->point.x - COLLISION_RESULT->point[0];
                            COLLISION_SCRATCH->push.y = 0.0f;
                            COLLISION_SCRATCH->push.z = COLLISION_SCRATCH->point.z - COLLISION_RESULT->point[2];
                            Vector_Normalize(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push);
                            Vector_Scale(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push, 100.0f);
                            COLLISION_SCRATCH->point.x = COLLISION_RESULT->point[0] + COLLISION_SCRATCH->push.x;
                            COLLISION_SCRATCH->point.z = COLLISION_RESULT->point[2] + COLLISION_SCRATCH->push.z;
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    player_state.position.x = point->x;
    player_state.position.z = point->z;
}

/* Finds the floor triangle under the player (the highest one within 10 below his feet; y grows downward) in
   the records around him and notes its material in PlayerState.unk210. */
void Player_FindFloorMaterial(void) {
    f32 lowest;
    MapSlot *slot;
    MapRecordBounds *record;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 strips_left;
    s32 points_left;

    lowest = D_0044696C[0];
    Vector_Copy(&COLLISION_SCRATCH->point, &player_state.position);
    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &map_slots[0] : &map_slots[1];
        if (slot->unk00 != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (player_state.position.x < record->lowest[0] - 100.0f
                    || record->highest[0] + 100.0f < player_state.position.x
                    || player_state.position.z < record->lowest[2] - 100.0f
                    || record->highest[2] + 100.0f < player_state.position.z) {
                    continue;
                }
                Collision_TransformMeshPoints(record->model);
                mesh = (CollisionMesh *)record->model->strips;
                strips_left = mesh->floor_strip_count;
                strip = mesh->floor_strips;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        Collision_DropOnTriangleWithin(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                      &COLLISION_SCRATCH->point, 10.0f);
                        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                            lowest = COLLISION_RESULT->point[1];
                            player_state.unk210 = Collision_GetFloorMaterial(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
}

/* Collides the player with the entities. With `climb` set the player is first put on top of any
   entity low enough to step onto (a step higher than 16.67 is taken over several frames); then
   the player is pushed out of every entity touched. Four kinds are made of up to four bodies. */
void Player_CollideWithEntities(int climb) {
    f32 old_position[4];
    f32 position[4];
    f32 offset[4];
    s32 stepped = FALSE;
    Entity *entity;
    EntityBodyPoint *body;
    void *point;
    f32 reach;
    f32 push;
    f32 radius;
    f32 height;
    f32 top;
    f32 rise;
    f32 step;
    f32 length;
    s32 i;

    player_state.unk204 = 0;
    if (climb) {
        Vector_Copy(old_position, &player_state);
        player_state.position.y += 1.0f;
        reach = player_state.unk1AC + 75.0f;
        if (!((s32)(player_state.unk1C8 >> 33) & 1)) {
            for (entity = D_004467E0; entity != NULL; entity = entity->next) {
                if (!((s32)(entity->flags >> 1) & 1)) {
                    continue;
                }
                if (((s32)(entity->flags >> 2) & 3) >= 2) {
                    continue;
                }
                if ((s32)(entity->flags >> 27) & 1) {
                    continue;
                }
                switch (entity->spawn_info->type_id) {
                case 0x11:
                case 0x14:
                case 0x97:
                case 0x9B:
                    continue;
                }
                Vector_Copy(position, entity->model->position);
                radius = entity->radius;
                if (!(((EntityParamsWord *)entity->params)->unk00 & 0x8000000)) {
                    height = entity->height;
                } else {
                    height = radius + radius;
                    position[1] += radius;
                }
                if (radius + 100.0f < entity->distance_to_player) {
                    continue;
                }
                top = position[1] - height;
                if (player_state.position.y < top) {
                    continue;
                }
                if (position[1] < player_state.position.y - 230.0f) {
                    continue;
                }
                if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
                    continue;
                }
                if (entity->spawn_info->type_id == 0x50) {
                    continue;
                }
                if (entity->spawn_info->type_id == 0x9F) {
                    continue;
                }
                if (reach < player_state.position.y - top) {
                    continue;
                }
                player_state.position.y = top;
                stepped = TRUE;
                player_state.unk204 = (s32)entity;
            }
        }
        player_state.position.y -= 1.0f;
        if (stepped) {
            step = 16.666666f;
            rise = old_position[1] - player_state.position.y;
            if (step < rise) {
                player_state.position.y = old_position[1] - step;
                player_state.unk1D0 = rise - step + 5.0f;
                player_state.unk1D4 = step;
                player_state.unk1C8 |= 1L << 33;
            }
            player_state.unk1AC = 0.0f;
            player_state.unk1C8 &= ~(1L << 34);
        }
    }
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (!((s32)(entity->flags >> 1) & 1)) {
            continue;
        }
        if (((s32)(entity->flags >> 2) & 3) >= 2) {
            continue;
        }
        if ((s32)(entity->flags >> 27) & 1) {
            continue;
        }
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            continue;
        }
        switch (entity->spawn_info->type_id) {
        case 0x11:
        case 0x14:
        case 0x97:
        case 0x9B:
            ENTITY_MODEL(entity)->unk1B4(ENTITY_MODEL(entity));
            body = entity->params->unk20;
            for (i = 0; i < 4; i++, body++) {
                COLLISION_RESULT->hit = 0;
                if (!(body->unk00.flags & ENTITY_BODY_USED)) {
                    continue;
                }
                if (entity->spawn_info->type_id == 0x9B && i != 3) {
                    continue;
                }
                point = (void *)ENTITY_MODEL(entity)->get_point(ENTITY_MODEL(entity), body->unk00.point);
                Vector_Copy(position, point);
                if (!(body->unk00.flags & ENTITY_BODY_ROUND)) {
                    radius = body->unk04 * ENTITY_MODEL(entity)->unkE0;
                    height = body->unk06 * ENTITY_MODEL(entity)->unkE4;
                } else {
                    radius = body->unk04 * ENTITY_MODEL(entity)->unkE0;
                    position[1] += radius;
                    height = radius + radius;
                }
                push = 100.0f;
                Collision_CylinderCylinder(&player_state, push, 230.0f, position, radius, height);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_PLAYER_OUT();
                }
            }
            break;
        default:
            push = 100.0f;
            Collision_CylinderCylinder(&player_state, push, 230.0f, entity->model->position, entity->radius, entity->height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_PLAYER_OUT();
            }
            break;
        }
    }
}

/* Collides the player with the map objects. With `climb` set the player is first tried a little
   higher, so a low object is stepped onto; a step higher than 16.67 is taken over several frames
   (status bit 33). An object touched for the first time starts the timed flag its definition
   names. */
void Player_CollideWithMapObjects(s32 climb) {
    f32 old_position[4];
    s32 hit = FALSE;
    f32 reach;
    f32 rise;
    f32 step;

    player_state.unk208 = 0;
    player_state.unk20C = 0;
    if (climb) {
        Vector_Copy(old_position, &player_state);
        player_state.position.y += 1.0f;
        reach = player_state.unk1AC + 75.0f;
        if (player_previous.unk208 != 0) {
            player_state.position.y += 30.0f;
            reach += 30.0f;
        }
        if (!((s32)(player_state.unk1C8 >> 33) & 1)) {
            hit = Collision_StandOnObjects((f32 *)&player_state, 1, reach, 100.0f);
        }
        if (hit) {
            step = 16.666666f;
            rise = old_position[1] - player_state.position.y;
            if (step < rise && player_previous.unk208 != player_state.unk208) {
                player_state.position.y = old_position[1] - step;
                player_state.unk1D0 = rise - step + 5.0f;
                player_state.unk1D4 = step;
                player_state.unk1C8 |= 1L << 33;
            }
            player_state.unk1AC = 0.0f;
            player_state.unk1C8 &= ~(1L << 34);
        } else {
            Vector_Copy(&player_state, old_position);
        }
    }
    if (!((s32)(player_state.unk1C8 >> 33) & 1)) {
        Collision_PushOutOfObjects((f32 *)&player_state, 0, 230.0f, 100.0f);
    }
    if (player_state.unk20C != 0 && player_state.unk20C != player_previous.unk20C) {
        switch ((s16)((MapObject *)player_state.unk20C)->unk08->unk52) {
        case 1:
            player_state.timed_flags[0] = 1800;
            player_state.unk2C4 = D_00446ED4;
            break;
        case 2:
            player_state.timed_flags[1] = 150;
            player_state.unk2C8 = D_00446ED4;
            break;
        case 3:
            player_state.timed_flags[2] = 3600;
            player_state.unk2CC = D_00446ED4;
            break;
        case 4:
            player_state.timed_flags[3] = 5400;
            player_state.unk2D0 = D_00446ED4;
            break;
        case 5:
            player_state.timed_flags[4] = 1800;
            player_state.unk2D4 = D_00446ED4;
            break;
        case 6:
            player_state.timed_flags[5] = 18000;
            player_state.unk2D8 = D_00446ED4;
            break;
        }
        Player_FlashDamage();
    }
    if (player_state.unk208 != 0 && ((MapObject *)player_state.unk208)->unk08->unk30 == 0x165) {
        player_state.unk210 = 2;
    }
}

/* Whether the player has learned any magic (a nonzero entry of spell_levels). */
int Player_KnowsAnyMagic(void) {
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] != 0) {
                return 1;
            }
        }
    }
    return 0;
}

/* Whether the player has learned a magic whose first level launches a shot (fx_table unk102 0: the magics
   that are equipped and cast from the gauge). */
int Player_KnowsShotMagic(void) {
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] != 0) {
                if (fx_table[15 * i + 3 * j + 1].unk102 == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* Whether the player has learned a magic whose first level is used from the item menu (fx_table unk102 1, the
   magics listed with the items); with none of those, no equipped magic and no weapon magic the magic gauge is
   kept empty. */
int Player_KnowsItemMenuMagic(void) {
    int i;
    int j;
    int k;
    FxTableEntry *effect;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] != 0) {
                k = 15 * i + 3 * j + 1;
                effect = &fx_table[k];
                if (effect->unk102 == 1) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
