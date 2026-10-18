/* Maps: the per-map slot table and the map's event triggers. */
#ifndef KF_MAP_H
#define KF_MAP_H

#include "common.h"
#include "model/model.h"

/* a map's flag block, MapSlot.flag_blocks[n] */
typedef struct {
    u8 bits[0x20];                  /* 0x00 one bit per flag of this map */
    u8 tags[0x10];                  /* 0x20 other maps' tags... */
    u8 values[0x10];                /* 0x30 ...and the flag set for each */
} MapFlagBlock;

/* a map record with the doubleword of its flags (MapRecord.record_flags) as bit-fields */
typedef struct {
    char pad00[0x38];
    u64 pad38_0 : 27;
    u64 map : 1;                    /* which of the two map slots it belongs to (Map_BuildModels) */
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

/* one of the 32 0x20-byte blocks MapSlot.fog_blocks points at; a model's unk14C points at one */
typedef struct {
    u64 unk00[4];                   /* 0x00 */
} MapSlotUnk0C;

/* a MapSlotUnk0C block as the per-map set-up functions (MapSetup_RunForMap) fill it */
typedef struct {
    f32 unk00;                      /* 0x00 */
    f32 unk04;                      /* 0x04 */
    f32 unk08;                      /* 0x08 */
    f32 unk0C;                      /* 0x0C */
    s64 unk10;                      /* 0x10 */
    char pad18[0x08];
} MapSlotUnk0CValues;

/* one of the 32 0x80-byte blocks MapSlot.light_blocks points at, as whole doublewords (see MapSlotUnk10
   in entity/entity.h); a model's unk148 points at one */
typedef struct {
    u64 unk00[16];                  /* 0x00 */
} MapSlotUnk10Words;

/* the first half of such a block: what the held item's models are lit by (held_item_light_blocks, Player_UpdateArmModels) */
typedef struct {
    u64 unk00[8];                   /* 0x00 */
} MapSlotUnk10Half;

/* a MapSlotUnk10Words block as the eight vectors it holds; MapObject_DrawAll blends the last four
   towards map_object_glow_colors */
typedef struct {
    Vector4 directions[4];          /* 0x00 the light directions (MapSlotUnk10Lights) */
    Vector4 colors[4];              /* 0x40 the light colours (MapSlotUnk10Lights) */
} MapSlotUnk10Vectors;

/* a MapSlotUnk10Words block as three lights: their directions are the columns of a matrix, their
   colours (0 to 1) the vectors after it (ScriptCmd_SetMapLight) */
typedef struct {
    f32 directions[4][4];           /* 0x00 [axis][light] */
    Vector4 colors[4];              /* 0x40 */
} MapSlotUnk10Lights;

/* map_draw_state as GameLoop_SetCameraMatrices reads it: the first word of MapDrawState.draw_flags as a float */
typedef struct {
    char pad00[0x60];
    f32 mirror_height;              /* 0x60 the mirrored view is flipped about this height */
} MapDrawStateMirror;

/* map_draw_state: what the maps are drawn with */
typedef struct {
    char pad00[0x60];
    s64 draw_flags;                 /* 0x60 the overlay mode byte and the bits for the drawn overlays */
    s32 hud_shown;                  /* 0x68 nonzero: Hud_Draw draws the HUD */
    s32 screen_fill_red;            /* 0x6C red of the colour GsDraw_FillScreen covers the screen with */
    s32 screen_fill_green;          /* 0x70 green of that colour */
    s32 screen_fill_blue;           /* 0x74 blue of that colour */
    s32 screen_fill_alpha;          /* 0x78 alpha of that colour; 0: the screen is not covered */
    char pad7C[0x04];
    s64 screen_fill_blend;          /* 0x80 the GS ALPHA register GsDraw_FillScreen is handed */
    s32 previous_frame_alpha;       /* 0x88 alpha Game_BlendPreviousFrame blends the last frame with */
    char pad8C[0x0E];
    u8 unk9A;                       /* 0x9A */
    u8 unk9B;                       /* 0x9B */
    u8 unk9C;                       /* 0x9C */
    char pad9D[0x03];
    MapSlotUnk0C current_fog_block; /* 0xA0 the current map's first fog block */
    MapSlotUnk10Words current_light_block; /* 0xC0 the current map's first light block */
    char pad140[0x140];
    MapFogColor current_fog_color;  /* 0x280 the current map's fog colour */
    MapSlotUnk0C fog_blocks[2][0x20]; /* 0x290 each map slot's blocks */
    MapSlotUnk10Words light_blocks[2][0x20]; /* 0xA90 each map slot's blocks */
    f32 screen_tint_amount;         /* 0x2A90 0 to 0.25: rises while PlayerState.light_spell_timer or unk10C is set (Player_UpdateScreenTints) */
    f32 screen_tint_amount_copy;    /* 0x2A94 copy of unk2A90 */
    f32 screen_tint_amount_copy2;   /* 0x2A98 copy of unk2A90 */
    char pad2A9C[0x04];
} MapDrawState;

/* MapDrawState.draw_flags as bit-fields */
typedef struct {
    u64 pad00 : 46;
    u32 bit46 : 1;                  /* set: Map_ClassifyRecordOnScreen tests map records against the screen */
    u64 pad47 : 17;
} MapDrawFlagBits;

#define MAP_DRAW_FLAG_BITS(state) ((MapDrawFlagBits *)&(state)->draw_flags)

/* map_draw_state as GameLoop_DrawFrame reads it: MapDrawState.draw_flags as bit-fields inside the block, so that
   they are addressed from the block's start like the fields after them */
typedef struct {
    char pad00[0x60];
    u64 pad60 : 32;
    u32 mode : 8;                   /* 0x64 how the frame is drawn: 0 plain, 1 and 2 with a second view first */
    u32 bit40 : 1;                  /* set: Game_BlendPreviousFrame(previous_frame_alpha) is drawn */
    u32 bit41 : 1;                  /* set: Game_DrawDepthTestSprite is drawn */
    u32 bit42 : 1;                  /* set: Game_BlendScreenOffsetDown is drawn */
    u32 bit43 : 1;                  /* set: Game_BlendScreenOffsetUpDown is drawn */
    u32 bit44 : 1;                  /* set: Game_DrawHazeLayers is drawn */
    u32 bit45 : 1;                  /* set: Game_ShiftScreenByViewTurn is drawn (only on the main loop's call) */
    u32 pad46 : 18;
    s32 hud_shown;                  /* 0x68 nonzero: Hud_Draw draws the HUD */
    s32 screen_fill_red;            /* 0x6C a colour GsDraw_FillScreen covers the screen with */
    s32 screen_fill_green;          /* 0x70 green of that colour */
    s32 screen_fill_blue;           /* 0x74 blue of that colour */
    s32 screen_fill_alpha;          /* 0x78 its alpha, above 0: drawn */
    char pad7C[0x04];
    s64 screen_fill_blend;          /* 0x80 its blend mode */
    s32 previous_frame_alpha;       /* 0x88 alpha Game_BlendPreviousFrame blends the last frame with */
} MapDrawStateOverlays;

/* a map record (MapRecord in entity/entity.h) as the code that draws the maps reads it */
typedef struct MapDrawRecord {
    char pad00[0x34];
    u8 fog_block_index;             /* 0x34 which MapSlotUnk0C block it is drawn with */
    u8 light_block_index;           /* 0x35 which MapSlotUnk10Words block it is drawn with */
    char pad36[0x02];
    u8 linked_point_owner;          /* 0x38 low byte of MapRecord.linked_point_owner: compared with LinkedPoint.unk32 (LinkedPoint_FindHitsForMapDraw) */
    char pad39[0x03];
    s32 flags;                      /* 0x3C high half of MapRecord.record_flags */
    f32 box_min[4];                 /* 0x40 lowest x, y, z of the box linked points are tested against (LinkedPoint_FindHitsForMapDraw) */
    f32 box_max[4];                 /* 0x50 highest x, y, z of that box */
    char pad60[0x0C];
    struct MapDrawRecord *next;     /* 0x6C next in its draw list (record_draw_lists, late_record_draw_lists) */
    char pad70[0x0C];
    ModelInstance *model;           /* 0x7C */
} MapDrawRecord;

/* map_scroll_copy_pairs and map_scroll_draw_pairs in map/map_scroll.c: 16 pairs per map slot */
typedef struct {
    s16 scroll_x;                   /* 0x00 scroll offset in x, wrapped at the texture width */
    s16 scroll_y;                   /* 0x02 scroll offset in y, wrapped at the texture height */
} MapSlotPair;

/* one of the 16 0xC-byte entries MapSlot.scroll_copy_textures points at: a texture that MapScroll_UpdateCopy scrolls in
   place by copying it through a spare buffer, as far as the matching copy pair says */
typedef struct {
    u16 base;                       /* 0x00 0: the entry is unused */
    u16 format;                     /* 0x02 */
    u16 width;                      /* 0x04 */
    u16 height;                     /* 0x06 */
    u8 step_x;                      /* 0x08 signed; added to the pair each time */
    u8 step_y;                      /* 0x09 signed */
    u8 period;                      /* 0x0A it moves every this many frames; 0: every frame */
    u8 mode;                        /* 0x0B which call of MapScroll_UpdateCopy moves it */
} MapScrollCopy;

/* one of the 16 0x28-byte entries MapSlot.scroll_draw_textures points at: a texture that MapScroll_UpdateDraw draws into
   another one, moved along by the matching draw pair */
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
    u8 mode;                        /* 0x23 which call of MapScroll_UpdateDraw draws it */
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

/* what MapSlot.header points at: the loaded map's header, as Sound_ReadMapBankFileStep reads it */
typedef struct {
    char pad00[0x20];
    s32 item_models_offset;         /* 0x20 from the header to the item models: sized blocks, a model then its texture per kind */
    char pad24[0x18];
    u16 record_count;               /* 0x3C how many MapSlot.records there are */
    char pad3E[0x06];
    u16 item_count;                 /* 0x44 how many fixed items (MapItemDef) the map has */
    char pad46[0x2A];
    s8 file_index;                  /* 0x70 passed to sprintf as the file-name index */
    u8 light_block;                 /* 0x71 which block of MapDrawState.light_blocks the items lying in the map are drawn with */
} MapHeader;

/* one 0x60-byte entry of map_slots, indexed by the current map/slot current_map_slot */
typedef struct MapSlot {
    u8 loaded;                      /* 0x00 nonzero: the slot holds a loaded map */
    u8 tag;                         /* 0x01 matched against MapFlagBlock.tags */
    u8 has_flag_80000000_record;    /* 0x02 nonzero: the slot's map is being drawn */
    char pad03[0x01];
    s32 rounded_file_size;          /* 0x04 the map file's size, rounded up to 0x40000 */
    MapHeader *header;              /* 0x08 */
    MapSlotUnk0C *fog_blocks;       /* 0x0C 32 blocks */
    void *light_blocks;             /* 0x10 the map's 32 light blocks (MapSlotUnk10), picked per record */
    char *records;                  /* 0x14 0x80-byte records, by PlayerState.map_index */
    MapFlagBlock *flag_blocks;      /* 0x18 */
    Vector4 *origins;               /* 0x1C by EntityController.entity_group_index */
    char pad20[0x04];
    char *record_model_table;       /* 0x24 room for MapSlotUnk08.record_model_count words */
    char *record_model_spare_bytes; /* 0x28 room for MapSlotUnk08.record_model_count bytes */
    char pad2C[0x04];
    char *packet_data;              /* 0x30 the file at MapSlotUnk08.record_model_table: the first of the packets */
    struct SoundSpecSource *sound_bank; /* 0x34 the file at MapSlotUnk08.record_model_spare_bytes: the map's sound bank */
    char *scroll_copy_textures;     /* 0x38 the file at 0x1480 */
    char *scroll_draw_textures;     /* 0x3C the file at 0x1540 */
    MapSlotPackets packets;         /* 0x40 the map slot's packets */
    s32 script;                     /* 0x5C passed to Script_Init when the map becomes current */
} MapSlot;

/* what MapSlot.unk08 points at */
typedef struct {
    s32 file_size;                  /* 0x00 how long the map file's first part is */
    s32 record_models_offset;       /* 0x04 offset in the file of the record models (ModelFile, back to back) */
    s32 record_collision_offset;    /* 0x08 offset of the record collision meshes */
    s32 object_model_offset;        /* 0x0C offset of the plain object models */
    s32 skinned_object_model_offset; /* 0x10 offset of the jointed object models (Model_SetUpSkinned) */
    s32 object_collision_offset;    /* 0x14 offset of the object collision meshes */
    s32 object_motion_offset;       /* 0x18 offset of the object motions (ModelMotion_LoadFile) */
    char pad1C[0x08];
    s32 packet_table_offset;        /* 0x24 offset in the file of what MapSlot.packet_data points at */
    s32 sound_bank_offset;          /* 0x28 offset in the file of what MapSlot.sound_bank points at */
    u16 record_model_count;         /* 0x2C how many entries MapSlot.record_model_table and unk28 have */
    char pad2E[0x02];
    u16 plain_object_model_count;   /* 0x30 how many plain object models there are */
    u16 skinned_object_model_count; /* 0x32 how many jointed object models */
    u16 object_collision_count;     /* 0x34 how many object collision meshes */
    u16 object_motion_count;        /* 0x36 how many object motions */
    char pad38[0x02];
    u16 extra_model_count;          /* 0x3A how many models MapSlotMemory.extra_models has room for */
    u16 record_count;               /* 0x3C how many MapSlot.records there are */
    u16 flag_block_count;           /* 0x3E how many MapSlot.flag_blocks there are */
    u16 object_count;               /* 0x40 how many map objects (and object records) the map has */
    u16 spawn_count;                /* 0x42 how many entity spawn records (EntitySpawnInfo) the map has */
    u16 item_count;                 /* 0x44 how many fixed items (MapItemDef) the map has */
    char pad46[0x06];
    s32 script_offset;              /* 0x4C offset in the file of what MapSlot.script points at */
    MapFogColor fog_color;          /* 0x50 becomes MapDrawState.current_fog_color when the map becomes current */
    f32 player_start_x;             /* 0x60 where the player starts: x */
    f32 player_start_y;             /* 0x64 y */
    f32 player_start_z;             /* 0x68 z */
    f32 player_start_heading;       /* 0x6C and the heading */
    s8 sound_bank_file_index;       /* 0x70 becomes SoundLoadQueue.bgm_id */
    char pad71[0x07];
    u8 screen_tint_red;             /* 0x78 passed to Wobble_DrawRippledModel with the three after it */
    u8 screen_tint_green;           /* 0x79 green of that colour */
    u8 screen_tint_blue;            /* 0x7A blue of that colour */
    u8 screen_tint_alpha;           /* 0x7B alpha of that colour; Wobble takes all four as one colour */
} MapSlotUnk08;

/* a MapRecord (entity/entity.h) as the map code reads it */
typedef struct {
    char pad00[0x34];
    u8 draw_block_index;            /* 0x34 which block of the map's draw state its models use */
    u8 unk35;                       /* 0x35 */
    char pad36[0x02];
    u8 flag_block_bit;              /* 0x38 its bit in the map's flag block: are its objects in use */
    u8 needs_neighbour_map;         /* 0x39 nonzero: the neighbouring map must be loaded before going on */
    u8 neighbour_origin_index;      /* 0x3A which of MapSlot.origins leads to the neighbouring map, 0xFF: none */
    char pad3B[0x01];
    s32 unk3C;                      /* 0x3C flags */
    char pad40[0x3C];
    ModelInstance *model;           /* 0x7C */
} MapRecordInfo;

/* where a map change leads: a position in the new map and the map's number. map_load_request is one. */
typedef struct {
    f32 x;                          /* 0x00 */
    f32 y;                          /* 0x04 */
    f32 z;                          /* 0x08 */
    u16 map;                        /* 0x0C the low byte becomes MapSlot.tag */
} MapLoadRequest;

/* map_load_state: the background load of a neighbouring map into the slot that is not current */
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

/* map_slot_memory[2]: the memory each map slot is loaded into */
typedef struct {
    char pad00[0x14];
    char *file_read_cursor;         /* 0x14 where the next read goes */
    char *work_cursor;              /* 0x18 next free byte of the slot's work area (models and tables) */
    char *file_base;                /* 0x1C the start of the map file */
    char *work_base;                /* 0x20 the start of the slot's half of the work area, cleared on load */
    char pad24[0x10];
    char *record_models;            /* 0x34 room for a model (0x1C0 bytes) per record */
    char pad38[0x04];
    char *object_models;            /* 0x3C room for MapSlotUnk08.object_count models */
    char pad40[0x04];
    ModelInstance *entity_models;   /* 0x44 the models of the map's entities, one each */
    char *extra_models;             /* 0x48 room for MapSlotUnk08.extra_model_count models */
    char pad4C[0x0C];
} MapSlotMemory;

/* the trigger's one-shot state, MapTrigger+0x40 */
typedef struct {
    int unk00[6];                   /* 0x00 */
    int fired;                      /* 0x18 the type it fired for, 0 before */
} MapTriggerShot;

/* MapTrigger.shot as MapObject_StartSinkingPlatform (start) and MapObject_UpdateSinkingPlatform (update) use it */
typedef struct {
    f32 speed;                      /* 0x00 a speed, added to the model's y */
    s32 frames_to_wait;             /* 0x04 frames to wait */
    s32 player_on;                  /* 0x08 1 while object_under_player is this trigger */
    s32 player_was_on;              /* 0x0C player_on of the frame before */
} MapTriggerShotKind121660;

/* what MapTrigger.source points at, as MapObject_StopAmbientSound reads it */
typedef struct {
    char pad00[0x44];
    u16 sound_tag;                  /* 0x44 the sound tag, Sound_StopTagged stops it */
} TriggerSource;

/* a map event trigger (MapObject_PlaceStoneOnMarker) */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    u8 *unk04;                      /* 0x04 */
    char *source;                   /* 0x08 TriggerSource; byte 0x3A is passed to Game_AddTriggerItem */
    char pad0C[0x04];
    u8 flags;                       /* 0x10 bit 0 picks the MapSlot */
    u8 type;                        /* 0x11 */
    char pad12[0x2E];
    MapTriggerShot shot;            /* 0x40 */
} MapTrigger;

