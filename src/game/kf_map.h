/* Maps: the per-map slot table and the map's event triggers. */
#ifndef KF_MAP_H
#define KF_MAP_H

#include "common.h"
#include "kf_model.h"

/* a map's flag block, MapSlot.flag_blocks[n] */
typedef struct {
    u8 bits[0x20];                  /* 0x00 one bit per flag of this map */
    u8 tags[0x10];                  /* 0x20 other maps' tags... */
    u8 values[0x10];                /* 0x30 ...and the flag set for each */
} MapFlagBlock;

/* a map record with the doubleword of its flags (MapRecord.unk38) as bit-fields */
typedef struct {
    char pad00[0x38];
    u64 pad38_0 : 27;
    u64 map : 1;                    /* which of the two map slots it belongs to (func_00114AA8) */
    u64 pad38_28 : 36;
} MapRecordBits;

#define MAP_RECORD_BITS(record) ((MapRecordBits *)(record))

/* 16 bytes of a map's header; the first three are a colour */
typedef struct {
    u8 red;                         /* 0x00 */
    u8 green;                       /* 0x01 */
    u8 blue;                        /* 0x02 */
    char pad03[0x01];
    s32 unk04[3];                   /* 0x04 */
} MapFogColor;

/* one of the 32 0x20-byte blocks MapSlot.unk0C points at; a model's unk14C points at one */
typedef struct {
    u64 unk00[4];                   /* 0x00 */
} MapSlotUnk0C;

/* a MapSlotUnk0C block as the per-map set-up functions (func_00119360) fill it */
typedef struct {
    f32 unk00;                      /* 0x00 */
    f32 unk04;                      /* 0x04 */
    f32 unk08;                      /* 0x08 */
    f32 unk0C;                      /* 0x0C */
    s64 unk10;                      /* 0x10 */
    char pad18[0x08];
} MapSlotUnk0CValues;

/* one of the 32 0x80-byte blocks MapSlot.unk10 points at, as whole doublewords (see MapSlotUnk10
   in kf_entity.h); a model's unk148 points at one */
typedef struct {
    u64 unk00[16];                  /* 0x00 */
} MapSlotUnk10Words;

/* the first half of such a block: what the held item's models are lit by (D_004145E0, func_001BFCC8) */
typedef struct {
    u64 unk00[8];                   /* 0x00 */
} MapSlotUnk10Half;

/* a MapSlotUnk10Words block as the eight vectors it holds; func_0011D6B0 blends the last four
   towards D_0038D9F0 */
typedef struct {
    Vector4 unk00[4];               /* 0x00 */
    Vector4 unk40[4];               /* 0x40 */
} MapSlotUnk10Vectors;

/* a MapSlotUnk10Words block as three lights: their directions are the columns of a matrix, their
   colours (0 to 1) the vectors after it (func_00214358) */
typedef struct {
    f32 directions[4][4];           /* 0x00 [axis][light] */
    Vector4 colors[4];              /* 0x40 */
} MapSlotUnk10Lights;

/* D_0038A1B0 as func_001012D8 reads it: the first word of MapDrawState.unk60 as a float */
typedef struct {
    char pad00[0x60];
    f32 mirror_height;              /* 0x60 the mirrored view is flipped about this height */
} MapDrawStateMirror;

/* D_0038A1B0: what the maps are drawn with */
typedef struct {
    char pad00[0x60];
    s64 unk60;                      /* 0x60 */
    s32 unk68;                      /* 0x68 */
    s32 unk6C;                      /* 0x6C */
    s32 unk70;                      /* 0x70 */
    s32 unk74;                      /* 0x74 */
    s32 unk78;                      /* 0x78 */
    char pad7C[0x04];
    s64 unk80;                      /* 0x80 */
    s32 unk88;                      /* 0x88 */
    char pad8C[0x0E];
    u8 unk9A;                       /* 0x9A */
    u8 unk9B;                       /* 0x9B */
    u8 unk9C;                       /* 0x9C */
    char pad9D[0x03];
    MapSlotUnk0C unkA0;             /* 0xA0 the current map's first block */
    MapSlotUnk10Words unkC0;        /* 0xC0 the current map's first block */
    char pad140[0x140];
    MapFogColor unk280;             /* 0x280 the current map's */
    MapSlotUnk0C unk290[2][0x20];   /* 0x290 each map slot's blocks */
    MapSlotUnk10Words unkA90[2][0x20];  /* 0xA90 each map slot's blocks */
    f32 unk2A90;                    /* 0x2A90 0 to 0.25: rises while PlayerState.unk2E0 or unk10C is set (func_001B69D0) */
    f32 unk2A94;                    /* 0x2A94 copy of unk2A90 */
    f32 unk2A98;                    /* 0x2A98 copy of unk2A90 */
    char pad2A9C[0x04];
} MapDrawState;

