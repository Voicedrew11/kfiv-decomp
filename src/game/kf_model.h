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

/* the same 128 bits at any alignment, copied as two unaligned doublewords */
typedef struct __attribute__((packed)) {
    u64 unk00;
    u64 unk08;
} UnalignedQword;

/* four ints as a 16-byte aligned VU0 quadword (screen coordinates from the VU0) */
typedef s32 VuIntVector[4] __attribute__((aligned(16)));

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
    f32 unk5C;                      /* 0x5C 1.0 */
    f32 unk60[3];                   /* 0x60 the third vector of a motion key (scale, it seems) */
    u8 unk6C;                       /* 0x6C set by func_0019A430 */
    char pad6D[0x03];
    struct ModelPart *child;        /* 0x70 first part hanging off this one, NULL: none */
    struct ModelPart *sibling;      /* 0x74 next part on the same parent, NULL: none */
    u64 data_addr : 32;             /* 0x78 */
    u64 unk78_32 : 7;               /* MotionPart.unk02 of the motion last loaded */
    u64 level : 7;                  /* detail level picked by Motion_SeekKeys */
    u64 pad78_46 : 7;
    u64 data_count : 11;
} ModelPart;

/* one vertex of a ModelPartData block */
typedef struct {
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    char pad06[0x08];
    u16 unk0E;                      /* 0x0E flag bits func_00111368 passes on to the GS vertex */
    u16 tex_u;                          /* 0x10 texture coordinates */
    u16 tex_v;                          /* 0x12 */
    u16 unk14;                      /* 0x14 set to 1 by func_00111010 */
    char pad16[0x02];
    s16 colour[4];                  /* 0x18 red, green, blue, alpha: 0..0xFF each (set by func_0010E8E0) */
} ModelPartVertex;

/* a ModelPartVertex with every field a signed word, as the code that builds vertices between
   two others reads it (func_0010F898, func_0010FC30) */
typedef struct {
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    s16 unk06;                      /* 0x06 1 on a vertex built between two others */
    s16 unk08[3];                   /* 0x08 a normal, it seems */
    u16 unk0E;                      /* 0x0E 0; 0x8000 on the first two vertices of a strip (func_0010F478) */
    s16 tex_u;                          /* 0x10 */
    s16 tex_v;                          /* 0x12 */
    s16 unk14;                      /* 0x14 0x1000 */
    s16 unk16;                      /* 0x16 0x7FFF on a vertex built between two others, 0 on the two ends */
    s16 colour[4];                  /* 0x18 */
} ModelPartVertexWords;

/* a ModelPartData block from 0x70 on, for the models whose vertices start at 0x80 (func_00111010) */
typedef struct {
    s32 unk00;                      /* 0x00 low 15 bits: how many vertices */
    char pad04[0x0C];
    ModelPartVertex vertices[1];    /* 0x10 */
} ModelPartDataTail;

/* the doubleword at ModelPartData+0x30 */
typedef struct {
    u64 pad0 : 37;
    u64 unk37 : 14;                 /* not 0 on the blocks func_0010B758 looks for */
    u64 pad51 : 10;
    u64 unk61 : 3;                  /* set to 2 on the first block of a model whose unk37 is not 0 */
} ModelPartDataBits30;

/* one block of a part's data (ModelPart.data_addr): 0x90 bytes of GS setup, then the vertices.
   The next block starts (length + 1) * 16 bytes after this one. */
typedef struct {
    u16 length;                     /* 0x00 in 16-byte units, not counting the first */
    char pad02[0x2E];
    ModelPartDataBits30 unk30;      /* 0x30 */
    char pad38[0x08];
    u64 unk40;                      /* 0x40 low 14 bits: the texture it draws with */
    char pad48[0x08];
    u64 unk50;                      /* 0x50 set by func_0010E820 */
    char pad58[0x18];
    s32 unk70;                      /* 0x70 low 15 bits: how many vertices */
    s32 unk74;                      /* 0x74 */
    char pad78[0x18];
    ModelPartVertex vertices[1];    /* 0x90 */
} ModelPartData;

/* one record of a part's data (ModelPart.data_addr): 0x80 bytes plus its payload */
typedef struct {
    char pad00[0x40];
    int id_word;                    /* 0x40 low 14 bits: the record id */
    char pad44[0x1C];
    int payload_size;               /* 0x60 low 15 bits: the payload length in 0x40-byte units */
    char pad64[0x0C];
} ModelPartRecord;

/* ModelPart with its last doubleword (the bit-fields) whole. Bits 32-38 clear:
   Bone_MultiplyPathTo leaves the part's turn and offset out. */
typedef struct {
    char pad00[0x78];
    u64 unk78;                      /* 0x78 */
} ModelPartWords;

