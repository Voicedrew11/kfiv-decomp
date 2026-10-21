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
    u64 low_doubleword; /* low 64 bits */
    u64 high_doubleword; /* high 64 bits */
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
    void *position;                 /* 0x00 vertex position passed to the triangle tests */
    s32 x;                          /* 0x04 x, as Collision_GetFloorMaterial reads it */
    s32 y;                          /* 0x08 y, as Collision_GetFloorMaterial reads it */
    s32 z;                          /* 0x0C z, as Collision_GetFloorMaterial reads it */
} StripVertex;

/* a strip: a header, then vertex_count vertices */
typedef struct {
    u16 vertex_count;               /* 0x00 */
    char pad02[0x1E];
    StripVertex vertices[1];        /* 0x20 vertex_count of them */
} Strip;

/* ModelInstance.strips: the strips back to back */
typedef struct {
    s32 strip_count;                /* 0x00 */
    Strip *strips;                  /* 0x04 */
    s32 pad08;                      /* 0x08 */
    u32 total_vertex_count;         /* 0x0C gate: collision needs at least 4 */
} ModelStripList;

/* one 0x80-byte part (joint) of a model, ModelInstance.parts[i] */
typedef struct ModelPart {
    char matrix[0x40];              /* 0x00 the part's matrix, passed by address */
    f32 position[4];                /* 0x40 where it sits on its parent */
    f32 rotation_x;                 /* 0x50 an angle, wrapped to +-pi (about x) */
    f32 rotation_y;                 /* 0x54 angle about y */
    f32 rotation_z;                 /* 0x58 angle about z */
    f32 rotation_w;                 /* 0x5C 1.0 */
    f32 scale[3];                   /* 0x60 the third vector of a motion key (scale, it seems) */
    u8 motion_slot;                 /* 0x6C the part's motion slot (Motion_SetPartSlots, Motion_SetPartSlot) */
    char pad6D[0x03];
    struct ModelPart *child;        /* 0x70 first part hanging off this one, NULL: none */
    struct ModelPart *sibling;      /* 0x74 next part on the same parent, NULL: none */
    u64 data_addr : 32;             /* 0x78 */
    u64 part_flags : 7;             /* ModelPartLevels.part_flags of the motion last loaded */
    u64 level : 7;                  /* detail level picked by Motion_SeekKeys */
    u64 parent_index : 7;  /* parent part index from the model file */
    u64 data_count : 11;
} ModelPart;

/* one vertex of a ModelPartData block */
typedef struct {
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    char pad06[0x08];
    u16 flags;                      /* 0x0E flag bits Wobble_DrawRippledModel passes on to the GS vertex */
    u16 tex_u;                      /* 0x10 texture coordinates */
    u16 tex_v;                      /* 0x12 */
    u16 uvs_set_from_screen;        /* 0x14 flag: uvs were computed from the screen */
    char pad16[0x02];
    s16 colour[4];                  /* 0x18 red, green, blue, alpha: 0..0xFF each (set by Model_SetVertexColour) */
} ModelPartVertex;

/* a ModelPartVertex with every field a signed word, as the code that builds vertices between
   two others reads it (ModelScreen_StepVertices, ModelScreen_StepVerticesRounded) */
typedef struct {
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    s16 built_between;              /* 0x06 1 on a vertex built between two others */
    s16 normal[3];                  /* 0x08 normal, x y z */
    u16 cut_flag;                   /* 0x0E 0; 0x8000 on the first two vertices of a strip (ModelScreen_QueueCutTriangle) */
    s16 tex_u;                      /* 0x10 */
    s16 tex_v;                      /* 0x12 */
    s16 unit_weight;                /* 0x14 4.12 weight, always 1.0 here */
    s16 blend_fraction;             /* 0x16 0x7FFF on vertices built between two others */
    s16 colour[4];                  /* 0x18 */
} ModelPartVertexWords;

/* a ModelPartData block from 0x70 on, for the models whose vertices start at 0x80 (ModelScreen_SetUvsFromScreen) */
typedef struct {
    s32 vertex_count;               /* 0x00 low 15 bits: how many vertices */
    char pad04[0x0C];
    ModelPartVertex vertices[1];    /* 0x10 */
} ModelPartDataTail;

/* the doubleword at ModelPartData+0x30 */
typedef struct {
    u64 texture_base : 14;          /* 0x00 14 bits: the texture base pointer when it is a TEX0 (Model_SetTextureBase) */
    u64 pad14 : 23;
    u64 clut_base : 14;             /* 0x37 14 bits: the CLUT base, not 0 on the blocks Model_SetUpSkinned looks for */
    u64 pad51 : 10;
    u64 clut_load_mode : 3;                  /* TEX0 CLUT load field (bits 61-63) */
} ModelPartDataBits30;

/* one block of a part's data (ModelPart.data_addr): 0x90 bytes of GS setup, then the vertices.
   The next block starts (length + 1) * 16 bytes after this one. */
typedef struct {
    u16 length;                     /* 0x00 in 16-byte units, not counting the first */
    char pad02[0x2E];
    ModelPartDataBits30 texture_word; /* 0x30 the doubleword: texture base, CLUT base and skinned mark */
    char pad38[0x08];
    u64 tex0;                       /* 0x40 low 14 bits: the texture it draws with */
    char pad48[0x08];
    u64 blend;                      /* 0x50 set by Model_SetAlphaBlend */
    char pad58[0x18];
    s32 vertex_count;               /* 0x70 low 15 bits: how many vertices */
    s32 gif_tag_high;               /* 0x74 high word of the block's GIF tag: the primitive blend bit */
    char pad78[0x18];
    ModelPartVertex vertices[1];    /* 0x90 */
} ModelPartData;

/* the data block `qwords` quadwords on from `data` */
#define MODEL_PART_DATA_SKIP(data, qwords) ((ModelPartData *)((char *)(data) + (qwords) * 16))

#define MODEL_RECORD_ID_MASK 0x3FFF          /* ModelPartRecord.id_word: the record id, low 14 bits */
#define MODEL_RECORD_PAYLOAD_MASK 0x7FFF     /* ModelPartRecord.payload_size: low 15 bits */

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
    u64 bit_fields;                 /* 0x78 low word: data pointer; bits 39-45 level, 46-52 parent, 53+ count */
} ModelPartWords;

