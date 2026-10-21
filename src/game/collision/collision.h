/* Collision tests: the result block in the scratchpad and the meshes tested against. */
#ifndef KF_COLLISION_H
#define KF_COLLISION_H

#include "common.h"
#include "model/model.h"
#include "entity/entity.h"

/* tolerances of the collision tests */
#define COLLISION_PARALLEL_EPSILON 0.00001f
#define COLLISION_END_ON_EPSILON 0.000001f
#define COLLISION_HORIZONTAL_EPSILON 0.0001f
/* a ratio above this is past the far end of a segment */
#define COLLISION_RATIO_PAST_END 1.00001f

/* what every collision test leaves at the start of the scratchpad */
typedef struct {
    f32 point[4];                   /* 0x00 where it hit */
    s32 hit;                        /* 0x10 0: no hit */
    f32 distance;                   /* 0x14 */
    s32 floor_material;             /* 0x18 the floor material of what was hit (Collision_GetFloorMaterial) */
    char pad1C[0x04];
    f32 normal[4];                  /* 0x20 unit vector away from what was hit */
} CollisionResult;

#define COLLISION_RESULT ((CollisionResult *)0x70000000)

/* the scratchpad from 0x100 on: the points of a model's collision mesh (CollisionMesh.points)
   placed in the world by Collision_TransformMeshPoints; the strips point at these */
#define COLLISION_WORLD_POINTS ((Vector4 *)0x70000100)

/* the scratchpad as Player_PushOutOfWalls uses it. Reached as fields of one block at 0x70000000, retail forms each
   address anew (`lui $1`); a pointer constant such as (Vector4 *)0x70000080 is kept in a register instead. */
typedef struct {
    CollisionResult result;         /* 0x00 */
    char pad30[0x40];
    Vector4 push;                   /* 0x70 the way out of what was hit */
    Vector4 point;                  /* 0x80 the point that is tested */
    Vector4 wall_point;             /* 0x90 the point Player_CollideWithMap keeps away from the walls */
} CollisionScratch;

#define COLLISION_SCRATCH ((CollisionScratch *)0x70000000)

/* a CollisionResult kept aside and put back (Collision_FindFloorBelow): retail moves it by doublewords */
typedef union {
    CollisionResult result;
    s64 doublewords[6];
} CollisionResultCopy;

#define COLLISION_RESULT_COPY ((CollisionResultCopy *)0x70000000)

/* a CollisionResult aligned to 8 bytes: copied whole it moves by doublewords, and its members
   keep their own types (Collision_CastSegmentAtMap, where the union above does not match) */
typedef struct {
    f32 point[4];                   /* 0x00 */
    s32 hit;                        /* 0x10 */
    f32 distance;                   /* 0x14 */
    s32 floor_material;             /* 0x18 the floor material of what was hit (Collision_GetFloorMaterial) */
    char pad1C[0x04];
    f32 normal[4];                  /* 0x20 */
} __attribute__((aligned(8))) CollisionResultAligned;

#define COLLISION_RESULT_ALIGNED ((CollisionResultAligned *)0x70000000)

/* one point of a triangle strip */
typedef struct {
    void *position;                 /* 0x00 */
    s32 material_first;             /* 0x04 the first word of the floor material (Collision_GetFloorMaterial) */
    s32 material_second;            /* 0x08 the second word of the floor material */
    s32 material_third;             /* 0x0C the third word of the floor material */
} CollisionStripPoint;

/* a triangle strip; the next one follows its last point */
typedef struct {
    u16 point_count;                /* 0x00 */
    char pad02[0x1E];
    CollisionStripPoint points[1];  /* 0x20 point_count of them */
} CollisionStrip;