#define MODEL_PART_WORD(part) (((ModelPartWords *)(part))->unk78)

/* the same doubleword through a union, for where retail reads it again after stores of other
   types (func_0010BA30) */
typedef union {
    u64 word;
    u32 halves[2];
} ModelPartWordUnion;

#define MODEL_PART_WORD_ANY(part) (((ModelPartWordUnion *)&MODEL_PART_WORD(part))->word)

/* ModelPart.pad78_46, the part this one hangs off, read that way */
#define MODEL_PART_PARENT_ANY(part) ((s32)(MODEL_PART_WORD_ANY(part) >> 46) & 0x7F)

/* one 0x40-byte vertex of a ModelWideData block */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 offset[4];                  /* 0x10 added to the position before it is projected (func_0010CB98) */
    f32 tex_s;                          /* 0x20 texture coordinates, 0 to 1 across the screen */
    f32 tex_t;                          /* 0x24 */
    char pad28[0x04];
    s32 on_parent;                  /* 0x2C 0: placed by its own part's matrix, otherwise by the parent part's */
    char pad30[0x10];
} ModelWideVertex;

/* a block of a part's data in the models drawn with their own texture (MODEL_FLAG_800):
   0xB0 bytes of GS setup, then 0x40-byte vertices */
typedef struct {
    u16 length;                     /* 0x00 in 16-byte units, not counting the first */
    char pad02[0x8E];
    s32 unk90;                      /* 0x90 low 15 bits: how many vertices */
    char pad94[0x1C];
    ModelWideVertex vertices[1];    /* 0xB0 */
} ModelWideData;

/* the doubleword at ModelPartData+0x70 (the block's GIF tag) */
typedef struct {
    u64 pad0 : 53;
    u64 blended : 1;                /* the alpha blending bit of the primitive */
    u64 pad54 : 10;
} ModelPartDataBits70;

/* one part of a PosedModelFile */
typedef struct {
    f32 position[3];                /* 0x00 where the part rests: ModelPart.unk40 */
    f32 rotation[3];                /* 0x0C ModelPart.unk50, unk54, unk58 */
    f32 scale[3];                   /* 0x18 ModelPart.unk60 */
    s32 data_offset;                /* 0x24 from the start of the file to the part's ModelPartData blocks */
    u16 data_count;                 /* 0x28 how many blocks */
    char pad2A[0x06];
} PosedModelFilePart;

/* a model file as func_0010D018 reads it: every part comes with the pose it rests in */
typedef struct {
    s32 size;                       /* 0x00 the next file starts this many bytes on */
    u16 part_count;                 /* 0x04 */
    char pad06[0x06];
    s32 bone_anchors_offset;        /* 0x0C from the start of the file to 32 BoneAnchor, 0: none */
    PosedModelFilePart parts[1];    /* 0x10 part_count of them */
} PosedModelFile;

/* ModelPart with the bit-fields that share a doubleword with unk60[2] */
typedef struct {
    char pad00[0x68];
    u64 pad68_0 : 40;
    u64 bit40 : 1;                  /* 0x6D bit 0, set: func_0010E010 does not draw the part */
    u64 bit41 : 1;                  /* 0x6D bit 1, set by func_0010B758 */
    u64 pad68_42 : 22;
} ModelPartFlagBits;

#define MODEL_PART_FLAG_BITS(part) ((ModelPartFlagBits *)(part))

/* the same doubleword whole, for the code that tests it with masks */
typedef struct {
    char pad00[0x68];
    u64 unk68;                      /* 0x68 */
} ModelPartFlagWord;

#define MODEL_PART_FLAG_WORD(part) (((ModelPartFlagWord *)(part))->unk68)
#define MODEL_PART_BIT40 (0x8000L << 25)
#define MODEL_PART_BIT41 (0x8000L << 26)

/* per-part level thresholds, ModelInstance.level_tables[n][part] */
typedef struct {
    u8 count;                       /* 0x00 */
    char pad01;
    u8 unk02;                       /* 0x02 low 7 bits go to ModelPart.unk78_32 */
    u8 child;                       /* 0x03 index of the part's first child, 0: none */
    u8 sibling;                     /* 0x04 index of the next part on the same parent, 0: none */
    char pad05[0x03];
    u32 keys_offset;                /* 0x08 from the start of the MotionData, 0: no keys */
    u16 *thresholds;                /* 0x0C u16 values at a stride of 0x12 bytes (MotionKey), set from keys_offset */
} ModelPartLevels;