#define MODEL_PART_WORD(part) (((ModelPartWords *)(part))->bit_fields)

/* the same doubleword through a union, for where retail reads it again after stores of other
   types (Model_DrawSkinned) */
typedef union {
    u64 word;
    u32 halves[2];
} ModelPartWordUnion;

#define MODEL_PART_WORD_ANY(part) (((ModelPartWordUnion *)&MODEL_PART_WORD(part))->word)

#define MODEL_PART_PARENT_MASK 0x7F     /* ModelPart.parent_index: 7 bits */
#define MODEL_PART_LEVEL_MASK 0x7F      /* ModelPart.level: 7 bits */

/* the part this one hangs off (ModelPart.parent_index) */
#define MODEL_PART_PARENT(part) ((s32)(MODEL_PART_WORD(part) >> 46) & MODEL_PART_PARENT_MASK)
/* the part's detail level (ModelPart.level) */
#define MODEL_PART_LEVEL(part) ((u32)(MODEL_PART_WORD(part) >> 39) & MODEL_PART_LEVEL_MASK)

/* the part this one hangs off, read through the union */
#define MODEL_PART_PARENT_ANY(part) ((s32)(MODEL_PART_WORD_ANY(part) >> 46) & MODEL_PART_PARENT_MASK)

/* one 0x40-byte vertex of a ModelWideData block */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 offset[4];                  /* 0x10 added to the position before it is projected (Model_SetUvsFromScreen) */
    f32 tex_s;                      /* 0x20 texture coordinates, 0 to 1 across the screen */
    f32 tex_t;                      /* 0x24 */
    char pad28[0x04];
    s32 on_parent;                  /* 0x2C 0: placed by its own part's matrix, otherwise by the parent part's */
    char pad30[0x10];
} ModelWideVertex;

/* a block of a part's data in the models drawn with their own texture (MODEL_DRAW_OWN_TEXTURE):
   0xB0 bytes of GS setup, then 0x40-byte vertices */
typedef struct {
    u16 length;                     /* 0x00 in 16-byte units, not counting the first */
    char pad02[0x8E];
    s32 vertex_count;               /* 0x90 low 15 bits: how many vertices */
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
    f32 position[3];                /* 0x00 where the part rests: ModelPart.position */
    f32 rotation[3];                /* 0x0C ModelPart.rotation_x, unk54, unk58 */
    f32 scale[3];                   /* 0x18 ModelPart.scale */
    s32 data_offset;                /* 0x24 from the start of the file to the part's ModelPartData blocks */
    u16 data_count;                 /* 0x28 how many blocks */
    char pad2A[0x06];
} PosedModelFilePart;

/* a model file as Model_SetUpPosed reads it: every part comes with the pose it rests in */
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
    u64 bit40 : 1;                  /* 0x6D bit 0, set: Model_DrawPosedWithAlpha does not draw the part */
    u64 bit41 : 1;                  /* 0x6D bit 1, set by Model_SetUpSkinned */
    u64 pad68_42 : 22;
} ModelPartFlagBits;

#define MODEL_PART_FLAG_BITS(part) ((ModelPartFlagBits *)(part))

/* the same doubleword whole, for the code that tests it with masks */
typedef struct {
    char pad00[0x68];
    u64 flag_word;                  /* 0x68 the flag bits, MODEL_PART_BIT* */
} ModelPartFlagWord;

#define MODEL_PART_FLAG_WORD(part) (((ModelPartFlagWord *)(part))->flag_word)
#define MODEL_PART_BIT40 (0x8000L << 25)
#define MODEL_PART_BIT41 (0x8000L << 26)