#define MAP_TRIGGER_SOURCE(trigger) ((TriggerSource *)(trigger)->source)

/* the word at MapTrigger+0x10 (flags, type and flag bits) as the game reads it whole */
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

#define MAP_TRIGGER_FLAGS(trigger) ((MapTriggerFlags *)&(trigger)->flags)
#define MAP_TRIGGER_FLAG_20000  0x20000

/* MapTrigger.shot as the kind started by MapObject_StartPulse uses it (MapObject_UpdatePulse steps it) */
typedef struct {
    s32 phase;                      /* 0x00 */
    s32 period;                     /* 0x04 */
} MapTriggerShotKind122E88;

/* MapTrigger.shot as the kind started by MapObject_StartFadeObject uses it */
typedef struct {
    f32 fade_vector[4];             /* 0x00 a vector handed to Model_FadeVertexColoursNear */
    f32 fade_progress;              /* 0x10 grows while it moves */
} MapTriggerShotKind122BE8;

/* trigger_items[5]: which item was placed on which trigger (Game_AddTriggerItem records, Game_GetTriggerItem looks up) */
typedef struct {
    u8 map_tag;                     /* 0x00 MapSlot.tag; 0x11-0x13 are stored as 0x11 */
    u8 trigger_id;                  /* 0x01 byte 0x3A of the trigger's source */
    u16 item;                       /* 0x02 0: free */
} TriggerItemRecord;