/* one 0x12-byte key of a part: three (unknown, unknown, vector index) groups */
typedef struct {
    u16 frame;                      /* 0x00 the threshold */
    u16 length;                     /* 0x02 frames from this key to the next */
    u16 position;                   /* 0x04 index into MotionData.vectors */
    u16 position_in;                /* 0x06 tangent of the position curve arriving at this key */
    u16 position_out;               /* 0x08 tangent of the position curve leaving this key */
    u16 rotation;                   /* 0x0A */
    u16 rotation_in;                /* 0x0C tangent of the rotation curve arriving at this key */
    u16 rotation_out;               /* 0x0E tangent of the rotation curve leaving this key */
    u16 scale;                      /* 0x10 */
} MotionKey;

/* a vector of a motion (12 bytes) */
typedef struct {
    f32 x;
    f32 y;
    f32 z;
} MotionVector;

/* one motion of a MotionFile; func_001086E8 hooks it up to a model */
typedef struct {
    u32 size;                       /* 0x00 the next motion starts this many bytes on */
    u32 vectors_offset;             /* 0x04 */
    MotionVector *vectors;          /* 0x08 set from vectors_offset */
    u16 frame_count;                /* 0x0C ModelTableHeader.unk00 */
    u8 part_count;                  /* 0x0E */
    char pad0F;
    ModelPartLevels parts[1];       /* 0x10 part_count of them; ModelChannel.tables[n] points here */
} MotionData;

/* a 16-byte directory entry of a MotionFile */
typedef struct {
    char pad00[0x10];
} MotionFileEntry;

/* a file of motions: the directory, then the MotionData blocks back to back */
typedef struct {
    char pad00[0x04];
    u32 motion_count;               /* 0x04 */
    char pad08[0x08];
    MotionFileEntry entries[1];     /* 0x10 motion_count of them */
} MotionFile;

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

/* one part of a ModelFile */
typedef struct {
    s32 data_offset;                /* 0x00 from the start of the file to the part's ModelPartData blocks */
    u16 data_count;                 /* 0x04 how many blocks */
    u8 unk06;                       /* 0x06 1..3: the part goes to ModelInstance.unk130[unk06 - 1] */
    s8 unk07;                       /* 0x07 */
    char pad08[0x08];
} ModelFilePart;

/* a model file as func_0010B758 reads it */
typedef struct ModelFile {
    s32 size;                       /* 0x00 the next file starts this many bytes on */
    u16 part_count;                 /* 0x04 */
    char pad06[0x0A];
    BoneAnchor bone_anchors[32];    /* 0x10 */
    ModelFilePart parts[1];         /* 0x210 part_count of them */
} ModelFile;

/* a model file's header through a union of its two layouts: a read through it aliases everything
   (func_0010D018 tests the part count that way, after it has moved the memory cursor) */
typedef union {
    ModelFile plain;
    PosedModelFile posed;
} ModelFileUnion;

#define POSED_MODEL_FILE_ANY(file) (((ModelFileUnion *)(file))->posed)

#define MODEL_FLAG_800  0x800

/* ModelInstance with the three part slots at 0x130 as the array func_0010B758 fills
   (ModelInstance.unk130 is the first) */
typedef struct {
    char pad00[0x130];
    ModelPart *parts[3];            /* 0x130 the parts whose ModelFilePart.unk06 is 1, 2 and 3 */
} ModelInstanceSlots;

#define MODEL_SLOT_PARTS(model) (((ModelInstanceSlots *)(model))->parts)

/* a vertex's own texture coordinates and the wave (kf_screen.h WaveOscillator) that moves them */
typedef struct ModelWaveUv {
    u16 tex_u;                          /* 0x00 */
    u16 tex_v;                          /* 0x02 */
    s32 wave;                       /* 0x04 0-15 */
} ModelWaveUv;