/* per-part level thresholds, ModelInstance.level_tables[n][part] */
typedef struct {
    u8 count;                       /* 0x00 */
    char pad01;
    u8 part_flags;                  /* 0x02 low 7 bits go to ModelPart.part_flags */
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

/* one motion of a MotionFile; ModelMotion_Load hooks it up to a model */
typedef struct {
    u32 size;                       /* 0x00 the next motion starts this many bytes on */
    u32 vectors_offset;             /* 0x04 */
    MotionVector *vectors;          /* 0x08 set from vectors_offset */
    u16 frame_count;                /* 0x0C ModelTableHeader.frame_count */
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
    u16 frame_count;                /* 0x00 the motion's frame count, MotionData.frame_count (Motion_FrameCount) */
    char pad02[0x02];
} ModelTableHeader;

/* one part of a ModelFile */
typedef struct {
    s32 data_offset;                /* 0x00 from the start of the file to the part's ModelPartData blocks */
    u16 data_count;                 /* 0x04 how many blocks */
    u8 slot;                        /* 0x06 1..3: the part goes to ModelInstanceSlots slot 1 to 3 */
    s8 parent;                      /* 0x07 the part this one hangs off (ModelPart.pad78_46) */
    char pad08[0x08];
} ModelFilePart;

/* a model file as Model_SetUpSkinned reads it */
typedef struct ModelFile {
    s32 size;                       /* 0x00 the next file starts this many bytes on */
    u16 part_count;                 /* 0x04 */
    char pad06[0x0A];
    BoneAnchor bone_anchors[32];    /* 0x10 */
    ModelFilePart parts[1];         /* 0x210 part_count of them */
} ModelFile;

/* a model file's header through a union of its two layouts: a read through it aliases everything
   (Model_SetUpPosed tests the part count that way, after it has moved the memory cursor) */
typedef union {
    ModelFile plain;
    PosedModelFile posed;
} ModelFileUnion;

#define POSED_MODEL_FILE_ANY(file) (((ModelFileUnion *)(file))->posed)

#define MODEL_FLAG_FLAT_COLOUR 0x400    /* ModelInstance.model_flags: no directional light, lit flat by its own colour times its alpha */
#define MODEL_DRAW_FACE_CAMERA_X 0x100  /* ModelInstance.draw_flags: turned about x to face the camera */
#define MODEL_DRAW_FACE_CAMERA_Y 0x200  /* ModelInstance.draw_flags: turned about y to face the camera */
#define MODEL_DRAW_FACE_CAMERA   0x300  /* either of the two above */
#define MODEL_DRAW_OWN_TEXTURE   0x800  /* ModelInstance.draw_flags: drawn with its own texture, the frame copied into it first */
#define MODEL_DRAW_TURN_Z_ONLY   0x400  /* ModelInstance.draw_flags: turned about z alone */

/* ModelInstance with the three part slots at 0x130 as the array Model_SetUpSkinned fills
   (ModelInstance.root_part is the first) */
typedef struct {
    char pad00[0x130];
    ModelPart *parts[3];            /* 0x130 the parts whose ModelFilePart.slot is 1, 2 and 3 */
} ModelInstanceSlots;

#define MODEL_SLOT_PARTS(model) (((ModelInstanceSlots *)(model))->parts)

/* a vertex's own texture coordinates and the wave (gfx/screen.h WaveOscillator) that moves them */
typedef struct ModelWaveUv {
    u16 tex_u;                      /* 0x00 */
    u16 tex_v;                      /* 0x02 */
    s32 wave;                       /* 0x04 0-15 */
} ModelWaveUv;

typedef struct ModelInstance {
    char pad00[0x40];
    f32 model_matrix[16];           /* 0x40 a matrix, passed by address */
    char collision_matrix[0x40];    /* 0x80 a copy of model_matrix */
    Vector4 position;               /* 0xC0 */
    Vector4 rotation;               /* 0xD0 x, y (heading), z in radians */
    f32 scale_x;                    /* 0xE0 its sign picks the turn direction; the x scale */
    f32 scale_y;                    /* 0xE4 the y scale */
    f32 scale_z;                    /* 0xE8 the z scale */
    f32 scale_w;                    /* 0xEC the fourth value of the scale vector, 1.0 */
    u16 part_count;                 /* 0xF0 */
    u16 file_part_count;            /* 0xF2 set to the part count too */
    char padF4[0x04];
    ModelPart *parts;               /* 0xF8 */
    ModelPartLevels **level_tables; /* 0xFC */
    u16 model_flags;                /* 0x100 bits: 1 motion on, 0x200 draw-list, 0x400 unlit, 0x800 record sent */
    u16 draw_flags;                 /* 0x102 MODEL_DRAW_OWN_TEXTURE: drawn with its own texture */
    int effect_record_hint;         /* 0x104 int passed to Effect_FindModelRecord, unread */
    char pad108[0x08];
    ModelStripList *strips;         /* 0x110 also read as a CollisionMesh (collision/collision.h), NULL: none */
    MotionFile *motion_file;        /* 0x114 */
    char pad118[0x04];
    ModelFile *file;                /* 0x11C */
    BoneAnchor *bone_anchors;       /* 0x120 32 of them */
    Vector4 *bone_positions;        /* 0x124 32 of them, written from bone_anchors */
    struct ModelInstance *draw_next[2]; /* 0x128 next model in the same depth bucket, per draw list */
    ModelPart *root_part;           /* 0x130 the part whose placement Entity_RebuildModelMatrices leaves out (the root?) */
    char pad134[0x08];
    f32 radius;                     /* 0x13C how far the model reaches around its position (a map object's MapObject.reach_radius) */
    u8 hit_handles[4];              /* 0x140 handles of the linked points (linked_point_pool) that touch it; the drawing code lights it by them */
    u8 hit_count;                   /* 0x144 how many of hit_handles are used */
    u8 fade_enabled;                /* 0x145 1: drawn with fade_alpha instead of the parts' own alpha */
    s16 fade_alpha;                 /* 0x146 alpha (0..0x100) the model fades with while fade_enabled */
    s32 light_block;                /* 0x148 pointer to the model's lights: two 0x40 matrices */
    void *fog_block;                /* 0x14C pointer to the model's fog block (near, far, colour) */
    f32 colour_red;                 /* 0x150 the model's red tint */
    f32 colour_green;               /* 0x154 the model's green tint */
    f32 colour_blue;                /* 0x158 the model's blue tint */
    f32 alpha;                      /* 0x15C below 128: drawn without z writes */
    s32 draw_header_vector[4];      /* 0x160 four words sent in the draw packet */
    char pad170[0x08];
    void *chunk_skip_mask;          /* 0x178 passed to TextureTable_QueueEnabled with texture_data: bit n set skips chunk n (a mask, but not stored as an int) */
    int found_record_id;            /* 0x17C the id Effect_FindModelRecord last found; sent after the colour at unk150 by Model_DrawPosed, in its alpha slot */
    VuIntVector found_record;       /* 0x180 a copy of that record, its id bits replaced; a quadword sent after it
                                       (Effect_FindModelRecord clears it with one sq, so it is 16-byte aligned) */
    u16 texture_slot;               /* 0x190 slot of unk194 in texture_slots (0..2), 3: always sent */
    s16 shared_texture_slot;        /* 0x192 which entry of the shared last-sent texture record applies */
    int texture_data;               /* 0x194 the model's texture: a chunk table, or a raw packet */
    int shared_texture_packet;      /* 0x198 map texture packet 5, tracked in the shared record */
    int texture_packet_3;           /* 0x19C the map's texture packet 3, queued when it changes */
    int texture_packet_5;           /* 0x1A0 the map's texture packet 5, queued when it changes */
    struct ModelWaveUv *wave_uvs;   /* 0x1A4 one per vertex, built by Wobble_BuildWaveUvs */
    void (*draw_parts)(struct ModelInstance *model); /* 0x1A8 queues the model's part packets */
    void (*draw)(struct ModelInstance *model); /* 0x1AC sets the GS state and draws */
    void (*after_draw)(struct ModelInstance *model); /* 0x1B0 called with the model by DrawList_Draw */
    void (*update_bone_points)(struct ModelInstance *model); /* 0x1B4 recomputes the 32 bone positions from the bone anchors */
    f32 *(*get_point)(struct ModelInstance *model, int index); /* 0x1B8 point `index` as x, y, z, w */
    void (*on_textures_sent)(struct ModelInstance *model); /* 0x1BC called when ModelTexture_SendForModel sent new textures */
} ModelInstance;

/* a 0x40-byte vertex of four vectors; ModelScreen_StepFloatVertices fills the steps between two of them */
typedef struct {
    f32 position[4];                /* 0x00 w is 1 */
    f32 normal[4];                  /* 0x10 w is 0 */
    f32 tex_coords[4];              /* 0x20 s, t, then 1 and 0 */
    f32 colour[4];                  /* 0x30 r, g, b, a */
} BlendVertex;

/* one of the 0x40-byte lights at linked_point_pool, as the drawing code copies it into a packet */
typedef struct {
    f32 unk00[4];                   /* 0x00 sent whole, but for the last word */
    f32 unk10[4];                   /* 0x10 sent whole */
    char pad20[0x04];
    f32 unk24;                      /* 0x24 sent in place of unk00[3] */
    char pad28[0x18];
} __attribute__((aligned(16))) ModelLight;

#define LINKED_POINT_LIGHTS ((ModelLight *)linked_point_pool)

/* where a vertex lands on the screen, as Vu0_ProjectToScreen and Vu0_ProjectToScreen3 write it */
typedef struct {
    s32 x;                          /* 0x00 12.4 fixed point; the screen is 0x6C00 to 0x9400 across */
    s32 y;                          /* 0x04 0x7200 to 0x8E00 down */
    u32 z;                          /* 0x08 */
    s32 flags;                      /* 0x0C flag bits, 0x8000 on some points */
} ModelScreenPoint;

#define MODEL_SCREEN_POINT_FLAG_OFF_SCREEN 0x8000  /* the point is off the screen (held weapon trail) */

/* a 4x4 matrix (ModelInstance.model_matrix, ModelPart.matrix) as retail copies it whole: by doublewords */
typedef struct {
    f32 rows[4][4];
} __attribute__((aligned(16))) ModelMatrix;

/* the same matrix where retail copies it by quadwords */
typedef struct {
    Uint128 rows[4];
} ModelMatrixQwords;

/* ModelInstance.model_matrix as the 4x4 matrix it is */
#define MODEL_MATRIX(model) ((f32 (*)[4])(model)->model_matrix)

/* ModelPart.matrix as the 4x4 matrix it is */
#define PART_MATRIX(part) ((f32 (*)[4])(part)->matrix)

/* the scratchpad as Wobble_SetUvsFromJitteredScreen uses it: a matrix per part, then one point and where it lands */
typedef struct {
    f32 matrices[0x40][16];         /* 0x00 */
    f32 point[4];                   /* 0x1000 */
    s32 screen[4];                  /* 0x1010 */
} ModelProjectScratch;

#define MODEL_PROJECT_SCRATCH ((ModelProjectScratch *)0x70000000)

/* the scratchpad as a matrix, where the builders of model/model_matrix.c work */
#define MODEL_MATRIX_SCRATCH ((f32 *)0x70000000)

/* the scratchpad as the motion code (model/motion.c) uses it: the key vectors it blends between */
#define MOTION_SCRATCH ((Vector4 *)0x70000000)

/* the scratchpad as ModelMotion_Step uses it: the two keys and their two tangents of a Hermite step */
#define MOTION_SCRATCH_POINTS ((Vector4 *)0x70000000)

/* the scratchpad as Model_DrawSkinned and Model_DrawSkinnedTwoPass use it: the turn they work out, the two matrices
   ModelMatrix_BuildDrawMatrices makes for the model, then a matrix per part and the same without the
   model's own placement */
#define MODEL_TURN_SCRATCH ((f32 *)0x70000000)
#define MODEL_TURN_MATRIX_A ((void *)0x70000040)
#define MODEL_TURN_MATRIX_B ((void *)0x70000080)
#define MODEL_TURN_PART_MATRICES ((f32 (*)[16])0x700000C0)
#define MODEL_TURN_PART_TURNS ((f32 (*)[16])0x70001100)

/* the scratchpad as Model_DrawPosed, Model_DrawPosedDepthByPart and Model_DrawPosedWithAlpha use it: the two matrices
   ModelMatrix_BuildDrawMatrices makes for the model */
#define MODEL_DRAW_MATRIX_A ((void *)0x70000000)
#define MODEL_DRAW_MATRIX_B ((void *)0x70000040)

/* the scratchpad as ModelScreen_CutClippedTriangles uses it: where each vertex of a block lands on the screen */
#define MODEL_CLIP_SCREEN_POINTS ((ModelScreenPoint *)0x70000000)

/* the scratchpad as ModelScreen_QueueCutTriangle uses it: the edge opposite the apex, then two rows of the fan */
#define MODEL_CUT_EDGE ((ModelPartVertexWords *)0x70000200)
#define MODEL_CUT_ROW_A ((ModelPartVertexWords *)0x700002A0)
#define MODEL_CUT_ROW_B ((ModelPartVertexWords *)0x70000340)

/* the same for ModelScreen_QueueCutFloatTriangle, which cuts with float vertices (BlendVertex) */
#define MODEL_CUT_BLEND_EDGE ((BlendVertex *)0x70000200)
#define MODEL_CUT_BLEND_ROW_A ((BlendVertex *)0x70000340)
#define MODEL_CUT_BLEND_ROW_B ((BlendVertex *)0x70000480)

/* what MapObject.source points at */
typedef struct {
    Vector4 position;               /* 0x00 where the object rests */
    Vector4 rotation;               /* 0x10 */
    Vector4 scale;                  /* 0x20 */
    u16 def_index;                  /* 0x30 its row of map_object_defs (param/obj.prm) */
    u16 model_index;                /* 0x32 which of the map's object models it uses (Map_BuildModels) */
    u16 collision_mesh_index;       /* 0x34 which of the map's object collision meshes */
    u16 motion_index;               /* 0x36 which of the map's object motions */
    union {
        u64 flags;
        u8 bytes[8];                /* bytes[2] 0x3A, bytes[3] 0x3B: the id of another map object */
    } spawn_flags;                  /* 0x38 flags, the linked object's id and two block indices */
    u16 use_script;                 /* 0x40 passed to Script_Start, 0: nothing */
    u16 break_script;               /* 0x42 the same, by MapObject_Break */
    s16 kind_param_0;               /* 0x44 an item (item_counts) or, 0x40 to 0x7F, a sound */
    u8 kind_param_1;                /* 0x46 kind parameter 1: key item, sound mode */
    char pad47[0x01];
    u8 kind_param_2;                /* 0x48 kind parameter 2: sound id, gold to drop */
    char pad49[0x01];
    s16 kind_param_3;               /* 0x4A kind parameter 3: sound range, aim angle */
    char pad4C[0x06];
    u16 touch_status_flag;          /* 0x52 1 to 6: touching it starts PlayerState.timed_flags[unk52 - 1] (Player_CollideWithMapObjects) */
    u16 appear_effect;              /* 0x54 passed to Effect_StartSpawner when the object comes into view, 0: nothing */
    u8 effect_start_point;          /* 0x56 the model point it starts at; the next one gives its direction */
    char pad57[0x01];
    u16 item_use_script;            /* 0x58 passed to Script_Start when an item is used on the object, 0: nothing */
    u8 linked_point_owner;          /* 0x5A 0, 0xFF: in view with its map record; else by Map_TestFlagBlockBit */
    char pad5B[0x25];
} MapObjectUnk08;

/* MapObjectUnk08 with its four parameters as words; what they mean depends on the kind */
typedef struct {
    char pad00[0x44];
    s16 kind_param_0;               /* 0x44 kind parameter 0: fade distance, light red, item id */
    s16 kind_param_1;               /* 0x46 kind parameter 1: fade distance, light green, key item */
    s16 kind_param_2;               /* 0x48 kind parameter 2: fade alpha, light blue, gold dropped */
    s16 kind_param_3;               /* 0x4A kind parameter 3: fade alpha, light owner, aim angle */
    s16 kind_param_4;               /* 0x4C kind parameter 4: light flag, aim angle y */
    s16 kind_param_5;               /* 0x4E kind parameter 5: shot speed, light owner */
    s16 kind_param_6;               /* 0x50 kind parameter 6: light sort order */
    s16 kind_param_7;               /* 0x52 kind parameter 7: light model point, object flag */
} MapObjectUnk08Words;

#define MAP_OBJECT_WORDS(object) ((MapObjectUnk08Words *)(object)->source)

/* the same parameters as an array */
typedef struct {
    char pad00[0x44];
    s16 words[8];                   /* 0x44 */
} MapObjectUnk08Array;

#define MAP_OBJECT_WORD_ARRAY(object) (((MapObjectUnk08Array *)(object)->source)->words)

/* one animation of a map object's definition */
typedef struct {
    u8 anim;                        /* 0x00 passed to ModelMotion_Select */
    u8 sound_kind;                  /* 0x01 0: Sound_PlayCommonAt, 1: Sound_PlayMapAtRange, 2: Sound_StartTaggedAt */
    u16 spawner_effect_id;          /* 0x02 passed to Effect_StartSpawner on frame spawn_frame (MapObject_UpdateAll) */
    u16 sound;                      /* 0x04 0xFFFF: none */
    u8 spawn_frame;                 /* 0x06 the animation frame spawner_effect_id starts on */
    u8 sound_frame;                 /* 0x07 the animation frame the sound starts on */
} MapObjectAnim;

/* what MapObject.definition points at: its row of param/obj.prm (map_object_defs) */
typedef struct {
    u32 kind : 8;                   /* 0x00 1, 2, 6, 0x11: EntitySight_ObjectInWay tests lines of sight against it */
    u32 bit8 : 1;                   /* which function draws it (MapObject_InitMap) */
    u32 pad00_9 : 2;
    u32 bit11 : 1;                  /* MapObject_Break can break it */
    u32 bit12 : 1;                  /* it has a breaking animation (1) */
    u32 pad00_13 : 2;
    u32 bit15 : 1;                  /* the player standing on it is carried and turned with it (Player_Move) */
    u32 pad00_16 : 16;
    u16 model_index;                /* 0x04 0xFFFF: it has no model */
    u16 collision_mesh_index;       /* 0x06 0xFFFF: it has no collision mesh */
    char pad08[0x04];
    f32 collision_radius;           /* 0x0C above 0: MapObject.reach_radius is this times the scale, plus 100 */
    MapObjectAnim anims[8];         /* 0x10 by MapObjectBytes.anim; entry anim + 2 holds a second sound for anims 4 and 5 */
    u16 break_effect;               /* 0x50 passed to Effect_StartSpawner */
    s16 hit_resist_percent[9];      /* 0x52 percent of each of a hit's nine strengths that it shrugs off, 200: all of it (Attack_HitMapObject) */
    f32 facing_angle;               /* 0x64 above 0: the player must face it within this angle to act on it (Interact_FindTarget) */
    u32 definition_flags;           /* 0x68 flags; the low half is also the message id of its name ("Corpse", "Jail Door"...), shown when used */
    f32 interact_radius;            /* 0x6C MapObject.interact_radius is this times the scale, plus 100 */
    f32 sound_range;                /* 0x70 how far its sounds carry, 0: 800 */
    u8 sound_falloff_mode;          /* 0x74 which falloff curve its sounds use */
} MapObjectUnk04;

/* MapObjectUnk04's first word as two halves */
typedef struct {
    u16 flags;                      /* 0x00 */
    u16 toughness;                  /* 0x02 the damage one hit must do to break the object (Attack_HitMapObject) */
} MapObjectDefHalves;

#define MAP_OBJECT_DEF_TOUGHNESS(def) (((MapObjectDefHalves *)(def))->toughness)

/* MapObjectUnk04's first word whole, where retail tests a bit of it with a mask (Player_Move) */
#define MAP_OBJECT_DEF_FLAGS(def) (*(u32 *)(def))
#define MAP_OBJECT_DEF_CARRIES 0x8000   /* bit15 */
#define MAP_OBJECT_DEF_FADE_ON_BREAK 0x20000 /* definition_flags: fades out over its breaking animation (MapObject_UpdateBreaking) */

/* MapObjectUnk04.kind read as the byte it is, where retail compares it that way (Collision_PushOutOfMeshObj) */
#define MAP_OBJECT_DEF_KIND(def) (*(u8 *)(def))

/* AnimObject+0x40 */
typedef struct {
    char pad00[0x14];
    s32 pad14;                      /* 0x14 */
    s32 pad18;                      /* 0x18 */
} AnimObjectBody;

/* an animated object: the model, its animation table and a small state machine */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    void *anims;                    /* 0x04 8-byte entries */
    char pad08[0x08];
    u8 anim_flags;                  /* 0x10 flags word; bit 0x10000 enables the quad hit */
    u8 mode;                        /* 0x11 */
    char pad12[0x02];
    char pad14[0x2C];
    AnimObjectBody body;            /* 0x40 */
} AnimObject;

