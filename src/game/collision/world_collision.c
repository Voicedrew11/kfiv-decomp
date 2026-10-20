#include "collision/collision.h"
#include "map/map.h"
#include "map_object/map_object.h"
#include "entity/entity.h"
#include "player/player.h"
#include "model/model.h"
#include "core/file.h"
#include "enemy/enemy.h"
#include <libvu0.h>
#include <math.h>
#include "math/angle.h"
#include "math/vu0.h"

/* MapObjectUnk04.kind values this file tests; the others are in map_object.h */
#define MAP_OBJECT_KIND_PITFALL             18      /* a pit that opens under the player (mechanism.c) */

/* cos 20 degrees: a quad whose normal is within 20 degrees of vertical is a floor or a ceiling */
#define QUAD_FLOOR_NORMAL_Y 0.93969f

typedef void (*MapObjectSweepTest)(MapObject *, f32 *, f32, f32);
typedef void (*MapObjectFanTest)(MapObject *, Vector4 *, f32, f32, f32, f32, f32, f32);
typedef void (*MapObjectCircleTest)(MapObject *, f32 *, f32, f32);
/* what each kind's test does when it finds something: the answer is TRUE, and the player state
   notes the object when the point tested is the player's position */
#define NOTE_FOUND(object) \
    result = TRUE; \
    if (centre == (f32 *)&player_state) { \
        ((PlayerState *)centre)->floor_map_object = (object); \
    }
// match: the last test is called for its value, which keeps it an ordinary call
typedef int (*WallTest)(void *, void *, f32, void *, f32);
// match: the last test is called for its value, which keeps it an ordinary call
typedef int (*CornerTest)(void *, f32, f32, void *, f32, f32);
/* tests the circle against the wall from one point to the next; the function ends on a hit */
#define TEST_WALL(from, to) \
    Collision_CylinderEdge(from, to, height, centre, radius, push); \
    if (COLLISION_RESULT->hit != 0) { \
        return; \
    }
/* the same against the post standing at a point */
#define TEST_POST(point) \
    Collision_CylinderCylinder(centre, push, radius, point, 0.0f, height); \
    if (COLLISION_RESULT->hit != 0) { \
        return; \
    }
/* notes on which side of the lines from the origin to the two ends the point lies */
#define TEST_SIDES(point) \
    if ((first_x - origin->x) * ((point)[2] - origin->z) - (first_z - origin->z) * ((point)[0] - origin->x) > 0.0f) { \
        first_sides |= 1; \
    } \
    if ((second_x - origin->x) * ((point)[2] - origin->z) - (second_z - origin->z) * ((point)[0] - origin->x) < 0.0f) { \
        second_sides |= 1; \
    }
/* whether the point lies outside the line from the origin to the first end */
#define OUTSIDE_FIRST(point) \
    ((first_x - origin->x) * ((point)->z - origin->z) - (first_z - origin->z) * ((point)->x - origin->x) > 0.0f)
/* whether the point lies outside the line from the origin to the second end */
#define OUTSIDE_SECOND(point) \
    ((second_x - origin->x) * ((point)->z - origin->z) - (second_z - origin->z) * ((point)->x - origin->x) < 0.0f)
/* whether a model point lies left of the line from the origin to (line_x, line_z), seen from above */
#define LEFT_OF(point, line_x, line_z) \
    (0.0f < ((line_x) - origin->x) * ((point)->z - origin->z) - ((line_z) - origin->z) * ((point)->x - origin->x))
/* whether it lies right of that line */
#define RIGHT_OF(point, line_x, line_z) \
    (((line_x) - origin->x) * ((point)->z - origin->z) - ((line_z) - origin->z) * ((point)->x - origin->x) < 0.0f)
/* moves the centre out of what was hit, to the given distance from it */
#define PUSH_OUT(distance) \
    centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (distance); \
    centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (distance)
#define SAME_POINT(point, position) \
    ((point)[0] == (position).x && (point)[1] == (position).y && (point)[2] == (position).z)

/* Drops a vertical line through `point` onto a triangle: on a hit the scratchpad result gets the
   height where it meets the triangle and how far that is from the point (positive: below it, as y grows
   downward). */
void Collision_DropOnTriangle(void *corner_a, void *corner_b, void *corner_c, void *point) {
    f32 hit_point[4];
    f32 edge_a[4];
    f32 edge_c[4];
    f32 offset[4];
    f32 normal[4];
    f32 scalar; // match: one variable for the normal's y, then the rise, then the area, as retail keeps one register
    f32 along_c;
    f32 along_a;

    COLLISION_RESULT->hit = 0;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "lqc2 $vf18, 0(%2)\n"
        "vnop\n"
        "vnop\n"
        "vsub.xyz $vf19, $vf16, $vf17\n"
        "vsub.xyz $vf20, $vf18, $vf17\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vopmula.xyz ACC, $vf19, $vf20\n"
        "vopmsub.xyz $vf16, $vf20, $vf19\n"
        "sqc2 $vf19, 0(%3)\n"
        "sqc2 $vf20, 0(%4)\n"
        "vnop\n"
        "sqc2 $vf16, 0(%5)\n"
        : : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(edge_a), "r"(edge_c), "r"(normal));

    scalar = normal[1];
    if (-COLLISION_PARALLEL_EPSILON < scalar && scalar < COLLISION_PARALLEL_EPSILON) {
        return;
    }
    scalar = (normal[0] * ((f32 *)corner_a)[0] + scalar * ((f32 *)corner_a)[1] + normal[2] * ((f32 *)corner_a)[2] +
              (-normal[0] * ((f32 *)point)[0] - scalar * ((f32 *)point)[1] - normal[2] * ((f32 *)point)[2])) /
             scalar;
    hit_point[0] = ((f32 *)point)[0];
    hit_point[1] = ((f32 *)point)[1] + scalar;
    hit_point[2] = ((f32 *)point)[2];
    COLLISION_RESULT->distance = scalar;
    Vector_Subtract(offset, hit_point, corner_b);
    scalar = edge_c[0] * edge_a[2] - edge_c[2] * edge_a[0];
    along_c = offset[0] * edge_a[2] - offset[2] * edge_a[0];
    along_a = offset[2] * edge_c[0] - offset[0] * edge_c[2];
    along_c /= scalar;
    along_a /= scalar;
    if (along_c < negative_zero_along_limit[0]) {
        return;
    }
    if (along_a < negative_zero_along_limit[0]) {
        return;
    }
    if (along_c + along_a <= 1.0f) {
        COLLISION_RESULT->hit = 1;
        COLLISION_RESULT->point[1] = hit_point[1];
    }
}

/* Collision_DropOnTriangle with a limit: the triangle must lie at or below the point (y grows downward), by
   no more than reach. */
void Collision_DropOnTriangleWithin(void *corner_a, void *corner_b, void *corner_c, void *point, f32 reach) {
    f32 hit_point[4];
    f32 edge_a[4];
    f32 edge_c[4];
    f32 offset[4];
    f32 normal[4];
    f32 scalar; // match: one variable for the normal's y, then the rise, then the area, as retail keeps one register
    f32 along_c;
    f32 along_a;

    COLLISION_RESULT->hit = 0;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "lqc2 $vf18, 0(%2)\n"
        "vnop\n"
        "vnop\n"
        "vsub.xyz $vf19, $vf16, $vf17\n"
        "vsub.xyz $vf20, $vf18, $vf17\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vopmula.xyz ACC, $vf19, $vf20\n"
        "vopmsub.xyz $vf16, $vf20, $vf19\n"
        "sqc2 $vf19, 0(%3)\n"
        "sqc2 $vf20, 0(%4)\n"
        "vnop\n"
        "sqc2 $vf16, 0(%5)\n"
        : : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(edge_a), "r"(edge_c), "r"(normal));

    scalar = normal[1];
    if (-COLLISION_PARALLEL_EPSILON < scalar && scalar < COLLISION_PARALLEL_EPSILON) {
        return;
    }
    scalar = (normal[0] * ((f32 *)corner_a)[0] + scalar * ((f32 *)corner_a)[1] + normal[2] * ((f32 *)corner_a)[2] +
              (-normal[0] * ((f32 *)point)[0] - scalar * ((f32 *)point)[1] - normal[2] * ((f32 *)point)[2])) /
             scalar;
    if (scalar < negative_zero_drop_within_limit[0]) {
        return;
    }
    if (reach < scalar) {
        return;
    }
    hit_point[0] = ((f32 *)point)[0];
    hit_point[1] = ((f32 *)point)[1] + scalar;
    hit_point[2] = ((f32 *)point)[2];
    COLLISION_RESULT->distance = scalar;
    Vector_Subtract(offset, hit_point, corner_b);
    scalar = edge_c[0] * edge_a[2] - edge_c[2] * edge_a[0];
    along_c = offset[0] * edge_a[2] - offset[2] * edge_a[0];
    along_a = offset[2] * edge_c[0] - offset[0] * edge_c[2];
    along_c /= scalar;
    along_a /= scalar;
    if (along_c < negative_zero_drop_within_limit[0]) {
        return;
    }
    if (along_a < negative_zero_drop_within_limit[0]) {
        return;
    }
    if (along_c + along_a <= 1.0f) {
        COLLISION_RESULT->hit = 1;
        COLLISION_RESULT->point[1] = hit_point[1];
    }
}

/* Looks for the floor of the loaded maps' records nearest below `point_arg` (y grows downward), by no more
   than `reach`. Returns TRUE and leaves that hit, with its floor material, in the scratchpad result when
   there is one. */
int Collision_FindFloorBelow(f32 reach, void *point_arg) {
    CollisionResultCopy best;
    s32 found;
    f32 *point = point_arg;
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

    COLLISION_RESULT->hit = 0;
    lowest = largest_float_floor_lowest_seed[0];
    found = FALSE;
    for (map = 0; map < 2; map++) {
        slot = current_map_slot == map ? &map_slots[0] : &map_slots[1];
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
                    continue;
                }
                if (point[0] < record->lowest[0] || record->highest[0] < point[0] || point[2] < record->lowest[2] ||
                    record->highest[2] < point[2]) {
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
                        Collision_DropOnTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position, point);
                        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest &&
                            point[1] <= COLLISION_RESULT->point[1] && COLLISION_RESULT->point[1] <= point[1] + reach) {
                            found = TRUE;
                            COLLISION_RESULT->floor_material = Collision_GetFloorMaterial(strip_point->material_first, strip_point->material_second, strip_point->material_third);
                            best = *COLLISION_RESULT_COPY;
                            lowest = COLLISION_RESULT->point[1];
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    if (found) {
        // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
        *&COLLISION_RESULT_COPY->doublewords[0] = best.doublewords[0];
        *&COLLISION_RESULT_COPY->doublewords[1] = best.doublewords[1];
        *&COLLISION_RESULT_COPY->doublewords[2] = best.doublewords[2];
        *&COLLISION_RESULT_COPY->doublewords[3] = best.doublewords[3];
        *&COLLISION_RESULT_COPY->doublewords[4] = best.doublewords[4];
        *&COLLISION_RESULT_COPY->doublewords[5] = best.doublewords[5];
    }
    return found;
}

