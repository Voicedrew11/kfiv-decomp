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

/* the same vector, 8-byte aligned: how it is passed by value */
typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} __attribute__((aligned(8))) Vector4Arg;

/* a 128-bit value moved as two doublewords */
typedef struct {
    u64 unk00;
    u64 unk08;
} Qword;

/* one vertex of a strip */
typedef struct {
    void *unk00;                    /* 0x00 */
    s32 unk04;                      /* 0x04 */
    s32 unk08;                      /* 0x08 */
    s32 unk0C;                      /* 0x0C */
} StripVertex;

/* a strip: a header, then vertex_count vertices */
typedef struct {
    u16 vertex_count;               /* 0x00 */
    char pad02[0x1E];
    StripVertex vertices[1];      /* 0x20 vertex_count of them */
} Strip;

/* ModelInstance.strips: the strips back to back */
typedef struct {
    s32 strip_count;                /* 0x00 */
    Strip *strips;                  /* 0x04 */
    s32 unk08;                      /* 0x08 */
    u32 unk0C;                      /* 0x0C */
} ModelStripList;

/* one 0x80-byte part (joint) of a model, ModelInstance.parts[i] */
typedef struct ModelPart {
    char unk00[0x40];               /* 0x00 the part's matrix, passed by address */
    f32 unk40[4];                   /* 0x40 where it sits on its parent */
    f32 unk50;                      /* 0x50 an angle, wrapped to +-pi (about x) */
    f32 unk54;                      /* 0x54 angle about y */
    f32 unk58;                      /* 0x58 angle about z */
    char pad5C[0x10];
    u8 unk6C;                       /* 0x6C set by func_0019A430 */
    char pad6D[0x03];
    struct ModelPart *child;        /* 0x70 first part hanging off this one, NULL: none */
    struct ModelPart *sibling;      /* 0x74 next part on the same parent, NULL: none */
    u64 data_addr : 32;             /* 0x78 */
    u64 pad78_32 : 7;
    u64 level : 7;                  /* detail level picked by func_00192DA8 */
    u64 pad78_46 : 7;
    u64 data_count : 11;
} ModelPart;

/* ModelPart with its last doubleword (the bit-fields) whole. Bits 32-38 clear:
   func_0018BD18 leaves the part's turn and offset out. */
typedef struct {
    char pad00[0x78];
    u64 unk78;                      /* 0x78 */
} ModelPartWords;

#define MODEL_PART_WORD(part) (((ModelPartWords *)(part))->unk78)

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

/* the 4 bytes in front of each block ModelInstance.level_tables[n] points at */
typedef struct {
    u16 unk00;                      /* 0x00 read by func_0019A418 */
    char pad02[0x02];
} ModelTableHeader;

#define MODEL_FLAG_800  0x800

typedef struct ModelInstance {
    char pad00[0x40];
    char unk40[0x40];               /* 0x40 a matrix, passed by address */
    char pad80[0x40];
    Vector4 position;               /* 0xC0 */
    Vector4 rotation;               /* 0xD0 x, y (heading), z in radians */
    f32 unkE0;                      /* 0xE0 its sign picks the turn direction */
    char padE4[0x04];
    f32 unkE8;                      /* 0xE8 */
    char padEC[0x04];
    u16 part_count;                 /* 0xF0 */
    char padF2[0x06];
    ModelPart *parts;               /* 0xF8 */
    ModelPartLevels **level_tables; /* 0xFC */
    u16 unk100;                     /* 0x100 */
    u16 unk102;                     /* 0x102 MODEL_FLAG_800: drawn with its own texture */
    char pad104[0x0C];
    ModelStripList *strips;         /* 0x110 also read as a CollisionMesh (kf_collision.h), NULL: none */
    char pad114[0x0C];
    BoneAnchor *bone_anchors;       /* 0x120 32 of them */
    Vector4 *bone_positions;        /* 0x124 32 of them, written from bone_anchors */
    char pad128[0x20];
    s32 unk148;                     /* 0x148 */
    void *unk14C;                   /* 0x14C */
    char pad150[0x0C];
    f32 alpha;                      /* 0x15C below 128: drawn without z writes */
    char pad160[0x30];
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

/* what MapObject.unk08 points at */
typedef struct {
    char pad00[0x40];
    u16 unk40;                      /* 0x40 passed to func_002109C8, 0: nothing */
} MapObjectUnk08;

/* what MapObject.unk04 points at */
typedef struct {
    u32 kind : 8;                   /* 0x00 1, 2, 6, 0x11: func_0018C728 tests lines of sight against it */
    u32 pad00_8 : 24;
} MapObjectUnk04;

/* AnimObject+0x40 */
typedef struct {
    char pad00[0x14];
    s32 unk14;                      /* 0x14 */
    s32 unk18;                      /* 0x18 */
} AnimObjectBody;

/* an animated object: the model, its animation table and a small state machine */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    void *anims;                    /* 0x04 8-byte entries */
    char pad08[0x08];
    u8 unk10;                       /* 0x10 the flag word is ANIM_OBJECT_FLAGS */
    u8 mode;                        /* 0x11 */
    char pad12[0x02];
    char pad14[0x2C];
    AnimObjectBody body;            /* 0x40 */
} AnimObject;