/* MapDrawState.unk60 as bit-fields */
typedef struct {
    u64 pad00 : 46;
    u32 bit46 : 1;                  /* set: func_00116CB0 tests map records against the screen */
    u64 pad47 : 17;
} MapDrawFlagBits;

#define MAP_DRAW_FLAG_BITS(state) ((MapDrawFlagBits *)&(state)->unk60)

/* D_0038A1B0 as func_001005E8 reads it: MapDrawState.unk60 as bit-fields inside the block, so that
   they are addressed from the block's start like the fields after them */
typedef struct {
    char pad00[0x60];
    u64 pad60 : 32;
    u32 mode : 8;                   /* 0x64 how the frame is drawn: 0 plain, 1 and 2 with a second view first */
    u32 bit40 : 1;                  /* set: func_00106FB8(unk88) is drawn */
    u32 bit41 : 1;                  /* set: func_00106980 is drawn */
    u32 bit42 : 1;                  /* set: func_00106C58 is drawn */
    u32 bit43 : 1;                  /* set: func_00106DC8 is drawn */
    u32 bit44 : 1;                  /* set: func_00106520 is drawn */
    u32 bit45 : 1;                  /* set: func_00107528 is drawn (only on the main loop's call) */
    u32 pad46 : 18;
    s32 unk68;                      /* 0x68 */
    s32 unk6C;                      /* 0x6C a colour func_0010A4E8 covers the screen with */
    s32 unk70;                      /* 0x70 */
    s32 unk74;                      /* 0x74 */
    s32 unk78;                      /* 0x78 its alpha, above 0: drawn */
    char pad7C[0x04];
    s64 unk80;                      /* 0x80 its blend mode */
    s32 unk88;                      /* 0x88 */
} MapDrawStateOverlays;

/* a map record (MapRecord in kf_entity.h) as the code that draws the maps reads it */
typedef struct MapDrawRecord {
    char pad00[0x34];
    u8 unk34;                       /* 0x34 which MapSlotUnk0C block it is drawn with */
    u8 unk35;                       /* 0x35 which MapSlotUnk10Words block it is drawn with */
    char pad36[0x06];
    s32 flags;                      /* 0x3C high half of MapRecord.unk38 */
    char pad40[0x2C];
    struct MapDrawRecord *next;     /* 0x6C next in its draw list (D_00446778, D_00446780) */
    char pad70[0x0C];
    ModelInstance *model;           /* 0x7C */
} MapDrawRecord;

/* D_00471F60[2][16] and D_00471FE0[2][16]: 16 pairs of values per map slot (func_00116F20 clears them) */
typedef struct {
    s16 unk00;                      /* 0x00 */
    s16 unk02;                      /* 0x02 */
} MapSlotPair;

/* one of the 16 0xC-byte entries MapSlot.unk38 points at: a texture that func_00116F98 scrolls in
   place by copying it through a spare buffer, as far as the matching D_00471F60 pair says */
typedef struct {
    u16 base;                       /* 0x00 0: the entry is unused */
    u16 format;                     /* 0x02 */
    u16 width;                      /* 0x04 */
    u16 height;                     /* 0x06 */
    u8 step_x;                      /* 0x08 signed; added to the pair each time */
    u8 step_y;                      /* 0x09 signed */
    u8 period;                      /* 0x0A it moves every this many frames; 0: every frame */
    u8 mode;                        /* 0x0B which call of func_00116F98 moves it */
} MapScrollCopy;