/* the 32-bit flags at AnimObject+0x10 */
#define ANIM_OBJECT_FLAGS(object) (*(u32 *)&(object)->anim_flags)

/* the same word of AnimObject and CollisionOwner: the object is hit as a quad of model points 0 to 3 */
#define OBJECT_FLAG_QUAD_HIT 0x10000

/* the objects of a map, looked up by MapObject_GetById(map, id) */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    MapObjectUnk04 *definition;     /* 0x04 its row of param/obj.prm: kind, model, scripts, toughness */
    MapObjectUnk08 *source;         /* 0x08 its record in the map's object list: where it stands, its model */
    s32 map_record;                 /* 0x0C what Map_FindRecordAtInSlot returns for its position */
    u32 state_flags;                /* 0x10 flags, MAP_OBJECT_BITS */
    f32 reach_radius;               /* 0x14 how far around its position it reaches, 0: nothing to hit */
    f32 tested_point_distance;      /* 0x18 how far it is from the point last tested against the map objects (Collision_PushOutOfObjects, Collision_StandOnObjects) */
    void *next_in_draw_list;        /* 0x1C the next MapObject of its draw list (MapObject_InitMap) */
    f32 last_position_x;            /* 0x20 the model's position (x, y, z) as of the last update */
    f32 last_position_y;            /* 0x24 the model's y as of the last update */
    f32 last_position_z;            /* 0x28 the model's z as of the last update */
    u8 sound;                       /* 0x2C the looping sound it plays, 0: none */
    char pad2D[0x03];
    f32 last_rotation_x;            /* 0x30 the model's rotation (x, y, z) as of the last update */
    f32 last_rotation_y;            /* 0x34 the model's y rotation as of the last update */
    f32 last_rotation_z;            /* 0x38 the model's z rotation as of the last update */
    f32 interact_radius;            /* 0x3C MapObjectUnk04.interact_radius times the scale, plus 100 */
    s32 work[8];                    /* 0x40 the kind's own state (MapTrigger.shot) */
    u32 door_open_flags;            /* 0x60 flags */
    char pad64[0x0C];
} MapObject;

