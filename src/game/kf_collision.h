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

/* what ModelInstance.unk110 points at */
typedef struct {
    s32 strip_count;                /* 0x00 */
    CollisionStrip *strips;         /* 0x04 */
    char pad08[0x04];
    u32 unk0C;                      /* 0x0C */
    char pad10[0x08];
    CollisionStrip *unk18;          /* 0x18 unk38 strips */
    CollisionStrip *floor_strips;   /* 0x1C */
    CollisionStrip *unk20;          /* 0x20 unk3C strips */
    CollisionStrip *unk24;          /* 0x24 unk3E strips */
    CollisionStrip *unk28;          /* 0x28 unk40 strips */
    CollisionStrip *unk2C;          /* 0x2C the strips func_0018B518 drops a point onto */
    char pad30[0x08];
    u16 unk38;                      /* 0x38 how many strips unk18 has */
    u16 floor_strip_count;          /* 0x3A */
    u16 unk3C;                      /* 0x3C how many strips unk20 has */
    u16 unk3E;                      /* 0x3E how many strips unk24 has */
    u16 unk40;                      /* 0x40 how many strips unk28 has */
    u16 unk42;                      /* 0x42 how many strips unk2C has */
} CollisionMesh;

/* a MapRecord (kf_entity.h) as the collision code reads it: the box it covers and its model */
typedef struct {
    char pad00[0x32];
    u16 unk32;                      /* 0x32 0xFFFF: unused */
    char pad34[0x08];
    u32 unk3C;                      /* 0x3C high half of MapRecord.unk38; 0x40000000: nothing collides with it */
    char pad40[0x20];
    f32 lowest[4];                  /* 0x60 lowest x, y, z it covers */
    f32 highest[3];                 /* 0x70 highest x, y, z it covers */
    ModelInstance *model;           /* 0x7C */
} MapRecordBounds;

#endif