typedef struct ModelInstance {
    char pad00[0x40];
    char unk40[0x40];               /* 0x40 a matrix, passed by address */
    char unk80[0x40];               /* 0x80 a copy of unk40 */
    Vector4 position;               /* 0xC0 */
    Vector4 rotation;               /* 0xD0 x, y (heading), z in radians */
    f32 unkE0;                      /* 0xE0 its sign picks the turn direction; the x scale */
    f32 unkE4;                      /* 0xE4 the y scale */
    f32 unkE8;                      /* 0xE8 the z scale */
    f32 unkEC;                      /* 0xEC */
    u16 part_count;                 /* 0xF0 */
    u16 unkF2;                      /* 0xF2 set to the part count too */
    char padF4[0x04];
    ModelPart *parts;               /* 0xF8 */
    ModelPartLevels **level_tables; /* 0xFC */
    u16 unk100;                     /* 0x100 */
    u16 unk102;                     /* 0x102 MODEL_FLAG_800: drawn with its own texture */
    int unk104;                     /* 0x104 passed to Effect_FindModelRecord, which does not read it */
    char pad108[0x08];
    ModelStripList *strips;         /* 0x110 also read as a CollisionMesh (kf_collision.h), NULL: none */
    MotionFile *motion_file;        /* 0x114 */
    char pad118[0x04];
    ModelFile *file;                /* 0x11C */
    BoneAnchor *bone_anchors;       /* 0x120 32 of them */
    Vector4 *bone_positions;        /* 0x124 32 of them, written from bone_anchors */
    struct ModelInstance *draw_next[2]; /* 0x128 next model in the same depth bucket, per draw list */
    ModelPart *unk130;              /* 0x130 the part whose placement Entity_RebuildModelMatrices leaves out (the root?) */
    char pad134[0x08];
    f32 radius;                     /* 0x13C how far the model reaches around its position (a map object's MapObject.unk14) */
    u8 hit_handles[4];              /* 0x140 handles of the linked points (D_003E7500) that touch it; the drawing code lights it by them */
    u8 hit_count;                   /* 0x144 how many of hit_handles are used */
    u8 unk145;                      /* 0x145 */
    s16 unk146;                     /* 0x146 */
    s32 unk148;                     /* 0x148 */
    void *unk14C;                   /* 0x14C */
    f32 unk150;                     /* 0x150 */
    f32 unk154;                     /* 0x154 */
    f32 unk158;                     /* 0x158 */
    f32 alpha;                      /* 0x15C below 128: drawn without z writes */
    s32 unk160[4];                  /* 0x160 */
    char pad170[0x08];
    void *unk178;                   /* 0x178 passed to func_0010B2E0 with unk194: bit n set skips chunk n (a mask, but not stored as an int) */
    int found_record_id;            /* 0x17C the id Effect_FindModelRecord last found; sent after the colour at unk150 by func_0010D308, in its alpha slot */
    int found_record[4];            /* 0x180 a copy of that record, its id bits replaced; a quadword sent after it */
    u16 unk190;                     /* 0x190 slot of unk194 in D_00468190 (0..2), 3: always sent */
    s16 unk192;                     /* 0x192 */
    int unk194;                     /* 0x194 */
    int unk198;                     /* 0x198 */
    int unk19C;                     /* 0x19C */
    int unk1A0;                     /* 0x1A0 */
    struct ModelWaveUv *wave_uvs;   /* 0x1A4 one per vertex, built by func_00111E28 */
    void (*unk1A8)(struct ModelInstance *model);    /* 0x1A8 */
    void (*draw)(struct ModelInstance *model);  /* 0x1AC sets the GS state and draws */
    void (*unk1B0)();               /* 0x1B0 called with the model by func_0010B140 */
    void (*unk1B4)(struct ModelInstance *model);    /* 0x1B4 */
    int (*get_point)(void *model, int index);   /* 0x1B8 */
    void (*unk1BC)(struct ModelInstance *model);    /* 0x1BC called when func_0010AEB8 sent new textures */
} ModelInstance;

/* a 0x40-byte vertex of four vectors; func_00110D60 fills the steps between two of them */
typedef struct {
    f32 unk00[4];                   /* 0x00 w is 1 */
    f32 unk10[4];                   /* 0x10 w is 0 */
    f32 unk20[4];                   /* 0x20 two values, then 1 and 0 */
    f32 unk30[4];                   /* 0x30 */
} BlendVertex;

/* one of the 0x40-byte lights at D_003E7500, as the drawing code copies it into a packet */
typedef struct {
    f32 unk00[4];                   /* 0x00 sent whole, but for the last word */
    f32 unk10[4];                   /* 0x10 sent whole */
    char pad20[0x04];
    f32 unk24;                      /* 0x24 sent in place of unk00[3] */
    char pad28[0x18];
} __attribute__((aligned(16))) ModelLight;

/* where a vertex lands on the screen, as Vu0_ProjectToScreen and Vu0_ProjectToScreen3 write it */
typedef struct {
    s32 x;                          /* 0x00 12.4 fixed point; the screen is 0x6C00 to 0x9400 across */
    s32 y;                          /* 0x04 0x7200 to 0x8E00 down */
    u32 z;                          /* 0x08 */
    s32 unk0C;                      /* 0x0C */
} ModelScreenPoint;

/* a 4x4 matrix (ModelInstance.unk40, ModelPart.unk00) as retail copies it whole: by doublewords */
typedef struct {
    f32 rows[4][4];
} __attribute__((aligned(16))) ModelMatrix;

/* the same matrix where retail copies it by quadwords */
typedef struct {
    Uint128 rows[4];
} ModelMatrixQwords;

/* ModelInstance.unk40 as the 4x4 matrix it is */
#define MODEL_MATRIX(model) ((f32 (*)[4])(model)->unk40)