/* the bits of MapObject.state_flags, the word MAP_OBJECT_BITS views */
#define MAP_OBJECT_IN_VIEW          0x2       /* bit 1: in view this frame (MapObject_DrawAll and Interact_FindTarget test it) */
#define MAP_OBJECT_IN_VIEW_PREVIOUS 0x4       /* bit 2: in view the frame before */
#define MAP_OBJECT_IN_VIEW_MASK     0x6       /* bits 1-2: in view this frame or the frame before (the appear and disappear effects) */
#define MAP_OBJECT_STEP_AND_ON_MASK 0x2FF00   /* bits 8-15 and 17: the kind's state machine step plus the on bit */
#define MAP_OBJECT_IN_USE           0x10000   /* bit 16: in use (MapObject_Activate sets it, MapObject_Deactivate clears it) */
#define MAP_OBJECT_ON               0x20000   /* bit 17: open, on or firing (doors, switches, chests, shooters and pitfalls) */
#define MAP_OBJECT_EMPTIED          0x40000   /* bit 18: it has given out the item or key it held (MapObject_MarkEmptied) */
#define MAP_OBJECT_SCRIPTED         0x100000  /* bit 20: a script drives it (MapObject_SetScripted) */
#define MAP_OBJECT_FLAG_400000      0x400000  /* bit 22: set from a script; the motion-done script commands treat it as never done */