#define TRIGGER_ITEM_RECORD_COUNT 5

/* item_common_defs[150]: param/item_cmn.prm, a row for each item below ITEM_WEAPON_FIRST; type says what using it
   does (Player_UseItem): 0 heals (the Herb...), 1 and 5 are used on a map object, 2 on the map object the player
   stands on (the wands), 3 are the keys, 6 the maps, 8 "Nothing happened" */
typedef struct {
    u8 type;                        /* 0x00 */
    u16 unk01_0 : 4;                /* 0x01 */
    u16 use_mode : 4;               /* using one: 0 takes it, 2 takes it and gives one of item (Player_UseItem) */
    u16 item;                       /* 0x02 an item (item_counts) */
    u16 hp_heal;                    /* 0x04 added to PlayerState.hp_now when a type 0 one is used */
    u16 mp_heal;                    /* 0x06 added to PlayerState.mp_now */
    u16 timed_flags;                /* 0x08 bit n set: starts PlayerState.timed_flags[n] for 60 frames */
    char pad0A[0x04];
    u16 flags;                      /* 0x0E bit 1 */
    char pad10[0x10];
} ItemCommonDef;

/* ItemCommonDef.flags as bit-fields, where retail reads the low byte alone (Menu_BuildItemRings) */
typedef struct {
    char pad00[0x0E];
    u8 unlisted : 1;                /* 0x0E set: the item is left out of the menu's list of things to use */
    u8 bit1 : 1;
    u8 bits2 : 6;
    char pad0F[0x11];
} ItemCommonDefBits;