/* ModelPart.unk00 as the 4x4 matrix it is */
#define PART_MATRIX(part) ((f32 (*)[4])(part)->unk00)

/* the scratchpad as func_00112118 uses it: a matrix per part, then one point and where it lands */
typedef struct {
    f32 matrices[0x40][16];         /* 0x00 */
    f32 point[4];                   /* 0x1000 */
    s32 screen[4];                  /* 0x1010 */
} ModelProjectScratch;

#define MODEL_PROJECT_SCRATCH ((ModelProjectScratch *)0x70000000)

/* the scratchpad as a matrix, where the builders of kf_model_matrix.c work */
#define MODEL_MATRIX_SCRATCH ((f32 *)0x70000000)

/* the scratchpad as the motion code (kf_motion.c) uses it: the key vectors it blends between */
#define MOTION_SCRATCH ((Vector4 *)0x70000000)

/* what MapObject.unk08 points at */
typedef struct {
    Vector4 position;               /* 0x00 where the object rests */
    Vector4 rotation;               /* 0x10 */
    Vector4 scale;                  /* 0x20 */
    u16 unk30;                      /* 0x30 its row of map_object_defs (param/obj.prm) */
    u16 unk32;                      /* 0x32 which of the map's object models it uses (func_00114AA8) */
    u16 unk34;                      /* 0x34 which of the map's object collision meshes */
    u16 unk36;                      /* 0x36 which of the map's object motions */
    union {
        u64 flags;
        u8 bytes[8];                /* bytes[2] 0x3A, bytes[3] 0x3B: the id of another map object */
    } unk38;                        /* 0x38 */
    u16 unk40;                      /* 0x40 passed to Script_Start, 0: nothing */
    u16 unk42;                      /* 0x42 the same, by MapObject_Break */
    s16 unk44;                      /* 0x44 an item (item_counts) or, 0x40 to 0x7F, a sound */
    u8 unk46;                       /* 0x46 */
    char pad47[0x01];
    u8 unk48;                       /* 0x48 */
    char pad49[0x01];
    s16 unk4A;                      /* 0x4A */
    char pad4C[0x06];
    u16 unk52;                      /* 0x52 1 to 6: touching it starts PlayerState.timed_flags[unk52 - 1] (func_001BF350) */
    u16 unk54;                      /* 0x54 passed to Effect_StartSpawner when the object comes into view, 0: nothing */
    u8 unk56;                       /* 0x56 the model point it starts at; the next one gives its direction */
    char pad57[0x01];
    u16 unk58;                      /* 0x58 passed to Script_Start when an item is used on the object, 0: nothing */
    u8 unk5A;                       /* 0x5A 0, 0xFF: in view with its map record; else by func_00116C08 */
    char pad5B[0x25];
} MapObjectUnk08;

/* MapObjectUnk08 with its four parameters as words; what they mean depends on the kind */
typedef struct {
    char pad00[0x44];
    s16 unk44;                      /* 0x44 */
    s16 unk46;                      /* 0x46 */
    s16 unk48;                      /* 0x48 */
    s16 unk4A;                      /* 0x4A */
    s16 unk4C;                      /* 0x4C */
    s16 unk4E;                      /* 0x4E */
    s16 unk50;                      /* 0x50 */
    s16 unk52;                      /* 0x52 */
} MapObjectUnk08Words;

#define MAP_OBJECT_WORDS(object) ((MapObjectUnk08Words *)(object)->unk08)

/* the same parameters as an array */
typedef struct {
    char pad00[0x44];
    s16 words[8];                   /* 0x44 */
} MapObjectUnk08Array;

#define MAP_OBJECT_WORD_ARRAY(object) (((MapObjectUnk08Array *)(object)->unk08)->words)

/* one animation of a map object's definition */
typedef struct {
    u8 anim;                        /* 0x00 passed to func_00108A08 */
    u8 sound_kind;                  /* 0x01 0: Sound_PlayCommonAt, 1: Sound_PlayMapAtRange, 2: Sound_StartTaggedAt */
    u16 unk02;                      /* 0x02 passed to Effect_StartSpawner on frame unk06 (MapObject_UpdateAll) */
    u16 sound;                      /* 0x04 0xFFFF: none */
    u8 unk06;                       /* 0x06 the animation frame unk02 starts on */
    u8 sound_frame;                 /* 0x07 the animation frame the sound starts on */
} MapObjectAnim;