/* the first word of the definition, whole */
#define MAP_OBJECT_DEF_WORD(object) (*(u32 *)(object)->definition)
#define MAP_OBJECT_DEF_SKINNED      0x100   /* drawn as a skinned model (from the skinned object models) */
#define MAP_OBJECT_DEF_HAS_MOTION   0x200   /* has a motion (its motion_index) */
#define MAP_OBJECT_DEF_GLOW         0x4000  /* lit by the glow lights while the glow item is worn */
#define MAP_OBJECT_DEF_BREAKABLE    0x800   /* MapObject_Break can break it */
#define MAP_OBJECT_DEF_BREAK_ANIM   0x1000  /* it has a breaking animation */

/* MapObject.work as most kinds use it */
typedef struct {
    Vector4 pad00;                  /* 0x00 */
    f32 base_x;                     /* 0x10 the x of part 1's position at the start */
    f32 base_y;                     /* 0x14 the y of part 1's position at the start */
    f32 base_z;                     /* 0x18 the z of part 1's position at the start */
    s32 timer;                      /* 0x1C counts down to 0 */
} MapObjectWork;

#define MAP_OBJECT_WORK(object) ((MapObjectWork *)(object)->work)

/* MapObject.work of the light kind: the linked point handle its hit test excludes, and the two flicker phases and periods */
typedef struct {
    s16 linked_point_handle;        /* 0x00 -1 when none */
    s16 flicker_phase_a;            /* 0x02 where the first flicker period is */
    s16 flicker_period_a;           /* 0x04 the first flicker period */
    s16 flicker_phase_b;            /* 0x06 where the second flicker period is */
    s16 flicker_period_b;           /* 0x08 the second flicker period */
} MapObjectWorkLight;

#define MAP_OBJECT_WORK_LIGHT(object) ((MapObjectWorkLight *)(object)->work)

/* MapObject.work of the kind that fades with the distance to the player (MapObject_StartDistanceFade) */
typedef struct {
    f32 near_distance;              /* 0x00 */
    f32 far_distance;               /* 0x04 */
    f32 near_alpha;                 /* 0x08 */
    f32 far_alpha;                  /* 0x0C */
} MapObjectWorkFade;