/* Collision_FindFloorBelow for the records' water surfaces (CollisionMesh.water_strips, where Player_CollideWithMap
   finds the water): the nearest one below `point_arg`, by no more than `reach`. Returns TRUE and leaves that
   hit in the scratchpad result when there is one. */
int Collision_FindWaterBelow(f32 reach, void *point_arg) {
    CollisionResultCopy best;
    s32 found;
    f32 *point = point_arg;
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

    COLLISION_RESULT->hit = 0;
    lowest = largest_float_water_lowest_seed[0];
    found = FALSE;
    for (map = 0; map < 2; map++) {
        slot = current_map_slot == map ? &map_slots[0] : &map_slots[1];
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
                    continue;
                }
                if (point[0] < record->lowest[0] || record->highest[0] < point[0] || point[2] < record->lowest[2] ||
                    record->highest[2] < point[2]) {
                    continue;
                }
                Collision_TransformMeshPoints(record->model);
                mesh = (CollisionMesh *)record->model->strips;
                strips_left = mesh->water_strip_count;
                strip = mesh->water_strips;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        Collision_DropOnTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position, point);
                        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest &&
                            point[1] <= COLLISION_RESULT->point[1] && COLLISION_RESULT->point[1] <= point[1] + reach) {
                            found = TRUE;
                            COLLISION_RESULT->floor_material = Collision_GetFloorMaterial(strip_point->material_first, strip_point->material_second, strip_point->material_third);
                            best = *COLLISION_RESULT_COPY;
                            lowest = COLLISION_RESULT->point[1];
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    if (found) {
        // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
        *&COLLISION_RESULT_COPY->doublewords[0] = best.doublewords[0];
        *&COLLISION_RESULT_COPY->doublewords[1] = best.doublewords[1];
        *&COLLISION_RESULT_COPY->doublewords[2] = best.doublewords[2];
        *&COLLISION_RESULT_COPY->doublewords[3] = best.doublewords[3];
        *&COLLISION_RESULT_COPY->doublewords[4] = best.doublewords[4];
        *&COLLISION_RESULT_COPY->doublewords[5] = best.doublewords[5];
    }
    return found;
}

/* Sweeps a point from `position_arg` along `direction` times `scale` through the loaded maps'
   records and leaves the nearest hit in the scratchpad result. Four of each mesh's strip lists
   are tested (strip_list_indices names them); a strip's triangles alternate in winding. */
void Collision_CastRayAtMap(f32 scale, void *position_arg, void *direction) {
    f32 step[4];
    f32 lowest[4];
    f32 highest[4];
    CollisionResultCopy best;
    f32 *position = position_arg;
    MapSlot *slot;
    MapRecordBounds *record;
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 list;
    s32 strips_left;
    s32 points_left;
    s32 forward;

    Vector_Scale(step, direction, scale);
    best.result.hit = 0;
    COLLISION_RESULT->hit = 0;
    best.result.distance = largest_float_ray_best_distance[0];
    for (map = 0; map < 2; map++) {
        slot = map_slots;
        if (current_map_slot != map) {
            slot++;
        }
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
                    continue;
                }
                Vector_Copy(lowest, record->lowest);
                Vector_Copy(highest, record->highest);
                if (0.0f < step[0]) {
                    lowest[0] -= step[0];
                } else {
                    highest[0] -= step[0];
                }
                if (0.0f < step[1]) {
                    lowest[1] -= step[1];
                } else {
                    highest[1] -= step[1];
                }
                if (0.0f < step[2]) {
                    lowest[2] -= step[2];
                } else {
                    highest[2] -= step[2];
                }
                if (position[0] < lowest[0] || highest[0] < position[0] || position[1] < lowest[1] ||
                    highest[1] < position[1] || position[2] < lowest[2] || highest[2] < position[2]) {
                    continue;
                }
                Collision_TransformMeshPoints(record->model);
                for (list = 0; list < 4; list++) {
                    mesh = (CollisionMeshLists *)record->model->strips;
                    strips_left = mesh->strip_counts[strip_list_indices[list]];
                    strip = mesh->strip_lists[strip_list_indices[list]];
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        forward = TRUE;
                        do {
                            if (forward) {
                                Collision_RayTriangle(strip_point[0].position, strip_point[1].position,
                                              strip_point[2].position, position, direction, scale);
                            } else {
                                Collision_RayTriangle(strip_point[0].position, strip_point[2].position,
                                              strip_point[1].position, position, direction, scale);
                            }
                            forward ^= 1;
                            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < best.result.distance) {
                                best = *COLLISION_RESULT_COPY;
                                best.result.floor_material = Collision_GetFloorMaterial(strip_point->material_first, strip_point->material_second, strip_point->material_third);
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
            }
        }
    }
    // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
    *&COLLISION_RESULT_COPY->doublewords[0] = best.doublewords[0];
    *&COLLISION_RESULT_COPY->doublewords[1] = best.doublewords[1];
    *&COLLISION_RESULT_COPY->doublewords[2] = best.doublewords[2];
    *&COLLISION_RESULT_COPY->doublewords[3] = best.doublewords[3];
    *&COLLISION_RESULT_COPY->doublewords[4] = best.doublewords[4];
    *&COLLISION_RESULT_COPY->doublewords[5] = best.doublewords[5];
}

/* Collision_CastRayAtMap with the strip lists to test given as a mask: bit n set tests list n of each
   mesh's seven (CollisionMeshLists). */
void Collision_CastRayAtMapLists(f32 scale, void *position_arg, void *direction, s32 lists) {
    f32 step[4];
    f32 lowest[4];
    f32 highest[4];
    CollisionResultCopy best;
    f32 *position = position_arg;
    MapSlot *slot;
    MapRecordBounds *record;
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 list;
    s32 list_bit;
    s32 strips_left;
    s32 points_left;
    s32 forward;

    Vector_Scale(step, direction, scale);
    best.result.hit = 0;
    COLLISION_RESULT->hit = 0;
    best.result.distance = largest_float_ray_lists_best_distance[0];
    for (map = 0; map < 2; map++) {
        slot = map_slots;
        if (current_map_slot != map) {
            slot++;
        }
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++) {
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
                    continue;
                }
                Vector_Copy(lowest, record->lowest);
                Vector_Copy(highest, record->highest);
                if (0.0f < step[0]) {
                    lowest[0] -= step[0];
                } else {
                    highest[0] -= step[0];
                }
                if (0.0f < step[1]) {
                    lowest[1] -= step[1];
                } else {
                    highest[1] -= step[1];
                }
                if (0.0f < step[2]) {
                    lowest[2] -= step[2];
                } else {
                    highest[2] -= step[2];
                }
                if (position[0] < lowest[0] || highest[0] < position[0] || position[1] < lowest[1] ||
                    highest[1] < position[1] || position[2] < lowest[2] || highest[2] < position[2]) {
                    continue;
                }
                list_bit = 1; // match: set ahead of the call, so it takes a saved register
                Collision_TransformMeshPoints(record->model);
                for (list = 0; list < 7; list++, list_bit <<= 1) {
                    if (!(lists & list_bit)) {
                        continue;
                    }
                    mesh = (CollisionMeshLists *)record->model->strips;
                    strips_left = mesh->strip_counts[list];
                    strip = mesh->strip_lists[list];
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        forward = TRUE;
                        do {
                            if (forward) {
                                Collision_RayTriangle(strip_point[0].position, strip_point[1].position,
                                              strip_point[2].position, position, direction, scale);
                            } else {
                                Collision_RayTriangle(strip_point[0].position, strip_point[2].position,
                                              strip_point[1].position, position, direction, scale);
                            }
                            forward ^= 1;
                            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < best.result.distance) {
                                best = *COLLISION_RESULT_COPY;
                                best.result.floor_material = Collision_GetFloorMaterial(strip_point->material_first, strip_point->material_second, strip_point->material_third);
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
            }
        }
    }
    // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
    *&COLLISION_RESULT_COPY->doublewords[0] = best.doublewords[0];
    *&COLLISION_RESULT_COPY->doublewords[1] = best.doublewords[1];
    *&COLLISION_RESULT_COPY->doublewords[2] = best.doublewords[2];
    *&COLLISION_RESULT_COPY->doublewords[3] = best.doublewords[3];
    *&COLLISION_RESULT_COPY->doublewords[4] = best.doublewords[4];
    *&COLLISION_RESULT_COPY->doublewords[5] = best.doublewords[5];
}

/* Sweeps a point from `position` to `target` through the loaded maps' records and leaves the
   nearest hit in the scratchpad result. Two of each mesh's strip lists are tested (segment_strip_lists
   names them); a strip's triangles alternate in winding. */