/* what MapObject.unk04 points at: its row of param/obj.prm (map_object_defs) */
typedef struct {
    u32 kind : 8;                   /* 0x00 1, 2, 6, 0x11: EntitySight_ObjectInWay tests lines of sight against it */
    u32 bit8 : 1;                   /* which function draws it (MapObject_InitMap) */
    u32 pad00_9 : 2;
    u32 bit11 : 1;                  /* MapObject_Break can break it */
    u32 bit12 : 1;                  /* it has a breaking animation (1) */
    u32 pad00_13 : 2;
    u32 bit15 : 1;                  /* the player standing on it is carried and turned with it (Player_Move) */
    u32 pad00_16 : 16;
    u16 unk04;                      /* 0x04 0xFFFF: it has no model */
    u16 unk06;                      /* 0x06 0xFFFF: it has no collision mesh */
    char pad08[0x04];
    f32 unk0C;                      /* 0x0C above 0: MapObject.unk14 is this times the scale, plus 100 */
    MapObjectAnim anims[8];         /* 0x10 by MapObjectBytes.anim; entry anim + 2 holds a second sound for anims 4 and 5 */
    u16 unk50;                      /* 0x50 passed to Effect_StartSpawner */
    s16 unk52[9];                   /* 0x52 percent of each of a hit's nine strengths that it shrugs off, 200: all of it (func_001B3AC0) */
    f32 unk64;                      /* 0x64 above 0: the player must face it within this angle to act on it (func_001D0970) */
    u32 unk68;                      /* 0x68 flags; the low half is also the message id of its name ("Corpse", "Jail Door"...), shown when used */
    f32 unk6C;                      /* 0x6C MapObject.unk3C is this times the scale, plus 100 */
    f32 unk70;                      /* 0x70 how far its sounds carry, 0: 800 */
    u8 unk74;                       /* 0x74 */
} MapObjectUnk04;

/* MapObjectUnk04's first word as two halves */
typedef struct {
    u16 flags;                      /* 0x00 */
    u16 toughness;                  /* 0x02 the damage one hit must do to break the object (func_001B3AC0) */
} MapObjectDefHalves;

#define MAP_OBJECT_DEF_TOUGHNESS(def) (((MapObjectDefHalves *)(def))->toughness)

/* MapObjectUnk04's first word whole, where retail tests a bit of it with a mask (Player_Move) */
#define MAP_OBJECT_DEF_FLAGS(def) (*(u32 *)(def))
#define MAP_OBJECT_DEF_CARRIES 0x8000   /* bit15 */

/* MapObjectUnk04.kind read as the byte it is, where retail compares it that way (func_001C65F0) */
#define MAP_OBJECT_DEF_KIND(def) (*(u8 *)(def))

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

/* the objects of a map, looked up by MapObject_GetById(map, id) */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    MapObjectUnk04 *unk04;          /* 0x04 */
    MapObjectUnk08 *unk08;          /* 0x08 */
    s32 unk0C;                      /* 0x0C what func_00115CD0 returns for its position */
    u32 unk10;                      /* 0x10 flags, MAP_OBJECT_BITS */
    f32 unk14;                      /* 0x14 how far around its position it reaches, 0: nothing to hit */
    f32 unk18;                      /* 0x18 how far it is from the point last tested against the map objects (func_001C2758, func_001C2A38) */
    void *unk1C;                    /* 0x1C the next MapObject of its draw list (MapObject_InitMap) */
    f32 unk20;                      /* 0x20 the model's position (x, y, z) as of the last update */
    f32 unk24;                      /* 0x24 */
    f32 unk28;                      /* 0x28 */
    u8 sound;                       /* 0x2C the looping sound it plays, 0: none */
    char pad2D[0x03];
    f32 unk30;                      /* 0x30 the model's rotation (x, y, z) as of the last update */
    f32 unk34;                      /* 0x34 */
    f32 unk38;                      /* 0x38 */
    f32 unk3C;                      /* 0x3C MapObjectUnk04.unk6C times the scale, plus 100 */
    s32 work[8];                    /* 0x40 the kind's own state (MapTrigger.shot) */
    u32 unk60;                      /* 0x60 flags */
    char pad64[0x0C];
} MapObject;

/* the first word of the definition, whole (bit 11 0x800, bit 12 0x1000) */
#define MAP_OBJECT_DEF_WORD(object) (*(u32 *)(object)->unk04)

/* MapObject.work as most kinds use it */
typedef struct {
    Vector4 unk00;                  /* 0x00 */
    f32 unk10;                      /* 0x10 */
    f32 unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 */
    s32 timer;                      /* 0x1C counts down to 0 */
} MapObjectWork;

#define MAP_OBJECT_WORK(object) ((MapObjectWork *)(object)->work)

/* MapObject.work of the kind that fades with the distance to the player (func_001201E0) */
typedef struct {
    f32 near_distance;              /* 0x00 */
    f32 far_distance;               /* 0x04 */
    f32 near_alpha;                 /* 0x08 */
    f32 far_alpha;                  /* 0x0C */
} MapObjectWorkFade;