/* MapObject.work of the kinds that slide one part of their model (MapObject_MovePedestalItemIn) */
typedef struct {
    Vector4 step;                   /* 0x00 added to the part each frame */
    s32 frames;                     /* 0x10 how many frames are left */
    s32 part;                       /* 0x14 which part */
    s32 fired;                      /* 0x18 MapTriggerShot.fired */
    s32 timer;                      /* 0x1C counts down to 0 */
} MapObjectWorkSlide;

/* the same for the kind of MapObject_StartMarker, whose timer is a byte */
typedef struct {
    Vector4 step;                   /* 0x00 */
    s32 frames;                     /* 0x10 */
    s32 part;                       /* 0x14 */
    s32 fired;                      /* 0x18 */
    s8 timer;                       /* 0x1C */
} MapObjectWorkKind121DA8;

/* MapObject.door_open_flags as bit-fields */
typedef struct {
    u32 bit0 : 1;                   /* set while a door-like object is open or opening */
    u32 bits1 : 31;
} MapObjectUnk60Bits;

#define MAP_OBJECT_UNK60_BITS(object) ((MapObjectUnk60Bits *)&(object)->door_open_flags)

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

#define MAP_OBJECT_BITS(object) ((MapObjectBits *)&(object)->state_flags)

/* the same word as bytes */
typedef struct {
    u8 map_slot;                    /* 0x10 bit 0: the map slot */
    u8 step;                        /* 0x11 */
    u8 pad02;                       /* 0x12 */
    s8 anim;                        /* 0x13 */
} MapObjectBytes;

#define MAP_OBJECT_BYTES(object) ((MapObjectBytes *)&(object)->state_flags)

/* the same word through a union, for where retail reads it after stores of other types */
typedef union {
    u32 word;
    MapObjectBits bits;
} MapObjectFlags;

#define MAP_OBJECT_FLAGS(object) (((MapObjectFlags *)&(object)->state_flags)->word)

/* the same word with bit 3 on its own: MapObject_UpdateAll clears it every frame, the kinds' handlers set it */
typedef struct {
    u32 map : 1;
    u32 bit1 : 1;                   /* in view this frame */
    u32 bit2 : 1;                   /* in view the frame before */
    u32 bit3 : 1;
    u32 bits4 : 28;
} MapObjectBitsLow;

#define MAP_OBJECT_BITS_LOW(object) ((MapObjectBitsLow *)&(object)->state_flags)

/* the props of a map, looked up by Map_GetRecordById(map, id) */
typedef struct {
    char pad00[0x32];
    u16 collision_strip_index;      /* 0x32 0xFFFF when there is nothing to collide with */
    char pad34[0x48];
    ModelInstance *model;           /* 0x7C */
} MapProp;

/* what pickup_item points at: one of the nodes at fixed_items. lying_items heads
   the list of the ones in use. */
typedef struct ModelNode {
    s32 pad00;                      /* 0x00 */
    s32 unk04;                      /* 0x04 cleared when the node is unlinked */
    s32 unk08;                      /* 0x08 cleared when the node is unlinked */
    s32 unk0C;                      /* 0x0C bit 0: the map slot */
    char pad10[0x04];
    void *unk14;                    /* 0x14 the MapRecordInfo (map/map.h) it is standing in, found every frame */
    ModelInstance *model;           /* 0x18 non-null while the node is in use */
    s16 item;                       /* 0x1C which item it is (index into item_name_defs) */
    s16 unk1E;                      /* 0x1E index into the map's row of map_item_pickup_counts */
    struct ModelNode *prev;         /* 0x20 */
    struct ModelNode *next;         /* 0x24 */
    char pad28[0x14];
} ModelNode;

/* one of a map's fixed items: where it lies and what it is (0x40 bytes, map_item_defs[map]) */
typedef struct {
    Vector4 position;               /* 0x00 */
    Vector4 rotation;               /* 0x10 */
    Vector4 scale;                  /* 0x20 */
    u16 item;                       /* 0x30 which LevelDef (item_name_defs) it is */
    char pad32[0x02];
    u16 pickup_script;              /* 0x34 passed to Script_Start in place of picking the item up, 0: nothing (Interact_FindTarget) */
    u8 required_flag;               /* 0x36 nonzero: it only appears while its byte of map_item_pickup_counts is below this */
    u8 chance;                      /* 0x37 percent chance that it appears (MapItem_RollFixedItems) */
    u8 group;                       /* 0x38 0-15: picks the roll and the map's threshold it is tested against */
    char pad39[0x06];
    u8 slot_id;                     /* 0x3F its slot in fixed_item_by_id */
} MapItemDef;

/* an item lying where it was dropped, as it is saved (0x14 bytes, 16 per map at dropped_item_records) */
typedef struct {
    s16 position[3];                /* 0x00 */
    s8 rotation[3];                 /* 0x06 in 1/16 radian */
    s8 scale[3];                    /* 0x09 in 1/16 */
    u16 item;                       /* 0x0C */
    u8 used;                        /* 0x0E */
    u8 priority;                    /* 0x0F ItemNodeBits.priority */
    s32 order;                      /* 0x10 item_drop_counter when it was dropped: the oldest is replaced first */
} DroppedItemRecord;

/* a map's 16 dropped items (a row of dropped_item_records), for copying them all at once */
typedef struct {
    DroppedItemRecord records[16];  /* 0x00 */
} DroppedItemRecords;

/* a ModelNode as the item code reads it: 64 fixed items per map at fixed_items, then 16
   dropped ones per map at dropped_items */