/* what ModelInstance.strips points at */
typedef struct {
    s32 strip_count;                /* 0x00 */
    CollisionStrip *strips;         /* 0x04 */
    char *points;                   /* 0x08 the end of the strips: point_count 16-byte entries */
    u32 point_count;                /* 0x0C how many points there are, 4 or more */
    f32 reach_radius;               /* 0x10 how far it reaches from its origin, unscaled (MapObject_InitMap) */
    char *mesh_size;                /* 0x14 the next mesh of a map file starts this many bytes on (a size, typed as Map_BuildModels needs) */
    CollisionStrip *wall_strips;    /* 0x18 wall_strip_count strips */
    CollisionStrip *floor_strips;   /* 0x1C */
    CollisionStrip *second_wall_strips; /* 0x20 second_wall_strip_count strips */
    CollisionStrip *barrier_strips; /* 0x24 barrier_strip_count strips */
    CollisionStrip *hazard_strips;  /* 0x28 hazard_strip_count strips */
    CollisionStrip *water_strips;   /* 0x2C the strips EntityCollision_DistanceToGround drops a point onto */
    char pad30[0x08];
    u16 wall_strip_count;           /* 0x38 how many strips wall_strips has */
    u16 floor_strip_count;          /* 0x3A */
    u16 second_wall_strip_count;    /* 0x3C how many strips second_wall_strips has */
    u16 barrier_strip_count;        /* 0x3E how many strips barrier_strips has */
    u16 hazard_strip_count;         /* 0x40 how many strips hazard_strips has */
    u16 water_strip_count;          /* 0x42 how many strips water_strips has */
} CollisionMesh;

/* CollisionMesh with its strip lists and their lengths as arrays (Collision_CastRayAtMap picks them by index;
   Collision_CastRayAtMapLists goes through seven, one more than CollisionMesh names) */
typedef struct {
    char pad00[0x18];
    CollisionStrip *strip_lists[7]; /* 0x18 strip_lists, floor_strips, second_wall_strips, barrier_strips, hazard_strips, water_strips, and one at 0x30 */
    char pad34[0x04];
    u16 strip_counts[7];            /* 0x38 */
} CollisionMeshLists;

/* a MapRecord (entity/entity.h) as the collision code reads it: the box it covers and its model */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 rotation[4];                /* 0x10 */
    f32 scale[4];                   /* 0x20 */
    u16 model_index;                /* 0x30 0xFFFF: always drawn (Map_ClassifyRecordOnScreen) */
    u16 collision_mesh_index;       /* 0x32 0xFFFF: unused */
    char pad34[0x04];
    u8 flag_block_bit;              /* 0x38 goes to PlayerState.record_flag_block when the player stands on the record */
    char pad39[0x03];
    u32 record_flags;               /* 0x3C high half of MapRecord.flag_block_bit; 0x40000000: nothing collides with it */
    f32 box_lowest[4];              /* 0x40 a point, moved with the record */
    f32 box_highest[4];             /* 0x50 a point, moved with the record */
    f32 lowest[4];                  /* 0x60 lowest x, y, z it covers */
    f32 highest[3];                 /* 0x70 highest x, y, z it covers */
    ModelInstance *model;           /* 0x7C */
} MapRecordBounds;

/* which of a mesh's strip lists (CollisionMeshLists) a hit is tested against: local tables
   that Attack_TestMapModel, Attack_TestMapModelFloors and Attack_TestMapModelWalls copy whole from read-only data */
typedef struct {
    s32 lists[4];                   /* 0x00 */
} CollisionListPick4;

typedef struct {
    s32 lists[2];                   /* 0x00 */
} CollisionListPick2;

/* The tests of collision/collision.c; each leaves its result in COLLISION_RESULT. The cylinder tests are left out: their
   callers declare them with other pointer types (void *). */

/* A sphere against a sphere. */
void Collision_SphereSphere(void *center_a, void *center_b, float radius_a, float radius_b);

/* A point within `reach` of a triangle's plane, its foot inside the triangle. */
void Collision_PointTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *point, f32 reach);

/* An upright capsule (a sphere of `radius` swept down from `centre` by `height`) against a triangle. */
void Collision_CapsuleTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *centre, f32 radius, f32 height);