/* MapObject.work of the kinds that slide one part of their model (func_001211B0) */
typedef struct {
    Vector4 step;                   /* 0x00 added to the part each frame */
    s32 frames;                     /* 0x10 how many frames are left */
    s32 part;                       /* 0x14 which part */
    s32 fired;                      /* 0x18 MapTriggerShot.fired */
    s32 timer;                      /* 0x1C counts down to 0 */
} MapObjectWorkSlide;

/* the same for the kind of func_00121DA8, whose timer is a byte */
typedef struct {
    Vector4 step;                   /* 0x00 */
    s32 frames;                     /* 0x10 */
    s32 part;                       /* 0x14 */
    s32 fired;                      /* 0x18 */
    s8 timer;                       /* 0x1C */
} MapObjectWorkKind121DA8;

/* MapObject.unk60 as bit-fields */
typedef struct {
    u32 bit0 : 1;                   /* set while a door-like object is open or opening */
    u32 bits1 : 31;
} MapObjectUnk60Bits;

#define MAP_OBJECT_UNK60_BITS(object) ((MapObjectUnk60Bits *)&(object)->unk60)

/* the word at MapObject+0x10 (and MapTrigger+0x10) as bit-fields */
typedef struct {
    u32 map : 1;                    /* which of the two loaded maps it belongs to */
    u32 bit1 : 1;
    u32 bit2 : 1;
    u32 bits3 : 5;
    u32 step : 8;                   /* 0x11 step of the kind's state machine, 0: not started */
    u32 bit16 : 1;                  /* in use */
    u32 bit17 : 1;
    u32 bit18 : 1;
    u32 bit19 : 1;
    u32 bit20 : 1;                  /* a script drives it (MapObject_SetScripted) */
    u32 bit21 : 1;
    u32 bit22 : 1;
    u32 bit23 : 1;
    u32 anim : 8;                   /* 0x13 the animation MapObject_StartAnim started */
} MapObjectBits;

#define MAP_OBJECT_BITS(object) ((MapObjectBits *)&(object)->unk10)

/* the same word as bytes */
typedef struct {
    u8 unk00;                       /* 0x10 */
    u8 step;                        /* 0x11 */
    u8 unk02;                       /* 0x12 */
    s8 anim;                        /* 0x13 */
} MapObjectBytes;

#define MAP_OBJECT_BYTES(object) ((MapObjectBytes *)&(object)->unk10)

/* the same word through a union, for where retail reads it after stores of other types */
typedef union {
    u32 word;
    MapObjectBits bits;
} MapObjectFlags;

#define MAP_OBJECT_FLAGS(object) (((MapObjectFlags *)&(object)->unk10)->word)

/* the same word with bit 3 on its own: MapObject_UpdateAll clears it every frame, the kinds' handlers set it */
typedef struct {
    u32 map : 1;
    u32 bit1 : 1;                   /* in view this frame */
    u32 bit2 : 1;                   /* in view the frame before */
    u32 bit3 : 1;
    u32 bits4 : 28;
} MapObjectBitsLow;

#define MAP_OBJECT_BITS_LOW(object) ((MapObjectBitsLow *)&(object)->unk10)

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
    s32 unk00;                      /* 0x00 */
    s32 unk04;                      /* 0x04 cleared when the node is unlinked */
    s32 unk08;                      /* 0x08 cleared when the node is unlinked */
    s32 unk0C;                      /* 0x0C bit 0: the map slot */
    char pad10[0x04];
    void *unk14;                    /* 0x14 the MapRecordInfo (kf_map.h) it is standing in, found every frame */
    ModelInstance *model;           /* 0x18 non-null while the node is in use */
    s16 item;                       /* 0x1C which item it is (index into item_name_defs) */
    s16 unk1E;                      /* 0x1E index into the map's row of D_00356928 */
    struct ModelNode *prev;         /* 0x20 */
    struct ModelNode *next;         /* 0x24 */
    char pad28[0x14];
} ModelNode;

/* one of a map's fixed items: where it lies and what it is (0x40 bytes, D_004469B8[map]) */
typedef struct {
    Vector4 position;               /* 0x00 */
    Vector4 rotation;               /* 0x10 */
    Vector4 scale;                  /* 0x20 */
    u16 item;                       /* 0x30 which LevelDef (item_name_defs) it is */
    char pad32[0x02];
    u16 unk34;                      /* 0x34 passed to Script_Start in place of picking the item up, 0: nothing (func_001D0970) */
    u8 required_flag;               /* 0x36 nonzero: it only appears while its byte of D_00356928 is below this */
    u8 chance;                      /* 0x37 percent chance that it appears (func_001C87D8) */
    u8 group;                       /* 0x38 0-15: picks the roll and the map's threshold it is tested against */
    char pad39[0x06];
    u8 slot_id;                     /* 0x3F its slot in D_005B5EC0 */
} MapItemDef;