void Collision_CastSegmentAtMap(f32 *position, f32 *target) {
    f32 direction[4];
    f32 lowest[4];
    f32 highest[4];
    CollisionResultAligned best;
    MapSlot *slot;
    MapRecordBounds *record;
    ModelInstanceBlock *model;
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    f32 length;
    s32 map;
    s32 record_count;
    s32 i;
    s32 list;
    s32 strips_left;
    s32 points_left;
    s32 forward;

    Vector_Subtract(direction, target, position);
    length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
    Vector_Scale(direction, direction, 1.0f / length);
    best.hit = 0;
    COLLISION_RESULT->hit = 0;
    best.distance = largest_float_segment_best_distance[0];
    for (map = 0; map < 2; map++) {
        slot = map_slots;
        if (current_map_slot != map) {
            slot++;
        }
        if (slot->loaded != 0) {
            record_count = slot->header->record_count;
            record = (MapRecordBounds *)slot->records;
            model = map_slot_record_models[current_map_slot == map ? 0 : 22];
            for (i = 0; i < record_count; i++, model++, record++) {
                if (model->model.strips == NULL) {
                    continue;
                }
                if (record->record_flags & MAP_RECORD_HIDDEN) {
                    continue;
                }
                Vector_Copy(lowest, record->lowest);
                Vector_Copy(highest, record->highest);
                // match: the box is widened by the unit direction, not by the whole way to the target
                if (0.0f < direction[0]) {
                    lowest[0] -= direction[0];
                } else {
                    highest[0] -= direction[0];
                }
                if (0.0f < direction[1]) {
                    lowest[1] -= direction[1];
                } else {
                    highest[1] -= direction[1];
                }
                if (0.0f < direction[2]) {
                    lowest[2] -= direction[2];
                } else {
                    highest[2] -= direction[2];
                }
                if (position[0] < lowest[0] || highest[0] < position[0] || position[1] < lowest[1] ||
                    highest[1] < position[1] || position[2] < lowest[2] || highest[2] < position[2]) {
                    continue;
                }
                Collision_TransformMeshPoints(model);
                for (list = 0; list < 2; list++) {
                    mesh = (CollisionMeshLists *)model->model.strips;
                    strips_left = mesh->strip_counts[segment_strip_lists[list]];
                    strip = mesh->strip_lists[segment_strip_lists[list]];
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        forward = TRUE;
                        do {
                            if (forward) {
                                Collision_RayTriangle(strip_point[0].position, strip_point[1].position,
                                              strip_point[2].position, position, direction, length);
                            } else {
                                Collision_RayTriangle(strip_point[0].position, strip_point[2].position,
                                              strip_point[1].position, position, direction, length);
                            }
                            forward ^= 1;
                            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < best.distance) {
                                best = *COLLISION_RESULT_ALIGNED;
                                best.floor_material = Collision_GetFloorMaterial(strip_point->material_first, strip_point->material_second, strip_point->material_third);
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
            }
        }
    }
    // match: put back a doubleword at a time, each through *& (retail forms the address anew for every store)
    *&COLLISION_RESULT_COPY->doublewords[0] = ((CollisionResultCopy *)&best)->doublewords[0];
    *&COLLISION_RESULT_COPY->doublewords[1] = ((CollisionResultCopy *)&best)->doublewords[1];
    *&COLLISION_RESULT_COPY->doublewords[2] = ((CollisionResultCopy *)&best)->doublewords[2];
    *&COLLISION_RESULT_COPY->doublewords[3] = ((CollisionResultCopy *)&best)->doublewords[3];
    *&COLLISION_RESULT_COPY->doublewords[4] = ((CollisionResultCopy *)&best)->doublewords[4];
    *&COLLISION_RESULT_COPY->doublewords[5] = ((CollisionResultCopy *)&best)->doublewords[5];
}

/* Tests a ray (from `origin` along `direction`, no further than `reach` times it) against a map object with
   the test that fits its kind: the box (1), the double door (2), the quad (17), the pitfall (18), else its
   collision mesh. The result is left in COLLISION_RESULT. */
void Collision_HitRayObj(MapObject *object, f32 *origin, f32 *direction, f32 reach) {
    COLLISION_RESULT->hit = 0;
    switch (object->definition->kind) {
    case MAP_OBJECT_KIND_DOOR:
        Collision_HitRayBoxObj(object, origin, direction, reach);
        break;
    case MAP_OBJECT_KIND_DOUBLE_DOOR:
        Collision_HitRayDoubleDoorObj(object, origin, direction, reach);
        break;
    case MAP_OBJECT_KIND_SIGNAL_DOOR:
        Collision_HitRayQuadObj((AnimObject *)object, origin, direction, reach);
        break;
    case MAP_OBJECT_KIND_PITFALL:
        Collision_HitRayPitfallObj(object, origin, direction, reach);
        break;
    default:
        if (object->model->strips != NULL) {
            Collision_HitRayMeshObj(object, origin, direction, reach);
        }
        break;
    }
}

/* Tests a point (with a reach) against a map object: first the object's own reach around its
   model, then the test that fits the object's kind. The result is left in COLLISION_RESULT. */
void Collision_HitSphereObj(MapObject *object, f32 *point, f32 reach) {
    ModelInstance *model = object->model;
    f32 delta_x;
    f32 delta_z;
    f32 object_reach;

    COLLISION_RESULT->floor_material = 6;
    COLLISION_RESULT->hit = 0;
    delta_x = ABS(model->position.x - point[0]);
    object_reach = object->reach_radius;
    if (delta_x > object_reach) {
        return;
    }
    delta_z = ABS(model->position.z - point[2]);
    if (delta_z > object_reach) {
        return;
    }
    if (delta_x * delta_x + delta_z * delta_z > object_reach * object_reach) {
        return;
    }
    switch (object->definition->kind) {
    case MAP_OBJECT_KIND_DOOR:
        Collision_HitSphereBoxObj(object, point, reach);
        break;
    case MAP_OBJECT_KIND_DOUBLE_DOOR:
        Collision_HitSphereDoubleDoorObj(object, point, reach);
        break;
    case MAP_OBJECT_KIND_SIGNAL_DOOR:
        Collision_HitSphereQuadObj(object, point, reach);
        break;
    case MAP_OBJECT_KIND_PITFALL:
        Collision_HitSpherePitfallObj(object, point, reach);
        break;
    default:
        if (model->strips != NULL) {
            Collision_HitSphereMeshObj((CollisionOwner *)object, point, reach);
        }
        break;
    }
}

/* Tests a point (with two reaches) against a map object: first the object's own reach around
   its model, then the test that fits the object's kind. The result is left in COLLISION_RESULT. */
void Collision_HitCapsuleObj(MapObject *object, f32 *point, f32 reach, f32 height) {
    f32 delta_x;
    f32 delta_z;

    COLLISION_RESULT->hit = 0;
    delta_x = ABS(object->model->position.x - point[0]);
    if (delta_x > object->reach_radius) {
        return;
    }
    delta_z = ABS(object->model->position.z - point[2]);
    if (delta_z > object->reach_radius) {
        return;
    }
    if (sqrtf(delta_x * delta_x + delta_z * delta_z) > object->reach_radius) {
        return;
    }
    switch (object->definition->kind) {
    case MAP_OBJECT_KIND_DOOR:
        Collision_HitCapsuleBoxObj(object, point, reach, height);
        break;
    case MAP_OBJECT_KIND_DOUBLE_DOOR:
        Collision_HitCapsuleDoubleDoorObj(object, point, reach, height);
        break;
    case MAP_OBJECT_KIND_SIGNAL_DOOR:
        Collision_HitCapsuleQuadObj((CollisionOwner *)object, point, reach, height);
        break;
    case MAP_OBJECT_KIND_PITFALL:
        // match: the pitfall test is an empty function, but it is passed everything the others are
        ((MapObjectSweepTest)Collision_HitCapsulePitfallObj)(object, point, reach, height);
        break;
    default:
        if (object->model->strips != NULL) {
            Collision_HitCapsuleMeshObj(object, point, reach, height);
        }
        break;
    }
}

/* Tests a fan (a sector of a circle around `origin`, `spread` to each side of `heading`)
   against a map object: first the object's own reach around its model, then the test that fits
   the object's kind, which is given the fan's two edge points. The result is left in
   COLLISION_RESULT. */
void Collision_HitFanObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 spread, f32 heading) {
    f32 first_angle;
    f32 second_angle;
    f32 first_x;
    f32 first_z;
    f32 second_x;
    f32 second_z;
    f32 delta_x;
    f32 delta_z;
    f32 distance;

    COLLISION_RESULT->hit = 0;
    first_angle = Angle_Wrap(-heading + spread);
    second_angle = Angle_Wrap(-heading - spread);
    first_x = -sinf(first_angle);
    first_z = cosf(first_angle);
    second_x = -sinf(second_angle);
    second_z = cosf(second_angle);
    first_x += origin->x;
    first_z += origin->z;
    second_x += origin->x;
    second_z += origin->z;
    delta_x = ABS(object->model->position.x - origin->x);
    if (delta_x > object->reach_radius + push) {
        return;
    }
    delta_z = ABS(object->model->position.z - origin->z);
    if (delta_z > object->reach_radius + push) {
        return;
    }
    distance = sqrtf(delta_x * delta_x + delta_z * delta_z);
    if (distance > object->reach_radius + push) {
        return;
    }
    switch (object->definition->kind) {
    case MAP_OBJECT_KIND_DOOR:
        Collision_HitFanBoxObj(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    case MAP_OBJECT_KIND_DOUBLE_DOOR:
        Collision_HitFanDoubleDoorObj(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    case MAP_OBJECT_KIND_SIGNAL_DOOR:
        Collision_HitFanQuadObj(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    case MAP_OBJECT_KIND_PITFALL:
        // match: the pitfall test is an empty function, but it is passed everything the others are
        ((MapObjectFanTest)Collision_HitFanPitfallObj)(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    default:
        if (object->model->strips != NULL) {
            Collision_HitFanMeshObj(object, origin, radius, push, first_x, first_z, second_x, second_z);
        }
        break;
    }
}

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        if (current_map_slot != map) {
            return map_slot1_objects;
        }
        return map_slot_objects[0];
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        return map_object_counts[current_map_slot == map ? 0 : 1];
    }
    return 0;
}

/* Collides a circle with every map object of both loaded maps that is near enough. With
   `measure` set the distance to each object is worked out and kept in the object (unk18);
   otherwise the distances kept by an earlier call are used. */
void Collision_PushOutOfObjects(f32 *centre, s32 measure, f32 radius, f32 push) {
    MapObject *object;
    f32 reach;
    f32 delta_x;
    f32 delta_z;
    f32 distance;
    s32 map;
    s32 count;
    s32 i;

    COLLISION_RESULT->hit = 0;
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (object->reach_radius <= 0.0f) {
                continue;
            }
            reach = object->reach_radius + push;
            if (measure) {
                object->tested_point_distance = largest_float_tested_point_distance[0];
                delta_x = ABS(object->model->position.x - centre[0]);
                if (delta_x > reach) {
                    continue;
                }
                delta_z = ABS(object->model->position.z - centre[2]);
                if (delta_z > reach) {
                    continue;
                }
                distance = sqrtf(delta_x * delta_x + delta_z * delta_z);
                if (distance > reach) {
                    continue;
                }
                object->tested_point_distance = distance;
            } else if (object->tested_point_distance > reach) {
                continue;
            }
            switch (object->definition->kind) {
            case MAP_OBJECT_KIND_DOOR:
                Collision_PushOutOfBoxObj(object, centre, radius, push);
                break;
            case MAP_OBJECT_KIND_DOUBLE_DOOR:
                Collision_PushOutOfDoubleDoorObj(object, centre, radius, push);
                break;
            case MAP_OBJECT_KIND_SIGNAL_DOOR:
                Collision_PushOutOfQuadObj((CollisionOwner *)object, centre, radius, push);
                break;
            case MAP_OBJECT_KIND_PITFALL:
                // match: the pitfall test is an empty function, but it is passed everything the others are
                ((MapObjectCircleTest)Collision_PushOutOfPitfallObj)(object, centre, radius, push);
                break;
            default:
                if (object->model->strips != NULL) {
                    Collision_PushOutOfMeshObj(object, centre, radius, push);
                }
                break;
            }
        }
    }
}

/* Tests a point against every map object of both loaded maps that is near enough and that can
   be stood on (kinds 1 and 2 are left out); TRUE if any test says so. When the point is the
   player's position the object is noted in the player state. `measure` is as in Collision_PushOutOfObjects. */
s32 Collision_StandOnObjects(f32 *centre, s32 measure, f32 radius, f32 push) {
    MapObject *object;
    f32 reach;
    f32 delta_x;
    f32 delta_z;
    f32 distance;
    s32 map;
    s32 count;
    s32 i;
    s32 result = FALSE;
    f32 farthest = largest_float_stand_farthest_start[0];

    COLLISION_RESULT->hit = 0;
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (object->reach_radius <= 0.0f) {
                continue;
            }
            reach = object->reach_radius + push;
            if (measure) {
                object->tested_point_distance = farthest;
                delta_x = ABS(object->model->position.x - centre[0]);
                if (delta_x > reach) {
                    continue;
                }
                delta_z = ABS(object->model->position.z - centre[2]);
                if (delta_z > reach) {
                    continue;
                }
                distance = sqrtf(delta_x * delta_x + delta_z * delta_z);
                if (distance > reach) {
                    continue;
                }
                object->tested_point_distance = distance;
            } else if (object->tested_point_distance > reach) {
                continue;
            }
            switch (object->definition->kind) {
            case MAP_OBJECT_KIND_DOOR:
            case MAP_OBJECT_KIND_DOUBLE_DOOR:
                break;
            case MAP_OBJECT_KIND_SIGNAL_DOOR:
                if (Collision_StandOnQuadObj((CollisionOwner *)object, centre, radius)) {
                    NOTE_FOUND(object);
                }
                break;
            case MAP_OBJECT_KIND_PITFALL:
                if (Collision_StandOnPitfallObj(object, centre, radius)) {
                    NOTE_FOUND(object);
                }
                break;
            default:
                if (object->model->strips != NULL && Collision_StandOnMeshObj((CollisionOwner *)object, centre, radius)) {
                    NOTE_FOUND(object);
                }
                break;
            }
        }
    }
    return result;
}