/* one of the 16 0x28-byte entries MapSlot.unk3C points at: a texture that func_001175F8 draws into
   another one, moved along by the matching D_00471FE0 pair */
typedef struct {
    u16 width;                      /* 0x00 */
    u16 height;                     /* 0x02 */
    u16 format;                     /* 0x04 */
    u16 base;                       /* 0x06 */
    u16 clut_format;                /* 0x08 */
    u16 clut_base;                  /* 0x0A */
    f32 scale_x;                    /* 0x0C */
    f32 scale_y;                    /* 0x10 */
    u16 target_width;               /* 0x14 */
    u16 target_height;              /* 0x16 */
    u16 target_format;              /* 0x18 */
    u16 target_base;                /* 0x1A in words; 0: the entry is unused */
    u8 red;                         /* 0x1C */
    u8 green;                       /* 0x1D */
    u8 blue;                        /* 0x1E */
    u8 alpha;                       /* 0x1F */
    u8 blend;                       /* 0x20 the GS ALPHA register's A, B, C and D, two bits each */
    u8 blend_fix;                   /* 0x21 and its FIX */
    u8 period;                      /* 0x22 it moves every this many frames; 0: every frame */
    u8 mode;                        /* 0x23 which call of func_001175F8 draws it */
    s16 step_x;                     /* 0x24 added to the pair each time */
    s16 step_y;                     /* 0x26 */
} MapScrollTexture;

/* MapScrollTexture.blend as bit-fields of the word it starts */
typedef struct {
    u32 a : 2;
    u32 b : 2;
    u32 c : 2;
    u32 d : 2;
    u32 pad8 : 24;
} MapScrollBlendBits;

#define MAP_SCROLL_BLEND_BITS(texture) ((MapScrollBlendBits *)&(texture)->blend)

/* MapSlot+0x40: prebuilt packets, each a tag word and 0x10 further on its data */
typedef struct {
    u32 *items[7];                  /* 0x00 */
} MapSlotPackets;

/* what MapSlot.header points at: the loaded map's header, as func_0019F3A0 reads it */
typedef struct {
    char pad00[0x20];
    s32 item_models_offset;         /* 0x20 from the header to the item models: sized blocks, a model then its texture per kind */
    char pad24[0x18];
    u16 record_count;               /* 0x3C how many MapSlot.records there are */
    char pad3E[0x06];
    u16 item_count;                 /* 0x44 how many fixed items (MapItemDef) the map has */
    char pad46[0x2A];
    s8 unk70;                       /* 0x70 passed to sprintf as the file-name index */
    u8 unk71;                       /* 0x71 which block of MapDrawState.unkA90 the items lying in the map are drawn with */
} MapHeader;

/* one 0x60-byte entry of D_0038CD20, indexed by the current map/slot D_00446770 */
typedef struct MapSlot {
    u8 unk00;                       /* 0x00 */
    u8 tag;                         /* 0x01 matched against MapFlagBlock.tags */
    u8 unk02;                       /* 0x02 nonzero: the slot's map is being drawn */
    char pad03[0x01];
    s32 unk04;                      /* 0x04 the map file's size, rounded up to 0x40000 */
    MapHeader *header;              /* 0x08 */
    MapSlotUnk0C *unk0C;            /* 0x0C 32 blocks */
    void *unk10;                    /* 0x10 */
    char *records;                  /* 0x14 0x80-byte records, by PlayerState.map_index */
    MapFlagBlock *flag_blocks;      /* 0x18 */
    Vector4 *origins;               /* 0x1C by EntityController.unk43A */
    char pad20[0x04];
    char *unk24;                    /* 0x24 room for MapSlotUnk08.unk2C words */
    char *unk28;                    /* 0x28 room for MapSlotUnk08.unk2C bytes */
    char pad2C[0x04];
    char *unk30;                    /* 0x30 the file at MapSlotUnk08.unk24: the first of the packets */
    struct SoundSpecSource *unk34;  /* 0x34 the file at MapSlotUnk08.unk28: the map's sound bank */
    char *unk38;                    /* 0x38 the file at 0x1480 */
    char *unk3C;                    /* 0x3C the file at 0x1540 */
    MapSlotPackets unk40;           /* 0x40 */
    s32 unk5C;                      /* 0x5C passed to func_00210930 when the map becomes current */
} MapSlot;