/* the 32-bit flags at AnimObject+0x10 */
#define ANIM_OBJECT_FLAGS(object) (*(u32 *)&(object)->unk10)

/* the objects of a map, looked up by func_0011E1F0(map, id) */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    MapObjectUnk04 *unk04;          /* 0x04 */
    MapObjectUnk08 *unk08;          /* 0x08 */
    char pad0C[0x04];
    u32 unk10;                      /* 0x10 flags */
    f32 unk14;                      /* 0x14 how far around its position it reaches, 0: nothing to hit */
    char pad18[0x58];
} MapObject;

/* the props of a map, looked up by func_00116160(map, id) */
typedef struct {
    char pad00[0x32];
    u16 unk32;                      /* 0x32 0xFFFF: nothing to collide with */
    char pad34[0x48];
    ModelInstance *model;           /* 0x7C */
} MapProp;

/* what D_004469D8 points at: one of the nodes at D_0057E240. D_004469D4 heads
   the list of the ones in use. */
typedef struct ModelNode {
    char pad00[0x18];
    ModelInstance *model;           /* 0x18 non-null while the node is in use */
    char pad1C[0x04];
    struct ModelNode *prev;         /* 0x20 */
    struct ModelNode *next;         /* 0x24 */
    char pad28[0x14];
} ModelNode;

/* a motion that turns a model by a fixed step each frame (func_00212778) */
typedef struct {
    s32 active;                     /* 0x00 cleared when the motion ends */
    char pad04[0x04];
    s32 owner_id;                   /* 0x08 */
    char pad0C[0x04];
    f32 delta_x;                    /* 0x10 */
    char pad14[0x04];
    f32 delta_y;                    /* 0x18 */
    char pad1C[0x04];
    f32 delta_z;                    /* 0x20 */
    char pad24[0x04];
    s32 frames_left;                /* 0x28 */
} RotationMotion;

/* the object func_001C5170 tests against a model's points */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    char pad04[0x0C];
    int flags;                      /* 0x10 */
} CollisionOwner;

/* ChannelOwner.channel: the playback state func_00192C98 resets */
typedef struct {
    char pad00[0x80];
    u8 limit;                       /* 0x80 entries at or above this index are replaced by 0 */
    u8 index;                       /* 0x81 */
    s16 param;                      /* 0x82 */
    s32 param_shifted;              /* 0x84 param << 8 */
    u32 value;                      /* 0x88 */
    u16 word;                       /* 0x8C */
} ModelChannel;

/* ChannelOwner.vec: the vector that is cleared on a switch */
typedef struct {
    char pad00[0x40];
    f32 x;                          /* 0x40 */
    f32 y;                          /* 0x44 */
    f32 z;                          /* 0x48 */
} ChannelVector;

/* what ChannelActor.owner points at */
typedef struct {
    char pad00[0xFC];
    ModelChannel *channel;          /* 0xFC */
    char pad100[0x30];
    ChannelVector *vec;             /* 0x130 */
} ChannelOwner;

/* an actor that switches between the entries of a table of levels */
typedef struct {
    ChannelOwner *owner;            /* 0x00 */
    char pad04[0x08];
    u8 *table;                      /* 0x0C levels at +0x60, words at +0x80 */
    char pad10[0x18];
    u16 enabled;                    /* 0x28 */
    char pad2A[0x9C];
    u16 flags;                      /* 0xC6 */
    char padC8[0x08];
    u8 old_index;                   /* 0xD0 */
    s8 cur_index;                   /* 0xD1 */
} ChannelActor;

#endif