/* Places the points of a model's collision mesh in the world: each is multiplied by the model's
   matrix and written to the scratchpad (COLLISION_WORLD_POINTS), three at a time and then the
   one or two left over. */
// match: nothing is returned, but the callers that read $2 after it need the int
int Collision_TransformMeshPoints(void *target) {
    ModelInstance *model = target;
    CollisionMesh *mesh;
    Vector4 *world = COLLISION_WORLD_POINTS;
    Vector4 *local;
    u32 count;
    s32 i;

    mesh = (CollisionMesh *)model->strips;
    Vu0_LoadMatrix(model->collision_matrix);
    count = mesh->point_count;
    local = (Vector4 *)mesh->points;
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
}

/* Tests a ray (from `origin` along `direction`, no further than `reach` times it) against every triangle of
   the strips of the object's model, if the object has MAP_OBJECT_IN_USE and the mesh unk0C is 4 or more; stops at
   the first hit. */
void Collision_HitRayMeshObj(MapObject *object, f32 *origin, f32 *direction, f32 reach) {
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 strips_left;
    s32 triangles_left;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        mesh = (CollisionMesh *)object->model->strips;
        if (mesh != NULL && mesh->point_count >= 4) {
            Collision_TransformMeshPoints(object->model);
            strip = mesh->strips;
            strips_left = mesh->strip_count;
            do {
                point = strip->points;
                triangles_left = strip->point_count - 2;
                do {
                    Collision_RayTriangle(point[0].position, point[1].position, point[2].position, origin, direction,
                                          reach);
                    if (COLLISION_RESULT->hit != 0) {
                        return;
                    }
                    point++;
                } while (--triangles_left != 0);
                strip = (CollisionStrip *)(point + 2);
            } while (--strips_left != 0);
        }
    }
}

/* Tests a ray against the four walls of a box-like map object (kind 1), two triangles a wall.
   Model points 0 to 3 are the bottom corners; the top ones lie above them at the height of point
   0x1F (y grows downward). Stops at the first triangle hit. */
void Collision_HitRayBoxObj(void *target, f32 *from, f32 *direction, float distance) {
    MapObject *object = target;
    Vector4 bottom[4];
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *top;
    f32 height;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    corner0 = (f32 *)object->model->get_point(object->model, 0);
    top = (f32 *)object->model->get_point(object->model, 0x1F);
    height = corner0[1] - top[1];
    corner1 = (f32 *)object->model->get_point(object->model, 1);
    corner2 = (f32 *)object->model->get_point(object->model, 2);
    corner3 = (f32 *)object->model->get_point(object->model, 3);
    Vector_Copy(&bottom[0], corner0);
    bottom[0].y -= height;
    Collision_RayTriangle(corner0, corner1, &bottom[0].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[1], corner1);
    bottom[1].y -= height;
    Collision_RayTriangle(corner1, &bottom[0].x, &bottom[1].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[2], corner2);
    bottom[2].y -= height;
    Collision_RayTriangle(corner2, corner3, &bottom[2].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[3], corner3);
    bottom[3].y -= height;
    Collision_RayTriangle(corner3, &bottom[2].x, &bottom[3].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner1, corner2, &bottom[1].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner2, &bottom[1].x, &bottom[2].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner3, corner0, &bottom[3].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner0, &bottom[3].x, &bottom[0].x, from, direction, distance);
}

/* Tests a ray against the walls of a two-box map object (kind 2), two triangles a wall. Model
   points 0 to 3 and 4 to 7 are the bottom corners of its two halves; the top ones lie above
   them at the height of point 0x1F (y grows downward). While the object is on frame 1 of animation 4 the
   halves are tested as one box (corners 0, 1, 6, 7). Stops at the first triangle hit. */