/* A ray from `origin` along `direction`, no further than `reach` times it, against a triangle. */
void Collision_RayTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *origin, f32 *direction, f32 reach);

/* A ball against a wall edge (Collision_CylinderEdge with a cylinder twice its radius high). */
void Collision_BallEdge(void *edge_start, void *edge_end, f32 height, void *centre, f32 reach);

/* An upright cylinder against a wall edge `height` high, from its front side. */
void Collision_CylinderEdge(void *edge_start, void *edge_end, f32 height, void *centre, f32 radius, f32 reach);

/* Collision_CylinderEdge with the edge made longer by `reach` at both ends. */
void Collision_CylinderEdgeExtended(Vector4 *start, Vector4 *end, f32 height, Vector4 *centre, f32 radius, f32 reach);

/* A ball against a swing: a slab `depth` deep, `range` out, `half_angle` either side of `facing`. */
void Collision_BallSwing(void *ball_centre, void *swing_origin, f32 radius, f32 range, f32 depth, f32 half_angle,
                         f32 facing);

/* Collision_BallSwing for the player's swing: the slab between two heights, and a pitch window. */
void Collision_BallSwingPitch(void *ball_centre, void *swing_origin, f32 radius, f32 range, f32 lowest, f32 highest,
                              f32 half_angle, f32 facing, f32 half_pitch, f32 pitch);

/* collision/collision.c */

/* collision/collision_mesh.c */
char *Collision_RelocateMesh(char *data, ModelInstance *model, int use_scratchpad);
u32 Collision_GetFloorMaterial(int first, u32 second, u32 third);
u32 Collision_GetSurfaceEffect(u32 first, u32 second, int third);
u32 Collision_GetBarrierLevel(u32 first, u32 second, int third);
u32 Collision_IsBarrierFloor(u32 first, u32 second, int third);

/* collision/world_collision.c */
int Collision_TransformMeshPoints(void *target);
void Collision_DropOnTriangle(void *corner_a, void *corner_b, void *corner_c, void *point);
void Collision_DropOnTriangleWithin(void *corner_a, void *corner_b, void *corner_c, void *point, f32 reach);
int Collision_FindFloorBelow(f32 reach, void *point_arg);
int Collision_FindWaterBelow(f32 reach, void *point_arg);
void Collision_CastRayAtMap(f32 scale, void *position_arg, void *direction);
void Collision_CastRayAtMapLists(f32 scale, void *position_arg, void *direction, s32 lists);
void Collision_CastSegmentAtMap(f32 *position, f32 *target);
void Collision_HitRayObj(MapObject *object, f32 *origin, f32 *direction, f32 reach);
void Collision_HitSphereObj(MapObject *object, f32 *point, f32 reach);
void Collision_PushOutOfObjects(f32 *centre, s32 measure, f32 radius, f32 push);
s32 Collision_StandOnObjects(f32 *centre, s32 measure, f32 radius, f32 push);
void Collision_HitRayBoxObj(void *target, f32 *from, f32 *direction, float distance);
void Collision_HitRayDoubleDoorObj(void *target, f32 *from, f32 *direction, float distance);
s32 Collision_HitRayQuadObj(AnimObject *object, f32 *param1, f32 *param2, f32 param3);
void Collision_HitRayPitfallObj(void *target, f32 *from, f32 *direction, float distance);
void Collision_HitRayMeshObj(MapObject *object, f32 *origin, f32 *direction, f32 reach);
int Collision_HitSphereBoxObj(MapObject *object, f32 *point, f32 reach);
int Collision_HitSphereDoubleDoorObj(MapObject *object, f32 *point, f32 reach);
void Collision_HitSphereQuadObj(MapObject *object, f32 *point, f32 reach);
void Collision_HitSpherePitfallObj(MapObject *object, f32 *point, f32 reach);
void Collision_HitSphereMeshObj(CollisionOwner *owner, f32 *point, float factor);
int Collision_HitCapsuleBoxObj(MapObject *object, f32 *point, f32 radius, f32 push);
void Collision_HitCapsuleDoubleDoorObj(MapObject *object, f32 *centre, f32 radius, f32 push);
int Collision_HitCapsuleQuadObj(CollisionOwner *owner, f32 *point, float first_value, float second_value);
void Collision_HitCapsulePitfallObj(void);
void Collision_HitCapsuleMeshObj(MapObject *object, f32 *centre, f32 height, f32 radius);
void Collision_HitFanBoxObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x, f32 second_z);
void Collision_HitFanDoubleDoorObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x, f32 second_z);
void Collision_HitFanQuadObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x, f32 second_z);
void Collision_HitFanPitfallObj(void);
void Collision_HitFanMeshObj(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x, f32 second_z);
void Collision_PushOutOfBoxObj(MapObject *object, f32 *centre, f32 radius, f32 push);
void Collision_PushOutOfDoubleDoorObj(MapObject *object, f32 *centre, f32 radius, f32 push);
void Collision_PushOutOfQuadObj(CollisionOwner *owner, f32 *position, f32 radius, f32 push);
void Collision_PushOutOfPitfallObj(void);
void Collision_PushOutOfMeshObj(MapObject *object, f32 *position, f32 height, f32 radius);
s32 Collision_StandOnQuadObj(CollisionOwner *owner, f32 *position, f32 radius);
s32 Collision_StandOnPitfallObj(MapObject *object, f32 *position, f32 radius);
s32 Collision_StandOnMeshObj(CollisionOwner *owner, f32 *position, f32 radius);