/* what MapSlot.unk08 points at */
typedef struct {
    s32 unk00;                      /* 0x00 how long the map file's first part is */
    s32 unk04;                      /* 0x04 offset in the file of the record models (ModelFile, back to back) */
    s32 unk08;                      /* 0x08 offset of the record collision meshes */
    s32 unk0C;                      /* 0x0C offset of the plain object models */
    s32 unk10;                      /* 0x10 offset of the jointed object models (func_0010B758) */
    s32 unk14;                      /* 0x14 offset of the object collision meshes */
    s32 unk18;                      /* 0x18 offset of the object motions (func_00108940) */
    char pad1C[0x08];
    s32 unk24;                      /* 0x24 offset in the file of what MapSlot.unk30 points at */
    s32 unk28;                      /* 0x28 offset in the file of what MapSlot.unk34 points at */
    u16 unk2C;                      /* 0x2C how many entries MapSlot.unk24 and unk28 have */
    char pad2E[0x02];
    u16 unk30;                      /* 0x30 how many plain object models there are */
    u16 unk32;                      /* 0x32 how many jointed object models */
    u16 unk34;                      /* 0x34 how many object collision meshes */
    u16 unk36;                      /* 0x36 how many object motions */
    char pad38[0x02];
    u16 unk3A;                      /* 0x3A how many models MapSlotMemory.unk48 has room for */
    u16 record_count;               /* 0x3C how many MapSlot.records there are */
    u16 unk3E;                      /* 0x3E how many MapSlot.flag_blocks there are */
    u16 unk40;                      /* 0x40 */
    u16 unk42;                      /* 0x42 */
    u16 unk44;                      /* 0x44 */
    char pad46[0x06];
    s32 unk4C;                      /* 0x4C offset in the file of what MapSlot.unk5C points at */
    MapFogColor unk50;              /* 0x50 becomes MapDrawState.unk280 when the map becomes current */
    f32 unk60;                      /* 0x60 where the player starts: x */
    f32 unk64;                      /* 0x64 y */
    f32 unk68;                      /* 0x68 z */
    f32 unk6C;                      /* 0x6C and the heading */
    s8 unk70;                       /* 0x70 becomes SoundLoadQueue.unk00 */
    char pad71[0x07];
    u8 unk78;                       /* 0x78 passed to func_00111368 with the three after it */
    u8 unk79;                       /* 0x79 */
    u8 unk7A;                       /* 0x7A */
    u8 unk7B;                       /* 0x7B */
} MapSlotUnk08;

/* a MapRecord (kf_entity.h) as the map code reads it */
typedef struct {
    char pad00[0x34];
    u8 unk34;                       /* 0x34 */
    u8 unk35;                       /* 0x35 */
    char pad36[0x02];
    u8 unk38;                       /* 0x38 */
    u8 unk39;                       /* 0x39 nonzero: the neighbouring map must be loaded before going on */
    u8 unk3A;                       /* 0x3A which of MapSlot.origins leads to the neighbouring map, 0xFF: none */
    char pad3B[0x01];
    s32 unk3C;                      /* 0x3C flags */
    char pad40[0x3C];
    ModelInstance *model;           /* 0x7C */
} MapRecordInfo;

/* where a map change leads: a position in the new map and the map's number. D_0038D640 is one. */
typedef struct {
    f32 x;                          /* 0x00 */
    f32 y;                          /* 0x04 */
    f32 z;                          /* 0x08 */
    u16 map;                        /* 0x0C the low byte becomes MapSlot.tag */
} MapLoadRequest;