#define ITEM_COMMON_DEF_BITS(def) ((ItemCommonDefBits *)(def))

/* map_actors[map slot][16] in item/gold.c: a map actor (0x2C bytes); flags bit 2 is the map slot */
typedef struct MapActor {
    u32 flags;                      /* 0x00 bit 0: in use, bit 1: near enough to pick up, bit 2: the map slot; the high half is MapActorWords.amount */
    s8 step;                        /* 0x04 0: fading in, 1: lying there, 2: fading out */
    u8 fade_timer;                  /* 0x05 frames left of the fade */
    char pad06[0x02];
    f32 unk08;                      /* 0x08 D_004469DC when it was dropped */
    u32 serial;                     /* 0x0C the slot's count of drops when it was dropped: the lowest is the oldest */
    ModelInstance *body;            /* 0x10 */
    f32 position_y;                 /* 0x14 the y position */
    struct MapActor *prev;          /* 0x18 */
    struct MapActor *next;          /* 0x1C the list of the ones in use starts at map_actors_in_use */
    char pad20[0x04];
    u8 unk24;                       /* 0x24 */
    char pad25[0x07];
} MapActor;

/* a MapActor where the code reads the high half of flags and the word at 0x24 whole */
typedef struct {
    u16 flags_low;                  /* 0x00 */
    u16 amount;                     /* 0x02 how much gold it is worth */
    char pad04[0x20];
    s32 object_id;                  /* 0x24 the map object that dropped it (MapObject_MarkEmptiedById), 0: none */
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

/* map_object_marks[MapSlot.tag][16]: where a map object is marked (0xA bytes) */
typedef struct {
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    char radius;                    /* 0x06 */
    u8 object_id;                   /* 0x07 the map object the mark belongs to, passed to Gold_Drop */
    u16 amount;                     /* 0x08 the amount of gold, passed to Gold_Drop */
} ObjectMark;

/* a map's 16 marks, as Gold_InitMap copies them whole */
typedef struct {
    ObjectMark marks[16];
} ObjectMarkRow;

#endif