/* collision/world_strip_lists.c */

/* data still in asm */
extern ModelInstanceBlock *map_slot_record_models[];
extern char hit_sphere_pitfall_message[];
extern f32 negative_zero_along_limit[];
extern const f32 negative_zero_drop_within_limit[];
extern const f32 largest_float_floor_lowest_seed[];
extern const f32 largest_float_water_lowest_seed[];
extern const f32 largest_float_ray_best_distance[];
extern const f32 largest_float_ray_lists_best_distance[];
extern s32 segment_strip_lists[2];
extern const f32 largest_float_segment_best_distance[];
extern f32 largest_float_tested_point_distance[];
extern f32 largest_float_stand_farthest_start[];
extern f32 largest_float_mesh_lowest_seed[];  /* the largest float; an array so that it is not read through $gp */
extern f32 largest_float_quad_lowest_seed[];  /* the largest float; an array so that it is not read through $gp */
extern f32 largest_float_strip_lowest_seed[];  /* the largest float; an array so that it is not read through $gp */
extern const f32 negative_zero_point_triangle_limit[];  /* -0.0f */
extern const f32 negative_zero_capsule_limit[];  /* -0.0f */
extern const f32 negative_zero_ray_triangle_limit[];  /* -0.0f */
extern const f32 negative_zero_drop_triangle_limit[];  /* -0.0f */
extern const f32 negative_zero_ray_cylinder_limit[];  /* -0.0f */
extern const f32 negative_zero_sphere_cylinder_limit[];  /* -0.0f */

/* collision/collision.c */
void Collision_CylinderCylinder(f32 *base, f32 radius, f32 height, f32 *other_base, f32 other_radius, f32 other_height);
void Collision_RayCylinder(f32 radius, f32 height, f32 length, f32 *base, f32 *origin, f32 *direction);
void Collision_SphereCylinder(f32 *base, f32 *centre, f32 radius, f32 height, f32 reach);
void Collision_CylinderArc(f32 *base, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_height, f32 half_angle, f32 facing);
void Collision_CylinderArcPitch(f32 *base, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_low, f32 other_high, f32 half_angle, f32 facing, f32 pitch_range, f32 pitch);

/* collision/world_strip_lists.c */
extern s32 strip_list_indices[8];

#endif