/* D_0038D610: the background load of a neighbouring map into the slot that is not current */
typedef struct {
    Vector4 position;               /* 0x00 the request's, kept once the load is done */
    s32 step;                       /* 0x10 index into the step handlers */
    MapLoadRequest *request;        /* 0x14 */
    MapLoadRequest *next_request;   /* 0x18 asked for while a load was running */
    s32 file_handle;                /* 0x1C -1 when closed */
    union {
        u64 raw;
        struct {
            u64 bit0 : 1;
            u64 bit1 : 1;
            u64 bit2 : 1;
        } f;
    } flags;                        /* 0x20 */
    s32 timer;                      /* 0x28 frames to wait */
    char pad2C[0x04];
} MapLoadState;

/* D_004321F0[2]: the memory each map slot is loaded into */
typedef struct {
    char pad00[0x14];
    char *unk14;                    /* 0x14 where the next read goes */
    char *unk18;                    /* 0x18 */
    char *unk1C;                    /* 0x1C the start of the map file */
    char *unk20;                    /* 0x20 */
    char pad24[0x10];
    char *unk34;                    /* 0x34 room for a model (0x1C0 bytes) per record */
    char pad38[0x04];
    char *unk3C;                    /* 0x3C room for MapSlotUnk08.unk40 models */
    char pad40[0x04];
    ModelInstance *unk44;           /* 0x44 the models of the map's entities, one each */
    char *unk48;                    /* 0x48 room for MapSlotUnk08.unk3A models */
    char pad4C[0x0C];
} MapSlotMemory;

/* the trigger's one-shot state, MapTrigger+0x40 */
typedef struct {
    int unk00[6];                   /* 0x00 */
    int fired;                      /* 0x18 the type it fired for, 0 before */
} MapTriggerShot;

/* MapTrigger.shot as func_00121660 (start) and func_00121680 (update) use it */
typedef struct {
    f32 unk00;                      /* 0x00 a speed, added to the model's y */
    s32 unk04;                      /* 0x04 frames to wait */
    s32 unk08;                      /* 0x08 1 while D_00414188 is this trigger */
    s32 unk0C;                      /* 0x0C unk08 of the frame before */
} MapTriggerShotKind121660;

/* what MapTrigger.unk08 points at, as func_0011F0D8 reads it */
typedef struct {
    char pad00[0x44];
    u16 unk44;                      /* 0x44 */
} TriggerSource;

/* a map event trigger (func_00122538) */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    u8 *unk04;                      /* 0x04 */
    char *unk08;                    /* 0x08 TriggerSource; byte 0x3A is passed to func_00103BA8 */
    char pad0C[0x04];
    u8 unk10;                       /* 0x10 bit 0 picks the MapSlot */
    u8 type;                        /* 0x11 */
    char pad12[0x2E];
    MapTriggerShot shot;            /* 0x40 */
} MapTrigger;

#define MAP_TRIGGER_SOURCE(trigger) ((TriggerSource *)(trigger)->unk08)

/* the word at MapTrigger+0x10 (unk10, type and flag bits) as the game reads it whole */
typedef union {
    u32 word;                       /* MAP_TRIGGER_FLAG_20000 in bit 17 */
    struct {
        u32 unk10 : 8;
        u32 type : 8;
        u32 pad16 : 1;
        u32 flag_20000 : 1;         /* MAP_TRIGGER_FLAG_20000 */
        u32 pad18 : 14;
    } bits;
} MapTriggerFlags;

#define MAP_TRIGGER_FLAGS(trigger) ((MapTriggerFlags *)&(trigger)->unk10)
#define MAP_TRIGGER_FLAG_20000  0x20000

/* MapTrigger.shot as the kind started by func_00122E88 uses it (func_00122F48 steps it) */
typedef struct {
    s32 phase;                      /* 0x00 */
    s32 period;                     /* 0x04 */
} MapTriggerShotKind122E88;

/* MapTrigger.shot as the kind started by func_00122BE8 uses it */
typedef struct {
    f32 unk00[4];                   /* 0x00 a vector handed to func_0010EA08 */
    f32 unk10;                      /* 0x10 grows while it moves */
} MapTriggerShotKind122BE8;