typedef struct ItemNode {
    MapItemDef *def;                /* 0x00 NULL on a dropped item */
    DroppedItemRecord *record;      /* 0x04 NULL on a fixed item */
    s32 map_object;                 /* 0x08 the map object it was put out for (MapItem_FindForObject) */
    u32 flags;                      /* 0x0C ItemNodeBits */
    char pad10[0x04];
    void *map_record;               /* 0x14 the MapRecordInfo (map/map.h) it lies in, looked up before it is drawn */
    ModelInstance *model;           /* 0x18 */
    u16 item;                       /* 0x1C */
    s16 index;                      /* 0x1E */
    struct ItemNode *prev;          /* 0x20 */
    struct ItemNode *next;          /* 0x24 */
    f32 floor_y;                    /* 0x28 the height of the floor it lies on */
    f32 position_y;                 /* 0x2C the item's y */
    f32 fall_speed;                 /* 0x30 0 when laid down, +1 each frame in the air */
    char pad34[0x08];
} ItemNode;

/* ItemNode.flags with the map slot as a bit-field */
typedef struct {
    u32 map : 1;                    /* which of the two loaded maps it lies in */
    u32 bits1 : 31;
} ItemNodeMapBit;

#define ITEM_NODE_MAP_BIT(node) ((ItemNodeMapBit *)&(node)->flags)

/* ItemNode with all of its flags as bit-fields (MapItem_Drop) */
typedef struct {
    char pad00[0x0C];
    u32 map : 1;                    /* 0x0C which of the two loaded maps it lies in */
    u32 bit1 : 1;
    u32 bit2 : 1;                   /* set on a dropped item */
    u32 priority : 4;               /* a dropped item is only replaced by one whose priority is not lower */
    u32 bits7 : 25;
} ItemNodeBits;

#define ITEM_NODE_BITS(node) ((ItemNodeBits *)(node))

/* item_model_sources[map][32]: what is loaded for each kind of item a map holds (MapItem_FindModelSource finds one) */
typedef struct {
    char *model_data;               /* 0x00 handed to Model_SetUpPosed */
    char *texture;                  /* 0x04 becomes the model's unk194 */
} ItemModelSource;

/* the object Collision_HitCapsuleQuadObj tests against a model's points */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    char pad04[0x0C];
    int flags;                      /* 0x10 */
} CollisionOwner;

/* ChannelOwner.channel: the playback state Motion_Start resets */
typedef struct {
    ModelPartLevels *tables[0x20];  /* 0x00 MotionData.parts of each motion loaded */
    u8 limit;                       /* 0x80 entries at or above this index are replaced by 0 */
    u8 motion_index;                /* 0x81 the motion playing */
    s16 frame;                      /* 0x82 the current frame */
    s32 frame_fixed;                /* 0x84 the frame times 256 */
    u32 blend_left;                 /* 0x88 blend still to run (ModelMotion_StepBlend while nonzero) */
    u16 frame_step;                 /* 0x8C added to frame_fixed each frame (0x100: one frame) */
    u8 frame_changed;               /* 0x8E nonzero in a frame that moved on to a new frame */
    u8 blend_speed;                 /* 0x8F nonzero: a blend moves 255/blend_speed of the way each frame (ModelMotion_StepBlend) */
} ModelChannel;

/* ModelInstance.level_tables points at the model's ModelChannel, whose first 0x80 bytes are the tables */
#define MODEL_CHANNEL(model) (*(ModelChannel **)&(model)->level_tables)

/* ModelInstance.model_flags as bit-fields */
typedef struct {
    u16 has_motion : 1;             /* set once a motion is loaded (ModelMotion_Load); ModelMotion_Step animates only then */
    u16 bits1 : 15;
} ModelUnk100Bits;

#define MODEL_UNK100_BITS(model) ((ModelUnk100Bits *)&(model)->model_flags)

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

/* model/model.c */
void Model_SetUpSkinned(ModelInstance *model, int *file_cursor, int *memory);
void Model_UpdateSkinnedBonePoints(ModelInstance *actor);
// match: gcse hashes names into Model_SetUpSkinned's registers; these names and their order give retail's
f32 *Model_GetSkinnedBonePoint(ModelInstance *model, int index);
void Model_DrawSkinned(ModelInstance *model);
void Model_DrawPosed(ModelInstance *model);
void Model_MultiplyChildMatrices(ModelPart *part);
void Model_SetUvsFromScreen(void *model_arg);
s64 *Model_SaveGsWordsAndUvs(ModelInstance *model, s64 *save);
void Model_SetUpPosed(ModelInstance *model, char **file_cursor, char **memory);
void Model_DrawPosedWithAlpha(ModelInstance *model);
void Model_DrawPosedDepthByPart(ModelInstance *model);
void Model_SetPrimBits(ModelInstance *model, s64 clear_mask, s64 set_value);
void Model_SetTexture(ModelInstance *model, u64 unk40, s64 unk30);
void Model_SetAlphaBlend(ModelInstance *model, s64 value);
void Model_SetVu1Program(ModelInstance *model, s32 value);
int Model_FadeVertexColoursNear(int model_arg, void *centre_arg, int *steps, int *targets, float radius);
void Model_UpdateBonePoints(ModelInstance *model);
f32 *Model_GetBonePoint(ModelInstance *model, int index);

/* model/model_screen.c */
void ModelScreen_CutClippedTriangles(ModelInstance *model);
void ModelScreen_QueueCutTriangle(u32 *data, ModelPartVertexWords *vertex, int first, int setup, void *matrix);
void ModelScreen_StepVertices(ModelPartVertexWords *out, ModelPartVertexWords *from, ModelPartVertexWords *last, int steps);
void ModelScreen_StepFloatVertices(BlendVertex *out, BlendVertex *from, BlendVertex *last, int steps);

/* model/model_textures.c */
void ModelTexture_ResetSlots(void);
void ModelTexture_SendForModel(void *model_arg);

/* data still in asm */
extern int screen_uv_frame;  /* 0..59, stepped by ModelScreen_SetUvsFromScreen */
extern int cut_triangle_count;  /* triangles cut up this way */
extern int shared_texture_packet_sent[2];
extern int screen_uv_frame_copy;  /* the same value; nothing reads it */

/* model/model.c */
void Model_SetBlockGsWords(ModelInstance *model, s64 unk30, s64 unk40, s64 unk50, s64 unk60);
void Model_RestoreGsWordsAndUvs(ModelInstance *model);
void Model_SetVertexColour(void *model_arg, int red, int green, int blue, int alpha);
void Model_SetTextureBase(ModelInstance *model, int texture_base, int clut_base);

#endif
