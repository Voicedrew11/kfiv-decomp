/* Model instances: the posed, drawable body an entity or map object points at. */
#ifndef KF_MODEL_H
#define KF_MODEL_H

#include "common.h"

/* an x, y, z, w vector (VU0 order) */
typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

/* the same vector as a 16-byte aligned VU0 quadword, for whole-vector copies */
typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} __attribute__((aligned(16))) VuVector4;

/* a 128-bit value moved as two doublewords */
typedef struct {
    u64 unk00;
    u64 unk08;
} Qword;

/* the same 128 bits at any alignment, copied as two unaligned doublewords */
typedef struct __attribute__((packed)) {
    u64 unk00;
    u64 unk08;
} UnalignedQword;

/* a 128-bit integer, stored with one sq */
typedef u32 Uint128 __attribute__((mode(TI)));

/* one 0x80-byte part (joint) of a model, ModelInstance.parts[i] */
typedef struct {
    char unk00[0x40];               /* 0x00 the part's matrix, passed by address */
    char pad40[0x10];
    f32 unk50;                      /* 0x50 an angle, wrapped to +-pi */
    char pad54[0x24];
    u64 data_addr : 32;             /* 0x78 */
    u64 pad78_32 : 7;
    u64 level : 7;                  /* detail level picked by func_00192DA8 */
    u64 pad78_46 : 7;
    u64 data_count : 11;
} ModelPart;

/* one record of a part's data (ModelPart.data_addr): 0x80 bytes plus its payload */
typedef struct {
    char pad00[0x40];
    int id_word;                    /* 0x40 low 14 bits: the record id */
    char pad44[0x1C];
    int payload_size;               /* 0x60 low 15 bits: the payload length in 0x40-byte units */
    char pad64[0x0C];
} ModelPartRecord;

/* per-part level thresholds, ModelInstance.level_tables[n][part] */
typedef struct {
    u8 count;                       /* 0x00 */
    char pad01[0x0B];
    u16 *thresholds;                /* 0x0C u16 values at a stride of 0x12 bytes */
} ModelPartLevels;

/* where a bone sits: an offset from a part's origin (bone 0xFF: the model origin) */
typedef struct {
    f32 offset[3];                  /* 0x00 */
    u8 bone;                        /* 0x0C */
    char pad0D[0x03];
} BoneAnchor;

#define MODEL_FLAG_800  0x800

typedef struct ModelInstance {
    char pad00[0x40];
    char unk40[0x40];               /* 0x40 a matrix, passed by address */
    char pad80[0x40];
    Vector4 position;               /* 0xC0 */
    Vector4 rotation;               /* 0xD0 x, y (heading), z in radians */
    f32 unkE0;                      /* 0xE0 its sign picks the turn direction */
    f32 unkE4;                      /* 0xE4 */
    f32 unkE8;                      /* 0xE8 */
    char padEC[0x04];
    u16 part_count;                 /* 0xF0 */
    char padF2[0x06];
    ModelPart *parts;               /* 0xF8 */
    ModelPartLevels **level_tables; /* 0xFC */
    u16 unk100;                     /* 0x100 */
    u16 unk102;                     /* 0x102 MODEL_FLAG_800: drawn with its own texture */
    char pad104[0x1C];
    BoneAnchor *bone_anchors;       /* 0x120 32 of them */
    Vector4 *bone_positions;        /* 0x124 32 of them, written from bone_anchors */
    char pad128[0x34];
    f32 alpha;                      /* 0x15C below 128: drawn without z writes */
    char pad160[0x1C];
    int found_record_id;            /* 0x17C the id func_001A2E58 last found */
    int found_record[4];            /* 0x180 a copy of that record, its id bits replaced */
    u16 unk190;                     /* 0x190 */
    char pad192[0x02];
    int unk194;                     /* 0x194 */
    char pad198[0x04];
    int unk19C;                     /* 0x19C */
    int unk1A0;                     /* 0x1A0 */
    char pad1A4[0x08];
    void (*draw)(struct ModelInstance *model);  /* 0x1AC sets the GS state and draws */
    void (*unk1B0)(void);           /* 0x1B0 */
    char pad1B4[0x04];
    int (*get_point)(void *model, int index);   /* 0x1B8 */
} ModelInstance;

/* the objects of a map, looked up by func_0011E1F0(map, id) */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    char pad04[0x0C];
    u32 unk10;                      /* 0x10 flags */
} MapObject;

/* the props of a map, looked up by func_00116160(map, id) */
typedef struct {
    char pad00[0x7C];
    ModelInstance *model;           /* 0x7C */
} MapProp;

/* what D_004469D8 points at */
typedef struct {
    char pad00[0x18];
    ModelInstance *model;           /* 0x18 */
} ModelNode;

/* the object func_001C5170 tests against a model's points */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    char pad04[0x0C];
    int flags;                      /* 0x10 */
} CollisionOwner;

#endif