void Collision_HitRayDoubleDoorObj(void *target, f32 *from, f32 *direction, float distance) {
    MapObject *object = target;
    Vector4 bottom[4];
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *top;
    f32 height;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    if (MODEL_CHANNEL(object->model)->motion_index == object->definition->anims[4].anim && MODEL_CHANNEL(object->model)->frame == 1) {
        corner0 = (f32 *)object->model->get_point(object->model, 0);
        top = (f32 *)object->model->get_point(object->model, 0x1F);
        height = corner0[1] - top[1];
        corner1 = (f32 *)object->model->get_point(object->model, 1);
        corner2 = (f32 *)object->model->get_point(object->model, 6);
        corner3 = (f32 *)object->model->get_point(object->model, 7);
        Vector_Copy(&bottom[0], corner0);
        bottom[0].y -= height;
        Collision_RayTriangle(corner0, corner1, &bottom[0].x, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Vector_Copy(&bottom[1], corner1);
        bottom[1].y -= height;
        Collision_RayTriangle(corner1, &bottom[0].x, &bottom[1].x, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Vector_Copy(&bottom[2], corner2);
        bottom[2].y -= height;
        Collision_RayTriangle(corner2, corner3, &bottom[2].x, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Vector_Copy(&bottom[3], corner3);
        bottom[3].y -= height;
        Collision_RayTriangle(corner3, &bottom[2].x, &bottom[3].x, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_RayTriangle(corner1, corner2, &bottom[1].x, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_RayTriangle(corner2, &bottom[1].x, &bottom[2].x, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_RayTriangle(corner3, corner0, &bottom[3].x, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_RayTriangle(corner0, &bottom[3].x, &bottom[0].x, from, direction, distance);
        return;
    }
    corner0 = (f32 *)object->model->get_point(object->model, 0);
    top = (f32 *)object->model->get_point(object->model, 0x1F);
    height = corner0[1] - top[1];
    corner1 = (f32 *)object->model->get_point(object->model, 1);
    corner2 = (f32 *)object->model->get_point(object->model, 2);
    corner3 = (f32 *)object->model->get_point(object->model, 3);
    Vector_Copy(&bottom[0], corner0);
    bottom[0].y -= height;
    Collision_RayTriangle(corner0, corner1, &bottom[0].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[1], corner1);
    bottom[1].y -= height;
    Collision_RayTriangle(corner1, &bottom[0].x, &bottom[1].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[2], corner2);
    bottom[2].y -= height;
    Collision_RayTriangle(corner2, corner3, &bottom[2].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[3], corner3);
    bottom[3].y -= height;
    Collision_RayTriangle(corner3, &bottom[2].x, &bottom[3].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner1, corner2, &bottom[1].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner2, &bottom[1].x, &bottom[2].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner3, corner0, &bottom[3].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner0, &bottom[3].x, &bottom[0].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    corner0 = (f32 *)object->model->get_point(object->model, 4);
    corner1 = (f32 *)object->model->get_point(object->model, 5);
    corner2 = (f32 *)object->model->get_point(object->model, 6);
    corner3 = (f32 *)object->model->get_point(object->model, 7);
    Vector_Copy(&bottom[0], corner0);
    bottom[0].y -= height;
    Collision_RayTriangle(corner0, corner1, &bottom[0].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[1], corner1);
    bottom[1].y -= height;
    Collision_RayTriangle(corner1, &bottom[0].x, &bottom[1].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[2], corner2);
    bottom[2].y -= height;
    Collision_RayTriangle(corner2, corner3, &bottom[2].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Vector_Copy(&bottom[3], corner3);
    bottom[3].y -= height;
    Collision_RayTriangle(corner3, &bottom[2].x, &bottom[3].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner1, corner2, &bottom[1].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner2, &bottom[1].x, &bottom[2].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner3, corner0, &bottom[3].x, from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    Collision_RayTriangle(corner0, &bottom[3].x, &bottom[0].x, from, direction, distance);
}

// match: retail returns what the second test leaves in $v0; the definition returns nothing
typedef s32 (*RayTriangleValue)(f32 *, f32 *, f32 *, f32 *, f32 *, f32);

/* Tests a ray against the quad of the object's model points 0 to 3 as two triangles, if the object has flag
   0x10000; the second triangle only when the first is missed. */
s32 Collision_HitRayQuadObj(AnimObject *object, f32 *param1, f32 *param2, f32 param3) {
    f32 *point0;
    f32 *point1;
    f32 *point2;
    f32 *point3;
    s32 result;

    result = ANIM_OBJECT_FLAGS(object) & OBJECT_FLAG_QUAD_HIT;
    if (result) {
        point0 = object->model->get_point(object->model, 0);
        point1 = object->model->get_point(object->model, 1);
        point2 = object->model->get_point(object->model, 2);
        point3 = object->model->get_point(object->model, 3);
        ((RayTriangleValue)Collision_RayTriangle)(point0, point1, point2, param1, param2, param3);
        result = COLLISION_RESULT->hit;
        if (result == 0) {
            result = ((RayTriangleValue)Collision_RayTriangle)(point0, point2, point3, param1, param2, param3);
        }
    }
    return result;
}

/* Tests a ray against a map object of kind 18 (a pitfall). With MAP_OBJECT_ON clear the object's
   collision strips are tested, as Collision_HitRayMeshObj does. With it set, model points 0 to 0x1F are
   taken four at a time as quads, up to the first quad with a corner at the model's own position
   (an unused point). */
void Collision_HitRayPitfallObj(void *target, f32 *from, f32 *direction, float distance) {
    MapObject *object = target;
    ModelStripList *list;
    Strip *strip;
    StripVertex *vertex;
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    int strips_left;
    int triangles_left;
    int i;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        list = object->model->strips;
        if (list != NULL && list->total_vertex_count >= 4U) {
            Collision_TransformMeshPoints(object->model);
            strip = list->strips;
            strips_left = list->strip_count;
            do {
                vertex = strip->vertices;
                triangles_left = strip->vertex_count - 2;
                do {
                    Collision_RayTriangle(vertex[0].position, vertex[1].position, vertex[2].position, from, direction, distance);
                    if (COLLISION_RESULT->hit != 0) {
                        return;
                    }
                    vertex++;
                } while (--triangles_left != 0);
                strip = (Strip *)(vertex + 2);
            } while (--strips_left != 0);
        }
        return;
    }
    for (i = 0; i < 0x20; i += 4) {
        corner0 = (f32 *)object->model->get_point(object->model, i);
        corner1 = (f32 *)object->model->get_point(object->model, i + 1);
        corner2 = (f32 *)object->model->get_point(object->model, i + 2);
        corner3 = (f32 *)object->model->get_point(object->model, i + 3);
        if (corner0[0] == object->model->position.x && corner0[1] == object->model->position.y && corner0[2] == object->model->position.z) {
            return;
        }
        if (corner1[0] == object->model->position.x && corner1[1] == object->model->position.y && corner1[2] == object->model->position.z) {
            return;
        }
        if (corner2[0] == object->model->position.x && corner2[1] == object->model->position.y && corner2[2] == object->model->position.z) {
            return;
        }
        if (corner3[0] == object->model->position.x && corner3[1] == object->model->position.y && corner3[2] == object->model->position.z) {
            return;
        }
        Collision_RayTriangle(corner0, corner1, corner2, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_RayTriangle(corner0, corner2, corner3, from, direction, distance);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
    }
}

/* Tests a point (with a reach) against every triangle of the strips of a map object's collision mesh, if the
   object is in use and the mesh has 4 points or more. At the first hit it stops and notes the floor material
   in COLLISION_RESULT->floor_material. */
void Collision_HitSphereMeshObj(CollisionOwner *owner, f32 *point, float factor) {
    ModelStripList *list;
    Strip *strip;
    StripVertex *vertex;
    int i, j;

    if (owner->flags & OBJECT_FLAG_QUAD_HIT) {
        list = owner->model->strips;
        if (list != NULL && list->total_vertex_count >= 4U) {
            Collision_TransformMeshPoints(owner->model);
            strip = list->strips;
            i = list->strip_count;
            do {
                vertex = strip->vertices;
                j = strip->vertex_count - 2;
                do {
                    Collision_PointTriangle(vertex[0].position, vertex[1].position, vertex[2].position, point, factor);
                    if (COLLISION_RESULT->hit != 0) {
                        COLLISION_RESULT->floor_material = Collision_GetFloorMaterial(vertex->x, vertex->y, vertex->z);
                        return;
                    }
                    vertex++;
                } while (--j != 0);
                strip = (Strip *)(vertex + 2);
            } while (--i != 0);
        }
    }
}

/* Tests a point against the four walls of a box-like map object (kind 1): model points 0 to 3
   are its corners and point 0x1F gives its height. Stops at the first wall hit. */
int Collision_HitSphereBoxObj(MapObject *object, f32 *point, f32 reach) {
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *top;
    f32 height;
    int hit;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    if (object->model->get_point(object->model, 0x1F) == 0) {
        return;
    }
    corner0 = (f32 *)object->model->get_point(object->model, 0);
    if (corner0 == NULL) {
        return;
    }
    corner1 = (f32 *)object->model->get_point(object->model, 1);
    if (corner1 == NULL) {
        return;
    }
    corner2 = (f32 *)object->model->get_point(object->model, 2);
    if (corner2 == NULL) {
        return;
    }
    corner3 = (f32 *)object->model->get_point(object->model, 3);
    if (corner3 == NULL) {
        return;
    }
    top = (f32 *)object->model->get_point(object->model, 0x1F);
    if (top == NULL) {
        return;
    }
    height = corner0[1] - top[1];
    Collision_BallEdge(corner0, corner1, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner1, corner2, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner2, corner3, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    return ((WallTest)Collision_BallEdge)(corner3, corner0, height, point, reach);
}

/* Tests a point against the walls of a two-box map object (kind 2). Model points 0 to 3 and
   4 to 7 are the corners of its two halves and point 0x1F gives its height. While the object
   is at the start of animation 4 the halves are tested as one box (corners 0, 1, 6, 7).
   Stops at the first wall hit. */
int Collision_HitSphereDoubleDoorObj(MapObject *object, f32 *point, f32 reach) {
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *corner4;
    f32 *corner5;
    f32 *corner6;
    f32 *corner7;
    f32 *top;
    f32 height;
    int hit;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    if (MODEL_CHANNEL(object->model)->motion_index == object->definition->anims[4].anim && MODEL_CHANNEL(object->model)->frame < 2) {
        corner0 = (f32 *)object->model->get_point(object->model, 0);
        if (corner0 == NULL) {
            return;
        }
        corner1 = (f32 *)object->model->get_point(object->model, 1);
        if (corner1 == NULL) {
            return;
        }
        corner2 = (f32 *)object->model->get_point(object->model, 6);
        if (corner2 == NULL) {
            return;
        }
        corner3 = (f32 *)object->model->get_point(object->model, 7);
        if (corner3 == NULL) {
            return;
        }
        top = (f32 *)object->model->get_point(object->model, 0x1F);
        if (top == NULL) {
            return;
        }
        height = corner0[1] - top[1];
        Collision_BallEdge(corner0, corner1, height, point, reach);
        hit = COLLISION_RESULT->hit;
        if (hit != 0) {
            return hit;
        }
        Collision_BallEdge(corner1, corner2, height, point, reach);
        hit = COLLISION_RESULT->hit;
        if (hit != 0) {
            return hit;
        }
        Collision_BallEdge(corner2, corner3, height, point, reach);
        hit = COLLISION_RESULT->hit;
        if (hit != 0) {
            return hit;
        }
        return ((WallTest)Collision_BallEdge)(corner3, corner0, height, point, reach);
    }
    corner0 = (f32 *)object->model->get_point(object->model, 0);
    if (corner0 == NULL) {
        return;
    }
    corner1 = (f32 *)object->model->get_point(object->model, 1);
    if (corner1 == NULL) {
        return;
    }
    corner2 = (f32 *)object->model->get_point(object->model, 2);
    if (corner2 == NULL) {
        return;
    }
    corner3 = (f32 *)object->model->get_point(object->model, 3);
    if (corner3 == NULL) {
        return;
    }
    corner4 = (f32 *)object->model->get_point(object->model, 4);
    if (corner4 == NULL) {
        return;
    }
    corner5 = (f32 *)object->model->get_point(object->model, 5);
    if (corner5 == NULL) {
        return;
    }
    corner6 = (f32 *)object->model->get_point(object->model, 6);
    if (corner6 == NULL) {
        return;
    }
    corner7 = (f32 *)object->model->get_point(object->model, 7);
    if (corner7 == NULL) {
        return;
    }
    top = (f32 *)object->model->get_point(object->model, 0x1F);
    if (top == NULL) {
        return;
    }
    height = corner0[1] - top[1];
    Collision_BallEdge(corner0, corner1, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner1, corner2, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner2, corner3, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner3, corner0, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner4, corner5, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner5, corner6, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_BallEdge(corner6, corner7, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    return ((WallTest)Collision_BallEdge)(corner7, corner4, height, point, reach);
}

/* Tests a point (with a reach) against the quad of the object's model points 0 to 3, as two triangles, if the
   object has MAP_OBJECT_IN_USE. */
void Collision_HitSphereQuadObj(MapObject *object, f32 *point, f32 reach) {
    f32 *corner_0;
    f32 *corner_1;
    f32 *corner_2;
    f32 *corner_3;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    corner_0 = (f32 *)object->model->get_point(object->model, 0);
    corner_1 = (f32 *)object->model->get_point(object->model, 1);
    corner_2 = (f32 *)object->model->get_point(object->model, 2);
    corner_3 = (f32 *)object->model->get_point(object->model, 3);
    Collision_PointTriangle(corner_0, corner_1, corner_2, point, reach);
    if (COLLISION_RESULT->hit == 0) {
        Collision_PointTriangle(corner_0, corner_2, corner_3, point, reach);
    }
}

/* Tests a point against a pitfall (kind 18); the game's own debug print names it ("Hit HitSpherePitfallObj()"
   on a quad hit). Closed (MAP_OBJECT_ON clear) its collision strips are tested, as Collision_HitSphereMeshObj
   does; open, model points 0 to 0x1F are taken four at a time as quads, up to the first quad with a corner at
   the model's own position (an unused point). */
void Collision_HitSpherePitfallObj(MapObject *object, f32 *point, f32 reach) {
    ModelStripList *list;
    Strip *strip;
    StripVertex *vertex;
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    int strips_left;
    int triangles_left;
    int i;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        list = object->model->strips;
        if (list != NULL && list->total_vertex_count >= 4U) {
            Collision_TransformMeshPoints(object->model);
            strip = list->strips;
            strips_left = list->strip_count;
            do {
                vertex = strip->vertices;
                triangles_left = strip->vertex_count - 2;
                do {
                    Collision_PointTriangle(vertex[0].position, vertex[1].position, vertex[2].position, point, reach);
                    if (COLLISION_RESULT->hit != 0) {
                        COLLISION_RESULT->floor_material = Collision_GetFloorMaterial(vertex->x, vertex->y, vertex->z);
                        return;
                    }
                    vertex++;
                } while (--triangles_left != 0);
                strip = (Strip *)(vertex + 2);
            } while (--strips_left != 0);
        }
        return;
    }
    for (i = 0; i < 0x20; i += 4) {
        corner0 = (f32 *)object->model->get_point(object->model, i);
        corner1 = (f32 *)object->model->get_point(object->model, i + 1);
        corner2 = (f32 *)object->model->get_point(object->model, i + 2);
        corner3 = (f32 *)object->model->get_point(object->model, i + 3);
        if (corner0[0] == object->model->position.x && corner0[1] == object->model->position.y && corner0[2] == object->model->position.z) {
            return;
        }
        if (corner1[0] == object->model->position.x && corner1[1] == object->model->position.y && corner1[2] == object->model->position.z) {
            return;
        }
        if (corner2[0] == object->model->position.x && corner2[1] == object->model->position.y && corner2[2] == object->model->position.z) {
            return;
        }
        if (corner3[0] == object->model->position.x && corner3[1] == object->model->position.y && corner3[2] == object->model->position.z) {
            return;
        }
        Collision_PointTriangle(corner0, corner1, corner2, point, reach);
        if (COLLISION_RESULT->hit != 0) {
            puts(hit_sphere_pitfall_message);
            return;
        }
        Collision_PointTriangle(corner0, corner2, corner3, point, reach);
        if (COLLISION_RESULT->hit != 0) {
            puts(hit_sphere_pitfall_message);
            return;
        }
    }
}

/* Tests an upright capsule (`centre`, `radius`, `height`) against every triangle of the strips of the object's
   model, if the object has MAP_OBJECT_IN_USE and the mesh unk0C is 4 or more. At the first hit it stops and puts
   Collision_GetFloorMaterial of the hit point's three words in COLLISION_RESULT->floor_material. */
void Collision_HitCapsuleMeshObj(MapObject *object, f32 *centre, f32 height, f32 radius) {
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 strips_left;
    s32 triangles_left;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        mesh = (CollisionMesh *)object->model->strips;
        if (mesh != NULL && mesh->point_count >= 4) {
            Collision_TransformMeshPoints(object->model);
            strip = mesh->strips;
            strips_left = mesh->strip_count;
            do {
                point = strip->points;
                triangles_left = strip->point_count - 2;
                do {
                    Collision_CapsuleTriangle(point[0].position, point[1].position, point[2].position, centre, radius,
                                              height);
                    if (COLLISION_RESULT->hit != 0) {
                        COLLISION_RESULT->floor_material = Collision_GetFloorMaterial(point->material_first, point->material_second, point->material_third);
                        return;
                    }
                    point++;
                } while (--triangles_left != 0);
                strip = (CollisionStrip *)(point + 2);
            } while (--strips_left != 0);
        }
    }
}

/* Tests an upright capsule against a box-like map object (kind 1): its four walls (model points
   0 to 3 are the corners, point 0x1F gives the height), then its four corner edges. Stops at
   the first hit. `radius` and `push` are the two sizes Collision_HitCapsuleObj passes on. */
int Collision_HitCapsuleBoxObj(MapObject *object, f32 *point, f32 radius, f32 push) {
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *top;
    f32 height;
    int hit;

    if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
        return;
    }
    corner0 = (f32 *)object->model->get_point(object->model, 0);
    if (corner0 == NULL) {
        return;
    }
    corner1 = (f32 *)object->model->get_point(object->model, 1);
    if (corner1 == NULL) {
        return;
    }
    corner2 = (f32 *)object->model->get_point(object->model, 2);
    if (corner2 == NULL) {
        return;
    }
    corner3 = (f32 *)object->model->get_point(object->model, 3);
    if (corner3 == NULL) {
        return;
    }
    top = (f32 *)object->model->get_point(object->model, 0x1F);
    if (top == NULL) {
        return;
    }
    height = corner0[1] - top[1];
    Collision_CylinderEdge(corner0, corner1, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_CylinderEdge(corner1, corner2, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_CylinderEdge(corner2, corner3, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_CylinderEdge(corner3, corner0, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_CylinderCylinder(point, push, radius, corner0, 0.0f, height);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_CylinderCylinder(point, push, radius, corner1, 0.0f, height);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    Collision_CylinderCylinder(point, push, radius, corner2, 0.0f, height);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    return ((CornerTest)Collision_CylinderCylinder)(point, push, radius, corner3, 0.0f, height);
}

/* Tests an upright capsule (`radius` and `push` are the two sizes Collision_HitCapsuleObj passes on) against
   the walls and corner posts of a map object's outline and stops at the first hit: points 0, 1, 6, 7 while
   its fifth animation is on frame 0 or 1, otherwise the two quads 0-3 and 4-7. The result is left in the
   scratchpad. */
void Collision_HitCapsuleDoubleDoorObj(MapObject *object, f32 *centre, f32 radius, f32 push) {
    f32 *point0, *point1, *point2, *point3, *point4, *point5, *point6, *point7;
    f32 *top;
    f32 height;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        if (MODEL_CHANNEL(object->model)->motion_index == object->definition->anims[4].anim && MODEL_CHANNEL(object->model)->frame < 2) {
            point0 = (f32 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (f32 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (f32 *)object->model->get_point(object->model, 6);
            if (point2 == NULL) {
                return;
            }
            point3 = (f32 *)object->model->get_point(object->model, 7);
            if (point3 == NULL) {
                return;
            }
            top = (f32 *)object->model->get_point(object->model, 0x1F);
            if (top == NULL) {
                return;
            }
            height = point0[1] - top[1];
            TEST_WALL(point0, point1);
            TEST_WALL(point1, point2);
            TEST_WALL(point2, point3);
            TEST_WALL(point3, point0);
            TEST_POST(point0);
            TEST_POST(point1);
            TEST_POST(point2);
            Collision_CylinderCylinder(centre, push, radius, point3, 0.0f, height);
        } else {
            point0 = (f32 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (f32 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (f32 *)object->model->get_point(object->model, 2);
            if (point2 == NULL) {
                return;
            }
            point3 = (f32 *)object->model->get_point(object->model, 3);
            if (point3 == NULL) {
                return;
            }
            point4 = (f32 *)object->model->get_point(object->model, 4);
            if (point4 == NULL) {
                return;
            }
            point5 = (f32 *)object->model->get_point(object->model, 5);
            if (point5 == NULL) {
                return;
            }
            point6 = (f32 *)object->model->get_point(object->model, 6);
            if (point6 == NULL) {
                return;
            }
            point7 = (f32 *)object->model->get_point(object->model, 7);
            if (point7 == NULL) {
                return;
            }
            top = (f32 *)object->model->get_point(object->model, 0x1F);
            if (top == NULL) {
                return;
            }
            height = point0[1] - top[1];
            TEST_WALL(point0, point1);
            TEST_WALL(point1, point2);
            TEST_WALL(point2, point3);
            TEST_WALL(point3, point0);
            TEST_WALL(point4, point5);
            TEST_WALL(point5, point6);
            TEST_WALL(point6, point7);
            TEST_WALL(point7, point4);
            TEST_POST(point0);
            TEST_POST(point1);
            TEST_POST(point2);
            TEST_POST(point3);
            TEST_POST(point4);
            TEST_POST(point5);
            TEST_POST(point6);
            Collision_CylinderCylinder(centre, push, radius, point7, 0.0f, height);
        }
    }
}

// match: retail returns what the second test leaves in $v0; the definition returns nothing
typedef int (*CapsuleTriangleValue)(f32 *, f32 *, f32 *, f32 *, float, float);

/* Tests an upright capsule against the quad of the owner's model points 0 to 3 as two triangles, if the owner
   has flag 0x10000; the second triangle only when the first is missed. */
int Collision_HitCapsuleQuadObj(CollisionOwner *owner, f32 *point, float first_value, float second_value) {
    f32 *point_a, *point_b, *point_c, *point_d;
    int result;

    if (owner->flags & OBJECT_FLAG_QUAD_HIT) {
        point_a = owner->model->get_point(owner->model, 0);
        point_b = owner->model->get_point(owner->model, 1);
        point_c = owner->model->get_point(owner->model, 2);
        point_d = owner->model->get_point(owner->model, 3);
        ((CapsuleTriangleValue)Collision_CapsuleTriangle)(point_a, point_b, point_c, point, second_value, first_value);
        result = COLLISION_RESULT->hit;
        if (result == 0) {
            return ((CapsuleTriangleValue)Collision_CapsuleTriangle)(point_a, point_c, point_d, point, second_value, first_value);
        }
        return result;
    }
}

/* Does nothing: the pitfall case of Collision_HitCapsuleObj, which passes it the same arguments as the
   others. */
void Collision_HitCapsulePitfallObj(void) {
}

/* Tests the capsule at `origin` against every triangle of a map object's collision mesh,
   leaving out the triangles whose corners all lie outside the wedge from the origin to
   (first_x, first_z) and (second_x, second_z). Stops at the first hit and notes its surface. */
void Collision_HitFanMeshObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;
    s32 second_sides;
    s32 first_sides;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        mesh = (CollisionMesh *)object->model->strips;
        if (mesh == NULL) {
            return;
        }
        if (mesh->point_count < 4) {
            return;
        }
        Collision_TransformMeshPoints(object->model);
        strip = mesh->strips;
        i = mesh->strip_count;
        do {
            j = strip->point_count - 2;
            point = strip->points;
            if (j > 0) {
                second_sides = 0;
                first_sides = 0;
                TEST_SIDES((f32 *)strip->points[0].position);
                first_sides <<= 1;
                second_sides <<= 1;
                TEST_SIDES((f32 *)strip->points[1].position);
                first_sides <<= 1;
                second_sides <<= 1;
                point += 2;
                do {
                    TEST_SIDES((f32 *)point->position);
                    if (second_sides != 7 && first_sides != 7) {
                        // match: the unsigned indices give retail's two-step address
                        Collision_CapsuleTriangle(point[-2U].position, point[-1U].position, point->position,
                                      &origin->x, push, radius);
                        if (COLLISION_RESULT->hit != 0) {
                            COLLISION_RESULT->floor_material = Collision_GetFloorMaterial(point->material_first, point->material_second, point->material_third);
                            return;
                        }
                    }
                    second_sides = (second_sides << 1) & 7;
                    first_sides = (first_sides << 1) & 7;
                    point++;
                } while (--j != 0);
            } else {
                point += strip->point_count;
            }
            strip = (CollisionStrip *)point;
        } while (--i != 0);
    }
}

/* Tests the capsule at `origin` against the four walls of a map object's outline (points 0-3),
   leaving out a wall whose two ends both lie outside the wedge from the origin to
   (first_x, first_z) and (second_x, second_z). Stops at the first hit. */
void Collision_HitFanBoxObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    Vector4 *point0, *point1, *point2, *point3;
    Vector4 *top;
    f32 height;
    s32 first0, first1, first2, first3;
    s32 second0, second1, second2, second3;
    s32 hit;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        point0 = (Vector4 *)object->model->get_point(object->model, 0);
        point1 = (Vector4 *)object->model->get_point(object->model, 1);
        point2 = (Vector4 *)object->model->get_point(object->model, 2);
        point3 = (Vector4 *)object->model->get_point(object->model, 3);
        top = (Vector4 *)object->model->get_point(object->model, 0x1F);
        height = point0->y - top->y;
        first0 = OUTSIDE_FIRST(point0);
        second0 = OUTSIDE_SECOND(point0);
        first1 = OUTSIDE_FIRST(point1);
        second1 = OUTSIDE_SECOND(point1);
        first2 = OUTSIDE_FIRST(point2);
        second2 = OUTSIDE_SECOND(point2);
        first3 = OUTSIDE_FIRST(point3);
        second3 = OUTSIDE_SECOND(point3);
        if (!(first0 && first1) && !(second0 && second1)) {
            Collision_CylinderEdgeExtended(point0, point1, height, origin, radius, push);
        }
        // match: read even when the wall was left out, whatever the last test before this left there
        hit = COLLISION_RESULT->hit;
        if (hit != 0) {
            return;
        }
        if (!(first1 && first2) && !(second1 && second2)) {
            Collision_CylinderEdgeExtended(point1, point2, height, origin, radius, push);
            hit = COLLISION_RESULT->hit;
            if (hit != 0) {
                return;
            }
        } else if (hit != 0) {    // match: always false here; the repeated test (one copy in the binary) orders the saved registers
            return;
        }
        if (!(first2 && first3) && !(second2 && second3)) {
            Collision_CylinderEdgeExtended(point2, point3, height, origin, radius, push);
            hit = COLLISION_RESULT->hit;
            if (hit != 0) {
                return;
            }
        } else if (hit != 0) {    // match: as above
            return;
        }
        if (!(first3 && first0) && !(second3 && second0)) {
            Collision_CylinderEdgeExtended(point3, point0, height, origin, radius, push);
        }
    }
}

/* Collides a circle with the outline of a map object, leaving out every edge that lies wholly left of the first line from
   the origin or wholly right of the second. The outline is points 0, 1, 6, 7 while the object's fifth animation is on its
   first frames, otherwise the two quads 0-3 and 4-7. Stops at the first edge that hits. */
void Collision_HitFanDoubleDoorObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    Vector4 *point0, *point1, *point2, *point3, *point4, *point5, *point6, *point7;
    Vector4 *base;
    s32 left0, left1, left2, left3, left4, left5, left6, left7;
    s32 right0, right1, right2, right3, right4, right5, right6, right7;
    f32 height;
    CollisionResult *result = COLLISION_RESULT; // match: one pointer, so the hit flag is reloaded only after a call

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        if (MODEL_CHANNEL(object->model)->motion_index == object->definition->anims[4].anim && MODEL_CHANNEL(object->model)->frame < 2) {
            point0 = (Vector4 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (Vector4 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (Vector4 *)object->model->get_point(object->model, 6);
            if (point2 == NULL) {
                return;
            }
            point3 = (Vector4 *)object->model->get_point(object->model, 7);
            if (point3 == NULL) {
                return;
            }
            base = (Vector4 *)object->model->get_point(object->model, 0x1F);
            if (base == NULL) {
                return;
            }
            height = point0->y - base->y;
            left0 = LEFT_OF(point0, first_x, first_z);
            right0 = RIGHT_OF(point0, second_x, second_z);
            left1 = LEFT_OF(point1, first_x, first_z);
            right1 = RIGHT_OF(point1, second_x, second_z);
            left2 = LEFT_OF(point2, first_x, first_z);
            right2 = RIGHT_OF(point2, second_x, second_z);
            left3 = LEFT_OF(point3, first_x, first_z);
            right3 = RIGHT_OF(point3, second_x, second_z);
            if (!(left0 && left1) && !(right0 && right1)) {
                Collision_CylinderEdgeExtended(point0, point1, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left1 && left2) && !(right1 && right2)) {
                Collision_CylinderEdgeExtended(point1, point2, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left2 && left3) && !(right2 && right3)) {
                Collision_CylinderEdgeExtended(point2, point3, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left3 && left0) && !(right3 && right0)) {
                Collision_CylinderEdgeExtended(point3, point0, height, origin, radius, push);
            }
        } else {
            point0 = (Vector4 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (Vector4 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (Vector4 *)object->model->get_point(object->model, 2);
            if (point2 == NULL) {
                return;
            }
            point3 = (Vector4 *)object->model->get_point(object->model, 3);
            if (point3 == NULL) {
                return;
            }
            point4 = (Vector4 *)object->model->get_point(object->model, 4);
            if (point4 == NULL) {
                return;
            }
            point5 = (Vector4 *)object->model->get_point(object->model, 5);
            if (point5 == NULL) {
                return;
            }
            point6 = (Vector4 *)object->model->get_point(object->model, 6);
            if (point6 == NULL) {
                return;
            }
            point7 = (Vector4 *)object->model->get_point(object->model, 7);
            if (point7 == NULL) {
                return;
            }
            base = (Vector4 *)object->model->get_point(object->model, 0x1F);
            if (base == NULL) {
                return;
            }
            height = point0->y - base->y;
            left0 = LEFT_OF(point0, first_x, first_z);
            right0 = RIGHT_OF(point0, second_x, second_z);
            left1 = LEFT_OF(point1, first_x, first_z);
            right1 = RIGHT_OF(point1, second_x, second_z);
            left2 = LEFT_OF(point2, first_x, first_z);
            right2 = RIGHT_OF(point2, second_x, second_z);
            left3 = LEFT_OF(point3, first_x, first_z);
            right3 = RIGHT_OF(point3, second_x, second_z);
            left4 = LEFT_OF(point4, first_x, first_z);
            right4 = RIGHT_OF(point4, second_x, second_z);
            left5 = LEFT_OF(point5, first_x, first_z);
            right5 = RIGHT_OF(point5, second_x, second_z);
            left6 = LEFT_OF(point6, first_x, first_z);
            right6 = RIGHT_OF(point6, second_x, second_z);
            left7 = LEFT_OF(point7, first_x, first_z);
            right7 = RIGHT_OF(point7, second_x, second_z);
            if (!(left0 && left1) && !(right0 && right1)) {
                Collision_CylinderEdgeExtended(point0, point1, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left1 && left2) && !(right1 && right2)) {
                Collision_CylinderEdgeExtended(point1, point2, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left2 && left3) && !(right2 && right3)) {
                Collision_CylinderEdgeExtended(point2, point3, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left3 && left0) && !(right3 && right0)) {
                Collision_CylinderEdgeExtended(point3, point0, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left4 && left5) && !(right4 && right5)) {
                Collision_CylinderEdgeExtended(point4, point5, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left5 && left6) && !(right5 && right6)) {
                Collision_CylinderEdgeExtended(point5, point6, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left6 && left7) && !(right6 && right7)) {
                Collision_CylinderEdgeExtended(point6, point7, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left7 && left4) && !(right7 && right4)) {
                Collision_CylinderEdgeExtended(point7, point4, height, origin, radius, push);
            }
        }
    }
}

/* Tests the capsule at `origin` against the two triangles of a map object's face (points 1, 2,
   0, 3 as a strip), leaving out a triangle whose corners all lie outside the wedge from the
   origin to (first_x, first_z) and (second_x, second_z). Stops at the first hit. */
void Collision_HitFanQuadObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    f32 *points[4];
    s32 second_sides;
    s32 first_sides;
    s32 i;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        second_sides = 0;
        first_sides = 0;
        points[0] = (f32 *)object->model->get_point(object->model, 1);
        points[1] = (f32 *)object->model->get_point(object->model, 2);
        points[2] = (f32 *)object->model->get_point(object->model, 0);
        points[3] = (f32 *)object->model->get_point(object->model, 3);
        TEST_SIDES(points[0]);
        first_sides <<= 1;
        second_sides <<= 1;
        TEST_SIDES(points[1]);
        first_sides <<= 1;
        second_sides <<= 1;
        for (i = 2; i < 4; i++) {
            // match: the notes are not moved up a bit here (Collision_HitFanMeshObj does), so on the second triangle bit 0 is point 0 or point 3
            TEST_SIDES(points[i]);
            if (second_sides != 7 && first_sides != 7) {
                Collision_CapsuleTriangle(points[i - 2], points[i - 1], points[i], &origin->x, push, radius);
                if (COLLISION_RESULT->hit != 0) {
                    return;
                }
            }
        }
    }
}

/* Does nothing: the pitfall case of Collision_HitFanObj, which passes it the same arguments as the others. */
void Collision_HitFanPitfallObj(void) {
}

/* Pushes `position` (an upright capsule of this height and radius) sideways out of the wall
   strips of a map object's mesh. When the object is of kind 10 and the position is the
   player's, the object the player touched is noted in the player state. */
void Collision_PushOutOfMeshObj(MapObject *object, f32 *position, f32 height, f32 radius) {
    f32 centre[4];
    f32 offset[4];
    s32 touched;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;

    touched = FALSE;
    if (object->state_flags & MAP_OBJECT_IN_USE) {
        mesh = (CollisionMesh *)object->model->strips;
        if (mesh == NULL) {
            return;
        }
        if (mesh->point_count < 4) {
            return;
        }
        Collision_TransformMeshPoints(object->model);
        Vector_Copy(centre, position);
        centre[1] -= 0.001f;
        if (MAP_OBJECT_DEF_KIND(object->definition) == 10 && (PlayerState *)position == &player_state
            && ((PlayerState *)position)->touched_map_object == 0) {
            strip = mesh->second_wall_strips;
            i = mesh->second_wall_strip_count;
            while (--i != -1 && !touched) {
                point = strip->points;
                j = strip->point_count - 2;
                do {
                    Collision_CapsuleTriangle(point[0].position, point[1].position, point[2].position,
                                  centre, radius, height);
                    if (COLLISION_RESULT->hit != 0) {
                        touched = TRUE;
                    }
                    point++;
                } while (--j != 0 && !touched);
                strip = (CollisionStrip *)(point + 2);
            }
        }
        strip = mesh->wall_strips;
        i = mesh->wall_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                Collision_CapsuleTriangle(point[0].position, point[1].position, point[2].position,
                              centre, radius, height);
                if (COLLISION_RESULT->hit != 0) {
                    offset[0] = centre[0] - COLLISION_RESULT->point[0];
                    offset[1] = 0.0f;
                    offset[2] = centre[2] - COLLISION_RESULT->point[2];
                    sceVu0Normalize(offset, offset);
                    Vector_Scale(offset, offset, radius);
                    centre[0] = COLLISION_RESULT->point[0] + offset[0];
                    centre[2] = COLLISION_RESULT->point[2] + offset[2];
                    if (MAP_OBJECT_DEF_KIND(object->definition) == 10 && (PlayerState *)position == &player_state
                        && ((PlayerState *)position)->touched_map_object == 0) {
                        touched = TRUE;
                    }
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
        position[0] = centre[0];
        position[2] = centre[2];
        if (touched && player_state.touched_map_object == 0) {
            player_state.touched_map_object = object;
        }
    }
}

/* Pushes a circle out of the four walls of a map object's outline (points 0-3), or else out of
   the first corner post it touches. */
void Collision_PushOutOfBoxObj(MapObject *object, f32 *centre, f32 radius, f32 push) {
    f32 *point0, *point1, *point2, *point3;
    f32 *top;
    f32 height;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        point0 = (f32 *)object->model->get_point(object->model, 0);
        if (point0 == NULL) {
            return;
        }
        point1 = (f32 *)object->model->get_point(object->model, 1);
        if (point1 == NULL) {
            return;
        }
        point2 = (f32 *)object->model->get_point(object->model, 2);
        if (point2 == NULL) {
            return;
        }
        point3 = (f32 *)object->model->get_point(object->model, 3);
        if (point3 == NULL) {
            return;
        }
        top = (f32 *)object->model->get_point(object->model, 0x1F);
        if (top == NULL) {
            return;
        }
        height = point0[1] - top[1];
        Collision_CylinderEdge(point0, point1, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        Collision_CylinderEdge(point1, point2, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        Collision_CylinderEdge(point2, point3, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        Collision_CylinderEdge(point3, point0, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, point0, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, point1, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, point2, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, point3, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
        }
    }
}

/* whether the circle does not fit between the two quads; the player is told by the object instead */
static inline s32 gap_is_closed(MapObject *object, f32 *centre, f32 push, f32 first_gap, f32 second_gap) {
    s32 closed = FALSE;

    if (centre == (f32 *)&player_state) {
        closed = -(object->door_open_flags & 1);
    } else if (first_gap <= push * 2.25f || second_gap <= push * 2.25f) {
        closed = TRUE;
    }
    return closed;
}

/* Pushes a circle out of the walls and corner posts of a map object's outline: points 0, 1, 6, 7 while its fifth animation
   is on frame 1, otherwise the two quads 0-3 and 4-7. When the gap between the quads is too narrow for the circle (or, for
   the player, when the object says so), the gap is walled off instead of the quads' facing sides. */
void Collision_PushOutOfDoubleDoorObj(MapObject *object, f32 *centre, f32 radius, f32 push) {
    f32 *point0, *point1, *point2, *point3, *point4, *point5, *point6, *point7;
    f32 *top;
    f32 height;
    f32 delta_x, delta_z;
    f32 first_gap, second_gap;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        object->model->update_bone_points(object->model);
        if (MODEL_CHANNEL(object->model)->motion_index == object->definition->anims[4].anim && MODEL_CHANNEL(object->model)->frame == 1) {
            point0 = (f32 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (f32 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (f32 *)object->model->get_point(object->model, 6);
            if (point2 == NULL) {
                return;
            }
            point3 = (f32 *)object->model->get_point(object->model, 7);
            if (point3 == NULL) {
                return;
            }
            top = (f32 *)object->model->get_point(object->model, 0x1F);
            if (top == NULL) {
                return;
            }
            height = point0[1] - top[1];
            Collision_CylinderEdge(point0, point1, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            Collision_CylinderEdge(point1, point2, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            Collision_CylinderEdge(point2, point3, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            Collision_CylinderEdge(point3, point0, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            Collision_CylinderCylinder(centre, push, radius, point0, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point1, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point2, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point3, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
        } else {
            point0 = (f32 *)object->model->get_point(object->model, 0);
            point1 = (f32 *)object->model->get_point(object->model, 1);
            point2 = (f32 *)object->model->get_point(object->model, 2);
            point3 = (f32 *)object->model->get_point(object->model, 3);
            point4 = (f32 *)object->model->get_point(object->model, 4);
            point5 = (f32 *)object->model->get_point(object->model, 5);
            point6 = (f32 *)object->model->get_point(object->model, 6);
            point7 = (f32 *)object->model->get_point(object->model, 7);
            top = (f32 *)object->model->get_point(object->model, 0x1F);
            height = point0[1] - top[1];
            delta_x = point2[0] - point5[0];
            delta_z = point2[2] - point5[2];
            first_gap = sqrtf(delta_x * delta_x + delta_z * delta_z);
            delta_x = point3[0] - point4[0];
            delta_z = point3[2] - point4[2];
            second_gap = sqrtf(delta_x * delta_x + delta_z * delta_z);
            if (!(MODEL_CHANNEL(object->model)->motion_index == object->definition->anims[5].anim &&
                  MODEL_CHANNEL(object->model)->frame == 1) &&
                gap_is_closed(object, centre, push, first_gap, second_gap)) {
                Collision_CylinderEdge(point2, point5, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point4, point3, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point0, point1, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point1, point2, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point3, point0, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
            } else {
                Collision_CylinderEdge(point0, point1, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point1, point2, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point2, point3, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point3, point0, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                Collision_CylinderEdge(point4, point5, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
            }
            Collision_CylinderEdge(point5, point6, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            Collision_CylinderEdge(point6, point7, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            Collision_CylinderEdge(point7, point4, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            Collision_CylinderCylinder(centre, push, radius, point0, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point1, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point2, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point3, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point4, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point5, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point6, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            Collision_CylinderCylinder(centre, push, radius, point7, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
        }
    }
}

/* Pushes `position` out of the upright side of the owner's model that runs from its point 0
   to its point 3; does nothing when that side lies nearly flat. */
void Collision_PushOutOfQuadObj(CollisionOwner *owner, f32 *position, f32 radius, f32 push) {
    f32 normal[4];
    f32 *point0;
    f32 *point1;
    f32 *point3;
    f32 height;

    if (owner->flags & OBJECT_FLAG_QUAD_HIT) {
        point0 = (f32 *)owner->model->get_point(owner->model, 0);
        point1 = (f32 *)owner->model->get_point(owner->model, 1);
        point3 = (f32 *)owner->model->get_point(owner->model, 3);
        Vector_TriangleNormal(normal, point0, point1, point3);
        Vector_Normalize(normal, normal);
        if (normal[1] < -QUAD_FLOOR_NORMAL_Y) {
            return;
        }
        if (normal[1] > QUAD_FLOOR_NORMAL_Y) {
            return;
        }
        height = point0[1] - point1[1];
        Collision_CylinderEdge(point0, point3, height, position, radius, push);
        if (COLLISION_RESULT->hit == 0) {
            Collision_CylinderEdge(point3, point0, height, position, radius, push);
            if (COLLISION_RESULT->hit == 0) {
                return;
            }
        }
        position[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
        position[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
    }
}

/* Does nothing: the pitfall case of Collision_PushOutOfObjects, which passes it the same arguments as the
   others. */
void Collision_PushOutOfPitfallObj(void) {
}

/* Drops a ball of this radius at `position` onto the floor strips of the owner's collision mesh and puts it 1
   above the highest hit (y grows downward). Returns TRUE if it hit one. */
s32 Collision_StandOnMeshObj(CollisionOwner *owner, f32 *position, f32 radius) {
    f32 centre[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;
    f32 lowest;

    if (owner->flags & OBJECT_FLAG_QUAD_HIT) {
        mesh = (CollisionMesh *)owner->model->strips;
        if (mesh == NULL) {
            return FALSE;
        }
        if (mesh->point_count < 4) {
            return FALSE;
        }
        Collision_TransformMeshPoints(owner->model);
        Vector_Copy(centre, position);
        lowest = largest_float_mesh_lowest_seed[0];
        centre[1] = centre[1] - 1.0f - radius;
        strip = mesh->floor_strips;
        i = mesh->floor_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                Collision_DropOnTriangleWithin(point[0].position, point[1].position, point[2].position, centre, radius);
                if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                    lowest = COLLISION_RESULT->point[1];
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
        if (lowest < largest_float_mesh_lowest_seed[0]) {
            position[1] = lowest - 1.0f;
            return TRUE;
        }
    }
    return FALSE;
}

/* Drops a ball of this radius at `position` onto the four-cornered face of the owner's model, unless the face
   stands upright, and puts it 1 above the highest hit (y grows downward). Returns TRUE if it hit. */
s32 Collision_StandOnQuadObj(CollisionOwner *owner, f32 *position, f32 radius) {
    f32 centre[4];
    f32 normal[4];
    f32 *point0;
    f32 *point1;
    f32 *point2;
    f32 *point3;
    f32 lowest;

    if (owner->flags & OBJECT_FLAG_QUAD_HIT) {
        point0 = (f32 *)owner->model->get_point(owner->model, 0);
        point1 = (f32 *)owner->model->get_point(owner->model, 1);
        point2 = (f32 *)owner->model->get_point(owner->model, 2);
        point3 = (f32 *)owner->model->get_point(owner->model, 3);
        Vector_TriangleNormal(normal, point0, point1, point3);
        Vector_Normalize(normal, normal);
        if (normal[1] >= -QUAD_FLOOR_NORMAL_Y && normal[1] <= QUAD_FLOOR_NORMAL_Y) {
            return FALSE;
        }
        Vector_Copy(centre, position);
        lowest = largest_float_quad_lowest_seed[0];
        centre[1] = centre[1] - 1.0f - radius;
        Collision_DropOnTriangleWithin(point0, point1, point2, centre, radius);
        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
            lowest = COLLISION_RESULT->point[1];
        }
        Collision_DropOnTriangleWithin(point0, point2, point3, centre, radius);
        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
            lowest = COLLISION_RESULT->point[1];
        }
        if (lowest < largest_float_quad_lowest_seed[0]) {
            position[1] = lowest - 1.0f;
            return TRUE;
        }
    }
    return FALSE;
}

/* The test of Collision_StandOnMeshObj over again (retail has it inline here, with its own copy of the limit). */
static inline s32 drop_on_floor_strips(MapObject *object, f32 *position, f32 radius) {
    f32 centre[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 strips_left;
    s32 points_left;
    f32 lowest;

    mesh = (CollisionMesh *)object->model->strips;
    if (mesh == NULL) {
        return FALSE;
    }
    if (mesh->point_count < 4) {
        return FALSE;
    }
    Collision_TransformMeshPoints(object->model);
    Vector_Copy(centre, position);
    lowest = largest_float_strip_lowest_seed[0];
    centre[1] = centre[1] - 1.0f - radius;
    strip = mesh->floor_strips;
    strips_left = mesh->floor_strip_count;
    while (--strips_left != -1) {
        point = strip->points;
        points_left = strip->point_count - 2;
        do {
            Collision_DropOnTriangleWithin(point[0].position, point[1].position, point[2].position, centre, radius);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                lowest = COLLISION_RESULT->point[1];
            }
            point++;
        } while (--points_left != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (lowest < largest_float_strip_lowest_seed[0]) {
        position[1] = lowest - 1.0f;
        return TRUE;
    }
    return FALSE;
}

/* Drops a ball of this radius at `position` onto a pitfall and puts it 1 above the highest hit (y grows
   downward): onto the floor strips of its mesh while it rests closed (animation 4), else onto the eight
   four-cornered faces its model's points make. Returns TRUE if it hit. */
s32 Collision_StandOnPitfallObj(MapObject *object, f32 *position, f32 radius) {
    f32 centre[4];
    f32 unused[4];
    s32 i;
    f32 *point0;
    f32 *point1;
    f32 *point2;
    f32 *point3;
    f32 lowest;

    if (object->state_flags & MAP_OBJECT_IN_USE) {
        if (MODEL_CHANNEL(object->model)->motion_index == object->definition->anims[4].anim
            && MODEL_CHANNEL(object->model)->frame < 2) {
            return drop_on_floor_strips(object, position, radius);
        }
        Vector_Copy(centre, position);
        lowest = largest_float_strip_lowest_seed[0];
        centre[1] = centre[1] - 1.0f - radius;
        for (i = 0; i < 0x20; i += 4) {
            point0 = (f32 *)object->model->get_point(object->model, i);
            point1 = (f32 *)object->model->get_point(object->model, i + 1);
            point2 = (f32 *)object->model->get_point(object->model, i + 2);
            point3 = (f32 *)object->model->get_point(object->model, i + 3);
            if (SAME_POINT(point0, object->model->position) || SAME_POINT(point1, object->model->position)
                || SAME_POINT(point2, object->model->position) || SAME_POINT(point3, object->model->position)) {
                return FALSE;
            }
            Collision_DropOnTriangleWithin(point0, point1, point2, centre, radius);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                lowest = COLLISION_RESULT->point[1];
            }
            Collision_DropOnTriangleWithin(point0, point2, point3, centre, radius);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < lowest) {
                lowest = COLLISION_RESULT->point[1];
            }
            if (lowest < largest_float_strip_lowest_seed[0]) {
                position[1] = lowest - 1.0f;
                return TRUE;
            }
        }
    }
    return FALSE;
}