/* D_00367928[5]: which item was placed on which trigger (func_00103BA8 records, func_00103AE0 looks up) */
typedef struct {
    u8 map_tag;                     /* 0x00 MapSlot.tag; 0x11-0x13 are stored as 0x11 */
    u8 trigger_id;                  /* 0x01 byte 0x3A of the trigger's source */
    u16 item;                       /* 0x02 0: free */
} TriggerItemRecord;

#define TRIGGER_ITEM_RECORD_COUNT 5

/* D_0041A6F8[]: 0x20-byte definitions whose first byte is a trigger type */
typedef struct {
    u8 type;                        /* 0x00 */
    u16 unk01_0 : 4;                /* 0x01 */
    u16 use_mode : 4;               /* using one: 0 takes it, 2 takes it and gives one of unk02 (func_001BC2D0) */
    u16 unk02;                      /* 0x02 an item (D_00369830) */
    u16 unk04;                      /* 0x04 added to PlayerState.unkB4 when a type 0 one is used */
    u16 unk06;                      /* 0x06 added to PlayerState.unkB8 */
    u16 unk08;                      /* 0x08 bit n set: starts PlayerState.timed_flags[n] for 60 frames */
    char pad0A[0x04];
    u16 flags;                      /* 0x0E bit 1 */
    char pad10[0x10];
} TriggerTypeDef;

/* TriggerTypeDef.flags as bit-fields, where retail reads the low byte alone (func_001D2680) */
typedef struct {
    char pad00[0x0E];
    u8 unlisted : 1;                /* 0x0E set: the item is left out of the menu's list of things to use */
    u8 bit1 : 1;
    u8 bits2 : 6;
    char pad0F[0x11];
} TriggerTypeDefBits;

#define TRIGGER_TYPE_DEF_BITS(def) ((TriggerTypeDefBits *)(def))

/* D_005B6100[map slot][16]: a map actor (0x2C bytes); flags bit 2 is the map slot */
typedef struct MapActor {
    u32 flags;                      /* 0x00 bit 0: in use, bit 1: near enough to pick up, bit 2: the map slot; the high half is MapActorWords.amount */
    s8 step;                        /* 0x04 0: fading in, 1: lying there, 2: fading out */
    u8 fade_timer;                  /* 0x05 frames left of the fade */
    char pad06[0x02];
    f32 unk08;                      /* 0x08 D_004469DC when it was dropped */
    u32 serial;                     /* 0x0C the slot's count of drops when it was dropped: the lowest is the oldest */
    ModelInstance *body;            /* 0x10 */
    f32 unk14;                      /* 0x14 */
    struct MapActor *prev;          /* 0x18 */
    struct MapActor *next;          /* 0x1C the list of the ones in use starts at D_004469E0 */
    char pad20[0x04];
    u8 unk24;                       /* 0x24 */
    char pad25[0x07];
} MapActor;

/* a MapActor where the code reads the high half of flags and the word at 0x24 whole */
typedef struct {
    u16 flags_low;                  /* 0x00 */
    u16 amount;                     /* 0x02 how much gold it is worth */
    char pad04[0x20];
    s32 object_id;                  /* 0x24 the map object that dropped it (func_0011E678), 0: none */
    char pad28[0x04];
} MapActorWords;

#define MAP_ACTOR_WORDS(actor) ((MapActorWords *)(actor))

/* MapActor.flags as bit-fields */
typedef struct {
    u32 in_use : 1;
    u32 in_reach : 1;               /* the nearest one to the player, when it is close enough to pick up */
    u32 map : 1;                    /* which of the two loaded maps it lies in */
    u32 bits3 : 13;
    u32 amount : 16;
} MapActorBits;

#define MAP_ACTOR_BITS(actor) ((MapActorBits *)&(actor)->flags)
#define MAP_ACTOR_COUNT 16

/* D_00362928[MapSlot.tag][16]: where a map object is marked (0xA bytes) */
typedef struct {
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    char radius;                    /* 0x06 */
    u8 unk07;                       /* 0x07 */
    u16 unk08;                      /* 0x08 */
} ObjectMark;

/* a map's 16 marks, as func_001CA920 copies them whole */
typedef struct {
    ObjectMark marks[16];
} ObjectMarkRow;

#endif
