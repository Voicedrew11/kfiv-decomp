#include "map/map.h"
#include "collision/collision.h"
#include "player/player.h"
#include "entity/entity.h"
#include "effect/effect.h"
#include "math/vu0.h"
#include "enemy/enemy.h"
#include <math.h>
#include "save/save_state.h"

#define PLAYER_STEP_HEIGHT 16.666666f

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

/* Tests the player against the collision meshes of the records around him: what he touches, the walls that
   push him back, the water surface he is in and the floor he stands on. Returns TRUE when a floor was found. */
int Player_CollideWithMap(void) {
    // match: the scratchpad points through pointers (retail forms each address anew), and the order of these
    // declarations decides which stack slot each value that does not fit a register gets
    Vector4 *wall_point = &COLLISION_SCRATCH->wall_point;
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

    step_bit = (s32)(player_state.status_bits >> 33) & 1;
    stepping = step_bit; // match: tested as the bit, kept as a byte
    if (step_bit) {
        if (player_state.step_remainder <= player_state.step_per_frame) {
            player_state.position.y -= player_state.step_remainder;
            player_state.status_bits &= ~(0x8000L << 18);
            player_state.step_per_frame = 0.0f;
            player_state.step_remainder = 0.0f;
        } else {
            player_state.step_remainder -= player_state.step_per_frame;
            player_state.position.y -= player_state.step_per_frame;
        }
    }
    floor_height = player_no_floor_height[0];
    Vector_Copy(wall_point, &player_state.position);
    found = FALSE;
    wall_point->y -= 1.0f;
    Vector_Copy(floor_point, &player_state.position);
    floor_point->y -= 75.0f;
    reach = player_state.fall_speed + 75.0f;
    Vector_Copy(&raised_point, &player_state.position);
    floor_map = current_map_slot;
    player_state.floor_material_before_search = player_state.floor_material = 6;
    player_state.collision_base_y = 0.0f;
    raised_point.y += 3.0f; // match: the raised point is never used
    for (map = 0; map < 2; map++) {
        slot = current_map_slot == map ? &map_slots[0] : &map_slots[1];
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            model = (ModelInstance *)map_slot_memory[current_map_slot == map ? 0 : 1].record_models;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++, model++) {
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
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
                if (player_state.ground_effect_active == 0) {
                    strips_left = mesh->hazard_strip_count;
                    strip = mesh->hazard_strips;
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        do {
                            Collision_DropOnTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                          &player_state.position);
                            if (COLLISION_RESULT->hit != 0 && player_state.position.y - 230.0f <= COLLISION_RESULT->point[1]
                                && COLLISION_RESULT->point[1] <= player_state.position.y) {
                                surface = Collision_GetSurfaceEffect(strip_point->material_first, strip_point->material_second, strip_point->material_third);
                                if (surface != 0) {
                                    player_state.ground_effect_kind = surface;
                                    player_state.ground_effect_active = 1;
                                } else {
                                    player_state.ground_kills = 1;
                                }
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
                strips_left = mesh->wall_strip_count;
                strip = mesh->wall_strips;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        Collision_CapsuleTriangle((f32 *)strip_point[0].position, (f32 *)strip_point[1].position,
                                      (f32 *)strip_point[2].position, (f32 *)&COLLISION_SCRATCH->wall_point, 100.0f, 230.0f);
                        if (COLLISION_RESULT->hit != 0) {
                            COLLISION_SCRATCH->push.x = COLLISION_SCRATCH->wall_point.x - COLLISION_RESULT->point[0];
                            COLLISION_SCRATCH->push.y = 0.0f;
                            COLLISION_SCRATCH->push.z = COLLISION_SCRATCH->wall_point.z - COLLISION_RESULT->point[2];
                            Vector_Normalize(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push);
                            Vector_Scale(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push, 100.0f);
                            COLLISION_SCRATCH->wall_point.x = COLLISION_RESULT->point[0] + COLLISION_SCRATCH->push.x;
                            COLLISION_SCRATCH->wall_point.z = COLLISION_RESULT->point[2] + COLLISION_SCRATCH->push.z;
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
                if (map == 0) {
                    strips_left = mesh->water_strip_count;
                    strip = mesh->water_strips;
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        do {
                            Collision_DropOnTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                          &player_state.position);
                            if (COLLISION_RESULT->hit != 0
                                && COLLISION_RESULT->point[1] <= player_state.position.y + player_state.fall_speed
                                && (!((s32)(player_state.status_bits >> 38) & 1) || COLLISION_RESULT->point[1] < player_state.water_surface_y)) {
                                player_state.water_surface_y = COLLISION_RESULT->point[1];
                                player_state.status_bits |= 0x8000L << 23;
                                if (COLLISION_RESULT->point[1] <= player_state.position.y - 220.0f) {
                                    player_state.status_bits |= 0x8000L << 24;
                                } else {
                                    player_state.status_bits &= ~(0x8000L << 24);
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
                if (player_state.floor_map_object != 0
                    && (MAP_OBJECT_DEF_FLAGS(player_state.floor_map_object->definition) & MAP_OBJECT_DEF_CARRIES)) {
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
                            player_state.record_flag_block = record->flag_block_bit;
                            player_state.map_index = i;
                            player_state.floor_material = Collision_GetFloorMaterial(strip_point->material_first, strip_point->material_second, strip_point->material_third);
                            floor_map = current_map_slot != map;
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
            if (drop < PLAYER_STEP_HEIGHT) {
                player_state.position.y = floor_height;
            } else {
                player_state.status_bits |= 0x8000L << 18;
                player_state.step_per_frame = PLAYER_STEP_HEIGHT;
                player_state.position.y -= PLAYER_STEP_HEIGHT;
                player_state.step_remainder = drop - PLAYER_STEP_HEIGHT + 5.0f;
            }
            player_state.fall_speed = 0.0f;
        } else if (player_state.death_timer == 0) {
            player_state.position.y += player_state.fall_speed;
        } else {
            player_state.position.y += player_state.fall_speed * (f32)(60 - player_state.death_timer) / 60.0f;
        }
    }
    if (((s32)(player_state.status_bits >> 33) & 1) || found) {
        player_state.status_bits &= ~(0x8000L << 19);
    }
    if (!((s32)(player_previous.status_bits >> 38) & 1) && ((s32)(player_state.status_bits >> 38) & 1)) {
        player_state.fall_speed = 0.0f;
        player_state.status_bits &= ~(0x8000L << 19);
    }
    if (player_previous.ground_effect_kind != 4 && player_state.ground_effect_kind == 4) {
        player_state.fall_speed = 0.0f;
        player_state.status_bits &= ~(0x8000L << 19);
    }
    if (player_state.ground_effect_active == 0) {
        player_state.ground_effect_frames = 0;
    } else if (player_previous.ground_effect_active != 0 && player_previous.ground_effect_kind == player_state.ground_effect_kind) {
        player_state.ground_effect_frames++;
    } else {
        player_state.ground_effect_frames = 0;
    }
    Map_MakeCurrent(floor_map);
    return found;
}

/* Keeps the player 100 units away from the wall triangles (CollisionMesh.wall_strips) of the records around him:
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
        slot = current_map_slot == map ? &map_slots[0] : &map_slots[1];
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
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
                strips_left = mesh->wall_strip_count;
                strip = mesh->wall_strips;
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
   the records around him and notes its material in PlayerState.floor_material. */
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

    lowest = largest_float[0];
    Vector_Copy(&COLLISION_SCRATCH->point, &player_state.position);
    for (map = 0; map < 2; map++) {
        slot = current_map_slot == map ? &map_slots[0] : &map_slots[1];
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
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
                            player_state.floor_material = Collision_GetFloorMaterial(strip_point->material_first, strip_point->material_second, strip_point->material_third);
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

    player_state.stepped_on_entity = 0;
    if (climb) {
        Vector_Copy(old_position, &player_state);
        player_state.position.y += 1.0f;
        reach = player_state.fall_speed + 75.0f;
        if (!((s32)(player_state.status_bits >> 33) & 1)) {
            for (entity = active_entities_head; entity != NULL; entity = entity->next) {
                if (!(ENTITY_FLAG_BITS(entity)->live)) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->dying >= 2) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->no_player_collision) {
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
                if (!ENTITY_PARAMS_IS_SPHERE(entity->params)) {
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
                if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FLY) {
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
                player_state.stepped_on_entity = entity;
            }
        }
        player_state.position.y -= 1.0f;
        if (stepped) {
            step = PLAYER_STEP_HEIGHT;
            rise = old_position[1] - player_state.position.y;
            if (step < rise) {
                player_state.position.y = old_position[1] - step;
                player_state.step_remainder = rise - step + 5.0f;
                player_state.step_per_frame = step;
                player_state.status_bits |= 1L << 33;
            }
            player_state.fall_speed = 0.0f;
            player_state.status_bits &= ~(1L << 34);
        }
    }
    for (entity = active_entities_head; entity != NULL; entity = entity->next) {
        if (!(ENTITY_FLAG_BITS(entity)->live)) {
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->dying >= 2) {
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->no_player_collision) {
            continue;
        }
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FLY) {
            continue;
        }
        switch (entity->spawn_info->type_id) {
        case 0x11:
        case 0x14:
        case 0x97:
        case 0x9B:
            ENTITY_MODEL(entity)->update_bone_points(ENTITY_MODEL(entity));
            body = entity->params->body_points;
            for (i = 0; i < 4; i++, body++) {
                COLLISION_RESULT->hit = 0;
                if (!(body->flags_or_model_point.flags & ENTITY_BODY_USED)) {
                    continue;
                }
                if (entity->spawn_info->type_id == 0x9B && i != 3) {
                    continue;
                }
                point = (void *)ENTITY_MODEL(entity)->get_point(ENTITY_MODEL(entity), body->flags_or_model_point.point);
                Vector_Copy(position, point);
                if (!(body->flags_or_model_point.flags & ENTITY_BODY_ROUND)) {
                    radius = body->radius * ENTITY_MODEL(entity)->scale_x;
                    height = body->height * ENTITY_MODEL(entity)->scale_y;
                } else {
                    radius = body->radius * ENTITY_MODEL(entity)->scale_x;
                    position[1] += radius;
                    height = radius + radius;
                }
                push = 100.0f;
                Collision_CylinderCylinder(&player_state.position.x, push, 230.0f, position, radius, height);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_PLAYER_OUT();
                }
            }
            break;
        default:
            push = 100.0f;
            Collision_CylinderCylinder(&player_state.position.x, push, 230.0f, entity->model->position, entity->radius, entity->height);
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

    player_state.floor_map_object = 0;
    player_state.touched_map_object = 0;
    if (climb) {
        Vector_Copy(old_position, &player_state);
        player_state.position.y += 1.0f;
        reach = player_state.fall_speed + 75.0f;
        if (player_previous.floor_map_object != 0) {
            player_state.position.y += 30.0f;
            reach += 30.0f;
        }
        if (!((s32)(player_state.status_bits >> 33) & 1)) {
            hit = Collision_StandOnObjects((f32 *)&player_state, 1, reach, 100.0f);
        }
        if (hit) {
            step = PLAYER_STEP_HEIGHT;
            rise = old_position[1] - player_state.position.y;
            if (step < rise && player_previous.floor_map_object != player_state.floor_map_object) {
                player_state.position.y = old_position[1] - step;
                player_state.step_remainder = rise - step + 5.0f;
                player_state.step_per_frame = step;
                player_state.status_bits |= 1L << 33;
            }
            player_state.fall_speed = 0.0f;
            player_state.status_bits &= ~(1L << 34);
        } else {
            Vector_Copy(&player_state, old_position);
        }
    }
    if (!((s32)(player_state.status_bits >> 33) & 1)) {
        Collision_PushOutOfObjects((f32 *)&player_state, 0, 230.0f, 100.0f);
    }
    if (player_state.touched_map_object != 0 && player_state.touched_map_object != player_previous.touched_map_object) {
        switch ((s16)player_state.touched_map_object->source->touch_status_flag) {
        case 1:
            player_state.timed_flags[0] = 1800;
            player_state.ailment0_start_frame = frame_counter;
            break;
        case 2:
            player_state.timed_flags[1] = 150;
            player_state.ailment1_start_frame = frame_counter;
            break;
        case 3:
            player_state.timed_flags[2] = 3600;
            player_state.ailment2_start_frame = frame_counter;
            break;
        case 4:
            player_state.timed_flags[3] = 5400;
            player_state.ailment3_start_frame = frame_counter;
            break;
        case 5:
            player_state.timed_flags[4] = 1800;
            player_state.ailment4_start_frame = frame_counter;
            break;
        case 6:
            player_state.timed_flags[5] = 18000;
            player_state.ailment5_start_frame = frame_counter;
            break;
        }
        Player_FlashDamage();
    }
    if (player_state.floor_map_object != 0 && player_state.floor_map_object->source->def_index == 0x165) {
        player_state.floor_material = 2;
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
                if (fx_table[15 * i + 3 * j + 1].item_menu_magic == 0) {
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
                if (effect->item_menu_magic == 1) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
