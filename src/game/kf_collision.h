/* Collision tests: the result block in the scratchpad and the meshes tested against. */
#ifndef KF_COLLISION_H
#define KF_COLLISION_H

#include "common.h"
#include "kf_model.h"

/* what every collision test leaves at the start of the scratchpad */
typedef struct {
    f32 point[4];                   /* 0x00 where it hit */
    s32 hit;                        /* 0x10 0: no hit */
    f32 distance;                   /* 0x14 */
    s32 unk18;                      /* 0x18 */
    char pad1C[0x04];
    f32 normal[4];                  /* 0x20 unit vector away from what was hit */
} CollisionResult;

#define COLLISION_RESULT ((CollisionResult *)0x70000000)

/* the scratchpad from 0x100 on: the points of a model's collision mesh (CollisionMesh.unk08)
   placed in the world by func_001C2D08; the strips point at these */
#define COLLISION_WORLD_POINTS ((Vector4 *)0x70000100)

/* the scratchpad as func_001BE760 uses it. Reached as fields of one block at 0x70000000, retail forms each
   address anew (`lui $1`); a pointer constant such as (Vector4 *)0x70000080 is kept in a register instead. */
typedef struct {
    CollisionResult result;         /* 0x00 */
    char pad30[0x40];
    Vector4 push;                   /* 0x70 the way out of what was hit */
    Vector4 point;                  /* 0x80 the point that is tested */
    Vector4 unk90;                  /* 0x90 the point func_001BDDC0 keeps away from the walls */
} CollisionScratch;

#define COLLISION_SCRATCH ((CollisionScratch *)0x70000000)

/* a CollisionResult kept aside and put back (func_001C0E40): retail moves it by doublewords */
typedef union {
    CollisionResult result;
    s64 doublewords[6];
} CollisionResultCopy;

#define COLLISION_RESULT_COPY ((CollisionResultCopy *)0x70000000)

/* a CollisionResult aligned to 8 bytes: copied whole it moves by doublewords, and its members
   keep their own types (func_001C1C80, where the union above does not match) */
typedef struct {
    f32 point[4];                   /* 0x00 */
    s32 hit;                        /* 0x10 */
    f32 distance;                   /* 0x14 */
    s32 unk18;                      /* 0x18 */
    char pad1C[0x04];
    f32 normal[4];                  /* 0x20 */
} __attribute__((aligned(8))) CollisionResultAligned;

#define COLLISION_RESULT_ALIGNED ((CollisionResultAligned *)0x70000000)

/* one point of a triangle strip */
typedef struct {
    void *position;                 /* 0x00 */
    s32 unk04;                      /* 0x04 */
    s32 unk08;                      /* 0x08 */
    s32 unk0C;                      /* 0x0C */
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
    char *unk08;                    /* 0x08 the end of the strips: unk0C 16-byte entries */
    u32 unk0C;                      /* 0x0C */
    f32 unk10;                      /* 0x10 how far it reaches from its origin, unscaled (MapObject_InitMap) */
    char *unk14;                    /* 0x14 the next mesh of a map file starts this many bytes on (a size, typed as func_00114AA8 needs) */
    CollisionStrip *unk18;          /* 0x18 unk38 strips */
    CollisionStrip *floor_strips;   /* 0x1C */
    CollisionStrip *unk20;          /* 0x20 unk3C strips */
    CollisionStrip *unk24;          /* 0x24 unk3E strips */
    CollisionStrip *unk28;          /* 0x28 unk40 strips */
    CollisionStrip *unk2C;          /* 0x2C the strips EntityCollision_DistanceToGround drops a point onto */
    char pad30[0x08];
    u16 unk38;                      /* 0x38 how many strips unk18 has */
    u16 floor_strip_count;          /* 0x3A */
    u16 unk3C;                      /* 0x3C how many strips unk20 has */
    u16 unk3E;                      /* 0x3E how many strips unk24 has */
    u16 unk40;                      /* 0x40 how many strips unk28 has */
    u16 unk42;                      /* 0x42 how many strips unk2C has */
} CollisionMesh;

/* CollisionMesh with its strip lists and their lengths as arrays (func_001C1430 picks them by index;
   func_001C1858 goes through seven, one more than CollisionMesh names) */
typedef struct {
    char pad00[0x18];
    CollisionStrip *strip_lists[7]; /* 0x18 unk18, floor_strips, unk20, unk24, unk28, unk2C, and one at 0x30 */
    char pad34[0x04];
    u16 strip_counts[7];            /* 0x38 */
} CollisionMeshLists;

/* a MapRecord (kf_entity.h) as the collision code reads it: the box it covers and its model */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 rotation[4];                /* 0x10 */
    f32 scale[4];                   /* 0x20 */
    u16 unk30;                      /* 0x30 0xFFFF: always drawn (func_00116CB0) */
    u16 unk32;                      /* 0x32 0xFFFF: unused */
    char pad34[0x04];
    u8 unk38;                       /* 0x38 goes to PlayerState.unk250 when the player stands on the record */
    char pad39[0x03];
    u32 unk3C;                      /* 0x3C high half of MapRecord.unk38; 0x40000000: nothing collides with it */
    f32 unk40[4];                   /* 0x40 a point, moved with the record */
    f32 unk50[4];                   /* 0x50 a point, moved with the record */
    f32 lowest[4];                  /* 0x60 lowest x, y, z it covers */
    f32 highest[3];                 /* 0x70 highest x, y, z it covers */
    ModelInstance *model;           /* 0x7C */
} MapRecordBounds;

/* which of a mesh's strip lists (CollisionMeshLists) a hit is tested against: local tables
   that func_001B21F8, func_001B24B0 and func_001B2740 copy whole from read-only data */
typedef struct {
    s32 lists[4];                   /* 0x00 */
} CollisionListPick4;

typedef struct {
    s32 lists[2];                   /* 0x00 */
} CollisionListPick2;

/* The tests of kf_collision.c; each leaves its result in COLLISION_RESULT. The cylinder tests are left out: their
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

#endif