/* an item lying where it was dropped, as it is saved (0x14 bytes, 16 per map at D_00358928) */
typedef struct {
    s16 position[3];                /* 0x00 */
    s8 rotation[3];                 /* 0x06 in 1/16 radian */
    s8 scale[3];                    /* 0x09 in 1/16 */
    u16 item;                       /* 0x0C */
    u8 used;                        /* 0x0E */
    u8 priority;                    /* 0x0F ItemNodeBits.priority */
    s32 order;                      /* 0x10 D_004469D0 when it was dropped: the oldest is replaced first */
} DroppedItemRecord;

/* a map's 16 dropped items (a row of D_00358928), for copying them all at once */
typedef struct {
    DroppedItemRecord records[16];  /* 0x00 */
} DroppedItemRecords;

/* a ModelNode as the item code reads it: 64 fixed items per map at D_0057E240, then 16
   dropped ones per map at D_00580040 */
typedef struct ItemNode {
    MapItemDef *def;                /* 0x00 NULL on a dropped item */
    DroppedItemRecord *record;      /* 0x04 NULL on a fixed item */
    s32 unk08;                      /* 0x08 */
    u32 flags;                      /* 0x0C ItemNodeBits */
    char pad10[0x04];
    void *map_record;               /* 0x14 the MapRecordInfo (kf_map.h) it lies in, looked up before it is drawn */
    ModelInstance *model;           /* 0x18 */
    u16 item;                       /* 0x1C */
    s16 index;                      /* 0x1E */
    struct ItemNode *prev;          /* 0x20 */
    struct ItemNode *next;          /* 0x24 */
    f32 unk28;                      /* 0x28 a height */
    f32 unk2C;                      /* 0x2C a height */
    f32 unk30;                      /* 0x30 0 when the item is laid down */
    char pad34[0x08];
} ItemNode;

/* ItemNode.flags with the map slot as a bit-field */
typedef struct {
    u32 map : 1;                    /* which of the two loaded maps it lies in */
    u32 bits1 : 31;
} ItemNodeMapBit;

#define ITEM_NODE_MAP_BIT(node) ((ItemNodeMapBit *)&(node)->flags)

/* ItemNode with all of its flags as bit-fields (func_001C8F18) */
typedef struct {
    char pad00[0x0C];
    u32 map : 1;                    /* 0x0C which of the two loaded maps it lies in */
    u32 bit1 : 1;
    u32 bit2 : 1;                   /* set on a dropped item */
    u32 priority : 4;               /* a dropped item is only replaced by one whose priority is not lower */
    u32 bits7 : 25;
} ItemNodeBits;

#define ITEM_NODE_BITS(node) ((ItemNodeBits *)(node))

/* D_0057E040[map][32]: what is loaded for each kind of item a map holds (func_001CA498 finds one) */
typedef struct {
    char *model_data;               /* 0x00 handed to func_0010D018 */
    char *texture;                  /* 0x04 becomes the model's unk194 */
} ItemModelSource;

/* the object func_001C5170 tests against a model's points */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    char pad04[0x0C];
    int flags;                      /* 0x10 */
} CollisionOwner;

/* ChannelOwner.channel: the playback state Motion_Start resets */
typedef struct {
    ModelPartLevels *tables[0x20];  /* 0x00 MotionData.parts of each motion loaded */
    u8 limit;                       /* 0x80 entries at or above this index are replaced by 0 */
    u8 index;                       /* 0x81 */
    s16 param;                      /* 0x82 */
    s32 param_shifted;              /* 0x84 param << 8 */
    u32 value;                      /* 0x88 */
    u16 word;                       /* 0x8C */
    u8 unk8E;                       /* 0x8E */
    u8 unk8F;                       /* 0x8F nonzero: a blend moves 255/unk8F of the way each frame (func_00109178) */
} ModelChannel;

/* ModelInstance.level_tables points at the model's ModelChannel, whose first 0x80 bytes are the tables */
#define MODEL_CHANNEL(model) (*(ModelChannel **)&(model)->level_tables)

/* ModelInstance.unk100 as bit-fields */
typedef struct {
    u16 has_motion : 1;             /* set once a motion is loaded (func_001086E8); func_00108AC8 animates only then */
    u16 bits1 : 15;
} ModelUnk100Bits;

#define MODEL_UNK100_BITS(model) ((ModelUnk100Bits *)&(model)->unk100)

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
