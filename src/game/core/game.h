/* Game-wide state blocks. */
#ifndef KF_GAME_H
#define KF_GAME_H

#include "common.h"
#include "gfx/gs.h"
#include "model/model.h"
#include "math/camera.h"

/* game_status (cleared as 0x48 bytes by Map_ResetGameState) */
typedef struct {
    char player_control;            /* 0x00 nonzero: script blends are skipped */
    char set_on_map_reset;          /* 0x01 Flag set to 1 after the map state reset */
    char paused;                    /* 0x02 1 while GameLoop_UpdatePause holds the game */
    s8 camera_mode;                 /* 0x03 CAMERA_MODE_ */
    u8 camera_look_mode;            /* 0x04 CAMERA_LOOK_ */
    s8 camera_move_enabled;         /* 0x05 nonzero: the camera moves to CameraState.move_to_point */
    char player_run_disabled;       /* 0x06 nonzero: the player cannot run (Player_Update) */
    char movement_input_enabled;    /* 0x07 0: the player's move buttons are ignored */
    char pad08[0x18];
    int item_to_use;                /* 0x20 the item to use (Player_UseItem), with bit6 */
    u32 bit0 : 1;                   /* 0x24 */
    u32 using_faced_object : 1;     /* the player is using the object they face (faced_object): the use button (Player_Update) */
    u32 bit2 : 1;
    u32 bit3 : 1;
    u32 bit4 : 1;
    u32 save_requested : 1;         /* the save screen runs next (GameLoop_OpenRequestedMenu; MapObject_UpdateSavePoint) */
    u32 bit6 : 1;
    u32 bit7 : 1;
    u32 bit8 : 1;
    u32 bit9 : 1;                   /* set to leave the main loop (GameLoop_Run) */
    u32 bits10 : 2;
    u32 bit12 : 1;                  /* bits 12 and 13 both set: the main loop skips drawing and input */
    u32 bit13 : 1;
    u32 bit14 : 1;
    int magic_to_cast;              /* 0x28 the spell the magic menu asked for, cast after it */
    int trade_screen_id;            /* 0x2C the trade screen ScriptCmd_OpenTrade asked for */
    int update_hold_flags;          /* 0x30 bits hold off subsystems; bit 0 takes the player's control */
    int save_area;                  /* 0x34 the message id of the area name the save list shows (MapObject_UpdateSavePoint) */
    char pad38[0x08];
    void *effect_owner;             /* 0x40 the model of the item effect that asked for update_hold_flags = 0xC5F (ItemEffect_StepRequestGameStatus) */
    int item_use_cooldown;          /* 0x44 counts down to 0; 1 when an item was just used (Player_UseItem) */
} GameStatus;

/* GameStatus.camera_mode */
#define CAMERA_MODE_FOLLOW_PLAYER   0       /* the camera follows the player */
#define CAMERA_MODE_SCRIPTED        1       /* the script places the camera */

/* GameStatus.camera_look_mode: no look at a point, turn at once to it, or ease toward it */
#define CAMERA_LOOK_NONE            0
#define CAMERA_LOOK_SNAP            1
#define CAMERA_LOOK_EASE            2

/* GameStatus with bits 10 and 11 of the flag word (GameStatus.bits10) apart, as ScriptCmd_ExitMainLoop
   sets them */
typedef struct {
    char pad00[0x24];
    u32 bits0 : 9;                  /* 0x24 */
    u32 bit9 : 1;
    u32 bit10 : 1;
    u32 bit11 : 1;
    u32 bits12 : 20;
} GameStatusSplitBits;

#define GAME_STATUS_SPLIT_BITS(status) ((GameStatusSplitBits *)(status))

/* the flag word of GameStatus (0x24) whole, where retail changes two of its bits at once */
typedef struct {
    char pad00[0x24];
    s32 flags;                      /* 0x24 */
} GameStatusFlagWord;

#define GAME_STATUS_FLAG_WORD(status) (((GameStatusFlagWord *)(status))->flags)

/* GameStatus bit 12: the main loop holds its frame at the next START press (ScriptCmd_HoldFrameOnStart) */
#define GAME_STATUS_FLAG_HOLD_ARMED 0x1000
/* GameStatus bit 13: set by the main loop when START is pressed while held; it then skips drawing and input */
#define GAME_STATUS_FLAG_HOLDING    0x2000
/* GameStatus.update_hold_flags bits: 0x40 holds the scripts (Script_UpdateAll), 0x400 updates only the effect owned by effect_owner */
#define UPDATE_HOLD_SCRIPTS            0x40
#define UPDATE_HOLD_EFFECT_OWNER_ONLY  0x400
/* game_status_flags, the flag word of GameStatus (0x24), as bit-fields */
typedef struct {
    u32 pad0 : 1;
    u32 using_faced_object : 1;     /* the player is using the object they face (faced_object): the use button (Player_Update) */
} GameStatusFlagBits;

#define GAME_STATUS_FLAG_BITS(flags) ((GameStatusFlagBits *)(flags))

#define UPDATE_HOLD_MAP_OBJECTS 0x8        /* holds back MapObject_UpdateAll, the map objects' update */

/* an 8-byte time passed by value: the wall clock (PlayTime_DayOfYear, PlayTime_ClockSecondsBetween), and the play time
   current_play_time, which PlayTime_ComputeCurrent splits the same way (mday counts hundreds of hours); the record
   sceCdReadClock fills in */
typedef struct sceCdCLOCK {
    char pad0;                      /* 0x00 */
    u8 sec;                         /* 0x01 */
    u8 min;                         /* 0x02 */
    u8 hour;                        /* 0x03 */
    char pad4;                      /* 0x04 */
    u8 mday;                        /* 0x05 */
    u8 mon;                         /* 0x06 */
    u8 year;                        /* 0x07 years since 2000 */
} ClockTime;

/* the first 8 bytes of the save buffer save_image (Game_WriteSaveImage writes them, Game_CheckSaveImage checks them) */
typedef struct {
    s32 magic;                      /* 0x00 SAVE_MAGIC */
    s32 size;                       /* 0x04 bytes of state, header included, rounded up to SAVE_BLOCK_SIZE; one checksum
                                       byte for each block of that size follows */
} SaveHeader;

#define SAVE_MAGIC      0x15400
#define SAVE_BLOCK_SIZE 0x80

/* view_ray_status: the view ray from the player's eyes and what it finds in front of him (Interact_UpdateViewRay,
   Interact_FindTarget), and the event being run */
typedef struct {
    f32 view_origin[4];             /* 0x00 a position the camera is put in front of (GameLoop_UpdateCamera) */
    f32 view_direction[4];          /* 0x10 a direction from it */
    f32 prev_view_origin[4];        /* 0x20 view_origin as it was the frame before (Interact_UpdateViewRay) */
    f32 prev_view_direction[4];     /* 0x30 view_direction as it was the frame before */
    f32 view_distance;              /* 0x40 how far along view_direction */
    f32 prev_view_distance;         /* 0x44 view_distance as it was the frame before */
    f32 nearest_entity_distance;    /* 0x48 distance to the nearest entity around the player (Interact_FindTarget) */
    f32 target_distance;            /* 0x4C distance to what `current`, facing_entity or facing_item names */
    void *current;                  /* 0x50 the event being run */
    void *facing_entity;            /* 0x54 an Entity whose spawn info can answer the use of an item (Player_UseItem) */
    void *facing_item;              /* 0x58 the ItemNode (model/model.h) in front of the player, NULL: none (Interact_FindTarget) */
} EventRunner;

/* an event MapObject_OpenChestOnSignal hands to MapObject_UpdateClosedChest */
typedef struct {
    char pad00[0x10];
    u32 flags;                      /* 0x10 */
} GameEvent;

/* the viewers MapViewer_Run runs, indexed by picked_viewer (ui/map_viewer.c), 0xC bytes each */
typedef struct {
    void (*run)(void);              /* 0x00 loops until the viewer is left */
    int (*locate_player_fn)(int *x, int *y); /* 0x04 the function that finds the player's marker on the picture */
    int count;                      /* 0x08 the number of pictures; viewer_picture stays below it */
} ViewerEntry;

/* four texture coordinates, one per corner of the viewer's player marker: a local table MapViewer_DrawPictureMap
   copies whole from read-only data (viewer_marker_corner_us, viewer_marker_corner_vs) */
typedef struct {
    s32 values[4];                  /* 0x00 */
} ViewerCornerTable;

/* game_clock: the game clock (core/game_clock.c) */
typedef struct {
    char pad00[0x08];
    int ticks;                      /* 0x08 */
} GameClock;

/* game_clock_control: what decides whether GameClock_Tick advances GameClock.ticks */
typedef struct {
    char pad00[0x208];
    char startup_flag_a;            /* 0x208 Byte cleared at startup, never read */
    char startup_flag_b;            /* 0x209 Byte cleared at startup, never read */
    char startup_flag_c;            /* 0x20A Byte cleared at startup, never read */
    char startup_level;             /* 0x20B Byte set to 2 at startup, never read */
    char pad20C[0x7];
    char clock_stopped;             /* 0x213 nonzero: the clock stops */
    char tick_held;                 /* 0x214 nonzero: the clock stops */
    char pad215[0x73];
    int flags;                      /* 0x288 0x200: tick every fourth frame */
} ClockControl;

/* ClockControl.flags: the clock ticks only every fourth frame */
#define CLOCK_CONTROL_FLAG_QUARTER_RATE 0x200
/* game_clock_speed_flags: the clock advances 60 ticks a frame (fast forward) */
#define CLOCK_SPEED_FLAG_FAST_FORWARD   0x400

/* one corner of a character's sprite as TextLog_PutChar queues it: UV, RGBAQ and XYZ2 for the GS */
typedef struct {
    int tex_u;                      /* 0x00 in 1/16 texel */
    int tex_v;                      /* 0x04 */
    char pad08[0x08];
    int red;                        /* 0x10 */
    int green;                      /* 0x14 */
    int blue;                       /* 0x18 */
    int alpha;                      /* 0x1C */
    int pos_x;                      /* 0x20 in 1/16 pixel */
    int pos_y;                      /* 0x24 */
    int pos_z;                      /* 0x28 */
    char pad2C[0x04];
} TextLogVertex;

/* one character: a sprite of 16 by 8 pixels (0x60 bytes) */
typedef struct {
    TextLogVertex corners[2];       /* 0x00 top left, bottom right */
} TextLogGlyph;

/* text_log_packet: the GIF packet TextLog_Flush sends, a tag and room for 37 characters */
typedef struct {
    int tag[4];                     /* 0x00 */
    TextLogGlyph glyphs[37];        /* 0x10 */
} TextLogPacket;

/* text_log: the 0x800-byte text buffer TextLog_AppendV appends to, and the window TextLog_Flush prints it in */
typedef struct {
    TextLogGlyph *packet;           /* 0x00 where the next character goes in text_log_packet */
    char pad04[0x0C];
    int left;                       /* 0x10 the window, in GS coordinates (TextLog_SetWindow) */
    int top;                        /* 0x14 */
    int width;                      /* 0x18 */
    int height;                     /* 0x1C */
    int depth;                      /* 0x20 */
    char pad24[0x0C];
    int red;                        /* 0x30 text colour */
    int green;                      /* 0x34 */
    int blue;                       /* 0x38 */
    char pad3C[0x04];
    int font_base;                  /* 0x40 GS page of the font texture (TextLog_LoadFont) */
    int palette_base;               /* 0x44 GS page of its palette */
    int font_buffer_width;          /* 0x48 in units of 64 pixels */
    int used;                       /* 0x4C bytes of text */
    char text[0x800];               /* 0x50 */
} TextLog;

/* item_name_defs[ITEM_COUNT]: param/item_nam.prm, read by Map_LoadParamFiles: a row per item, starting with its
   name (the items carried, then 70 weapons from ITEM_WEAPON_FIRST, then 160 worn ones from ITEM_WORN_FIRST) */
typedef struct {
    char pad00[0x30];
    u16 model_file_number;          /* 0x30 the number in the name of its item_arm file (Player_LoadArmModels) */
    char pad32[0x01];
    u8 weight_tenths_of_kilogram;   /* 0x33 the item's weight in tenths of a kilogram */
    char pad34[0x02];
    u8 upgrade_wait_minutes;        /* 0x36 minutes that must pass since LevelRecord.last_upgraded (Smith_BuildItemRings) */
    u8 exp_per_level;               /* 0x37 wear (experience) that costs a weapon or worn item a level (Attack_WearWeapon) */
    u32 unk38;                      /* 0x38 */
    u8 starting_level_percent;      /* 0x3C the percent a level record's level starts at for it */
    char pad3D[0x01];
    u16 weight_tenths_of_pound;     /* 0x3E weight_tenths_of_kilogram scaled by kilograms_to_pounds_factor, worked out after loading */
} GameDefinition;

/* a GameDefinition as the menu code reads it: the record is word-aligned (LevelDef in player/player.h
   shows the word at 0x30 is bit-fields), and its low 16 bits are the number in the model's file name */
typedef struct {
    char pad00[0x30];
    u32 file_number : 16;           /* 0x30 */
    u32 pad30_16 : 16;
    char pad34[0x0C];
} GameDefinitionWords;

/* a GameDefinition as the save code reads it (Game_WriteSaveImage, Game_ReadSaveImage): unk33 is the top byte of flags */
typedef struct {
    char pad00[0x30];
    u32 flags;                      /* 0x30 bits 18 and 19 are a state; below 2 it is saved as one bit */
    char pad34[0x0C];
} GameDefinitionFlags;

/* GameDefinitionFlags bits 18 and 19: the item's state (below 2, it is saved as one bit) */
#define GAME_DEFINITION_STATE_MASK 0xC0000
#define GAME_DEFINITION_STATE_SHIFT 18
#define GAME_DEFINITION_STATE_VALUE_MASK 3  /* the state once shifted down to bit 0 */

/* map_entry_counters: each byte holds two 4-bit death counters, the even spawn record in the low nibble */
#define SPAWN_COUNTER_LOW_MASK  0xF
#define SPAWN_COUNTER_HIGH_MASK 0xF0
#define SPAWN_COUNTER_HIGH_SHIFT 4

/* item_counts and the five tables after it: one byte per GameDefinition */
typedef struct {
    u8 bytes[380];
} DefinitionByteSet;

/* RecollectionFlags.bits[0]: the number of the description shown (bits 0-2) */
#define RECOLLECTION_DESCRIPTION_MASK 0x7

/* recollection_flags[50]: one page of the menu's Recollection screen (messages MSG_RECOLLECTION_FIRST + 50 * page
   on): bits 0-2 the description shown, then a bit per message heard */
typedef struct {
    u8 bits[8];
} RecollectionFlags;

/* a word read from any alignment (the save image header) */
typedef struct {
    int value;
} __attribute__((packed)) UnalignedInt;

/* haze_layer_x_offsets: the x offsets Game_DrawHazeLayersShifted cycles its sprites through */
typedef struct {
    int x[3];
} LayerOffsets;

/* wobble_grid_origin, wobble_grid_points, wobble_grid_previous: an 8 x 11 grid of points Game_DrawWobblingScreen wobbles */
typedef struct {
    int points[8][11][2];
} __attribute__((aligned(16))) RippleGrid;

/* a node of the doubly linked list at lying_items */
typedef struct ListNode {
    char pad00[0x20];
    struct ListNode *prev;          /* 0x20 */
    struct ListNode *next;          /* 0x24 */
} ListNode;

/* pool_slots[32]: a pool of 0x70-byte slots chained into a free list through `next`;
   pool_slot_free_list holds the head */
typedef struct PoolSlot {
    char pad00[0x40];
    struct PoolSlot *parent;        /* 0x40 the parent slot, whose matrix this slot's model follows */
    struct PoolPart *part;          /* 0x44 the 0x50-byte part this slot shows, NULL: none */
    struct PoolPartRecord *part_record; /* 0x48 the record of the part this slot shows; NULL: no parent */
    char pad4C[0x04];
    struct PoolSlot *next;          /* 0x50 */
    s32 key_index;                  /* 0x54 the key of its node's animation that PoolNode_Pose is at */
    u64 tex0;                       /* 0x58 GS TEX0 copied from the texture header */
    u64 tex1;                       /* 0x60 GS TEX1 copied from the texture header */
    char pad68[0x08];
} PoolSlot;

/* a texture header's two GS register values (TextureHeader.tex0 and unk18), which PoolSlot_LoadGroup copies as one */
typedef struct {
    u64 tex0;                       /* 0x00 */
    u64 pad08;                      /* 0x08 */
} PoolSlotRegisters;

/* a PoolSlot with tex0 and unk60 as the one member they are copied as */
typedef struct {
    char pad00[0x58];
    PoolSlotRegisters registers;    /* 0x58 */
    char pad68[0x08];
} PoolSlotRegistersView;

#define POOL_SLOT_REGISTERS(slot) (((PoolSlotRegistersView *)(slot))->registers)

/* one 0x10-byte record of a RelocEntry, with two file offsets that PoolNodeFile_Relocate turns into pointers */
typedef struct {
    char pad00[0x08];
    void *layer_data;               /* 0x08 Pointer made from a file offset at load */
    void *layer_aux_data;           /* 0x0C Second pointer made from a file offset */
} RelocLayer;

/* one 0x40-byte entry of a RelocFile */
typedef struct {
    u16 key_count;                  /* 0x00 */
    u16 layer_count;                /* 0x02 */
    u16 slot;                       /* 0x04 the PoolSlot the node poses (PoolNode_Pose) */
    char pad06[0x2A];
    void *keys;                     /* 0x30 the animation keys, 0x80 bytes each */
    RelocLayer *layers;             /* 0x34 */
    void *child;                    /* 0x38 the child node, posed by this node's slot */
    void *sibling;                  /* 0x3C the next sibling node, posed by the parent matrix */
} RelocEntry;

/* a file whose offsets PoolNodeFile_Relocate turns into pointers: a 0x10-byte header, then the entries */
typedef struct {
    char pad00[0x08];
    u32 entry_count;                /* 0x08 */
    char pad0C[0x04];
    RelocEntry entries[1];          /* 0x10 */
} RelocFile;

/* lists of pool slot numbers that PoolSlot_SetGroupParts and PoolSlot_LoadGroup copy whole from read-only data, and
   their tables of counts */
typedef struct {
    u8 slots[7];                    /* 0x00 */
} PoolSlotList7;

typedef struct {
    u8 slots[4];                    /* 0x00 */
} PoolSlotList4;

typedef struct {
    u8 slots[2];                    /* 0x00 */
} PoolSlotList2;

typedef struct {
    u8 slots[6];                    /* 0x00 */
} PoolSlotList6;

typedef struct {
    u32 counts[6];                  /* 0x00 */
} PoolSlotCounts6;

typedef struct {
    u32 counts[10];                 /* 0x00 */
} PoolSlotCounts10;

typedef struct {
    u32 values[11];                 /* 0x00 */
} PoolSlotValues11;

/* one 0x20-byte point of a PoolPartMaterial's strip */
typedef struct {
    u16 point;                      /* 0x00 which of the part's points */
    u16 normal;                     /* 0x02 which of its normals */
    char pad04[0x04];
    f32 tex_s;                      /* 0x08 */
    f32 tex_t;                      /* 0x0C */
    f32 color[4];                   /* 0x10 */
} PoolPartPoint;

/* what a PoolPart draws with: the first of its strips; each strip's points follow it, then the next strip */
typedef struct {
    u16 point_count;                /* 0x00 */
    char pad02[0x02];
    u16 alpha_2;                    /* 0x04 the value of GS ALPHA_2 */
    u16 prim;                       /* 0x06 GS PRIM bits */
    u64 tex0;                       /* 0x08 GS TEX0, set from the texture header's before the part is drawn */
    u64 second_tex0;                /* 0x10 GS TEX0 of a second texture drawn over the first (PoolPart_DrawTinted), 0: none */
    char pad18[0x08];
    PoolPartPoint points[1];        /* 0x20 point_count of them */
} PoolPartMaterial;

/* PoolPartMaterial.tex0 with the CLUT load control of GS TEX0 as a bit-field */
typedef struct {
    char pad00[0x08];
    u64 pad08 : 61;
    u64 cld : 3;                    /* bit 61 */
} PoolPartMaterialBits;

/* the scratchpad while a PoolPart is drawn (PoolPart_DrawTinted, PoolPart_Draw, PoolSlot_Draw) */
typedef struct {
    u32 prim_tag[4];                /* 0x00 the GIF tag in front of a strip's points */
    u32 reg_tag[4];                 /* 0x10 the GIF tag in front of the registers set for a strip */
    s32 packet_size;                /* 0x20 quadwords */
    char pad24[0x4C];
    s32 point;                      /* 0x70 */
    u32 strip;                      /* 0x74 */
    u32 point_count;                /* 0x78 */
    u32 normal_count;               /* 0x7C */
    u32 strip_count;                /* 0x80 */
    char pad84[0x1C];
    f32 unity_color_scale[4];       /* 0xA0 Four floats of 128, the colour scale of 1.0 */
    f32 screen[16];                 /* 0xB0 the part's points to the screen */
    f32 light[16];                  /* 0xF0 */
    f32 world[16];                  /* 0x130 the part in the world */
    u64 tex0;                       /* 0x170 */
    PoolPartMaterial *strip_start;  /* 0x178 */
    u64 *registers;                 /* 0x17C */
    u32 *packet;                    /* 0x180 */
    PoolPartPoint *strip_point;     /* 0x184 */
    PoolPartMaterial *strip_at;     /* 0x188 */
    u32 *cursor;                    /* 0x18C */
    f32 (*screen_points)[4];        /* 0x190 the part's points on the screen, 12.4 fixed point */
    f32 (*work)[4];                 /* 0x194 part_work_buffer */
    f32 (*normals)[4];              /* 0x198 the part's normals in the world */
    char pad19C[0x04];
    s32 default_point_color[4];     /* 0x1A0 Quadword sent as each point's colour */
    u32 parent_point_count;         /* 0x1B0 PoolSlot_Draw: how many points go by the parent slot's matrix */
    u32 point_at;                   /* 0x1B4 */
    u32 point_index;                /* 0x1B8 */
    u32 parent_normal_count;        /* 0x1BC PoolSlot_Draw: how many normals go by the parent slot's matrix */
    u32 normal;                     /* 0x1C0 */
    u32 normal_index;               /* 0x1C4 */
} PoolDrawScratch;

#define POOL_DRAW_SCRATCH ((PoolDrawScratch *)0x70000000)

/* PoolDrawScratch.cursor as a union (PoolPart_DrawTinted advances it through this view once per point) */
typedef struct {
    char pad000[0x18C];
    union {
        u32 *words;                 /* 0x18C */
        u8 *bytes;
    } cursor;
} PoolDrawScratchCursorView;

#define POOL_DRAW_SCRATCH_CURSOR(scratch) (((PoolDrawScratchCursorView *)(scratch))->cursor)

/* the two places in GS memory that MenuModel_DrawTinted and MenuModel_Draw pick a palette's from (their local clut_pages) */
typedef struct {
    u64 pages[2];                   /* 0x00 */
} PoolClutPages;

/* one 0x80-byte animation key of a RelocEntry (its keys), interpolated towards the next one by PoolNode_Pose */
typedef struct {
    u16 frame;                      /* 0x00 the frame the key starts at */
    u16 length;                     /* 0x02 frames to the next key */
    char pad04[0x0C];
    f32 translation[4];             /* 0x10 */
    f32 translation_in[4];          /* 0x20 tangent arriving at this key */
    f32 translation_out[4];         /* 0x30 tangent leaving it */
    f32 rotation[4];                /* 0x40 angles about x, y and z */
    f32 rotation_in[4];             /* 0x50 */
    f32 rotation_out[4];            /* 0x60 */
    f32 scale[4];                   /* 0x70 */
} PoolNodeKey;

/* one 0x50-byte part of a PoolPartFile */
typedef struct PoolPart {
    f32 position[4];                /* 0x00 */
    f32 rotation[4];                /* 0x10 angles about x, y and z */
    f32 scale[4];                   /* 0x20 */
    u16 strip_count;                /* 0x30 */
    char pad32[0x02];
    PoolPartMaterial *material;     /* 0x34 */
    u16 point_count;                /* 0x38 */
    char pad3A[0x02];
    f32 (*points)[4];               /* 0x3C */
    u16 normal_count;               /* 0x40 */
    char pad42[0x02];
    f32 (*normals)[4];              /* 0x44 */
    char pad48[0x08];
} PoolPart;

/* one 0x10-byte record of a PoolPartFile; the records end records_end_offset bytes into the file, the
   last one belonging to the last part */
typedef struct PoolPartRecord {
    s32 parent_point_count;         /* 0x00 above 0: the part has a parent slot and this many points follow its matrix */
    s32 parent_normal_count;        /* 0x04 how many of the part's normals go by the parent slot's matrix */
    s32 parent_slot_index;          /* 0x08 index, in the group's slot list, of the parent slot */
    char pad0C[0x04];
} PoolPartRecord;

/* a part file as loaded: a 0x10-byte header, then the parts */
typedef struct {
    PoolPartRecord *records_end;    /* 0x00 the end of its PoolPartRecord array, as an offset from the file's start */
    char pad04[0x04];
    u32 part_count;                 /* 0x08 */
    char pad0C[0x04];
    PoolPart parts[1];              /* 0x10 */
} PoolPartFile;

/* the 0x10 bytes in front of a PoolPartFile's parts, copied as one (PoolPartFile_Expand) */
typedef struct {
    PoolPartRecord *records_end;    /* 0x00 */
    char pad04[0x04];
    u32 part_count;                 /* 0x08 */
    char pad0C[0x04];
} PoolPartFileHeader;

/* the 0x20 bytes in front of a strip's points as the four words PoolPartFile_Expand copies them as */
typedef union {
    u64 words[4];                   /* 0x00 */
    char bytes[0x20];               /* 0x00 */
} PoolPartStripWords;

/* one 0x40-byte point of a strip in the copy PoolPartFile_Expand makes: the point and normal themselves
   instead of their numbers */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 normal[4];                  /* 0x10 */
    f32 tex_s;                      /* 0x20 */
    f32 tex_t;                      /* 0x24 */
    f32 tex_q;                      /* 0x28 1.0 */
    f32 tex_w;                      /* 0x2C Fourth float of the texture quad, always 0 */
    f32 color[4];                   /* 0x30 */
} PoolPartFlatPoint;

/* a strip of that copy: the same 0x20 bytes as a PoolPartMaterial's, then 0x40-byte points */
typedef struct {
    u16 point_count;                /* 0x00 */
    char pad02[0x1E];
    PoolPartFlatPoint points[1];    /* 0x20 */
} PoolPartFlatStrip;

/* what PoolSlot_LoadGroup is handed: a part file and the texture its parts are drawn with */
typedef struct {
    PoolPartFile *file;             /* 0x00 */
    void *texture;                  /* 0x04 a TextureHeader (gfx/texture.h) */
} PoolPartSource;

/* pad_states[2]: a controller as the game reads it, one per port (Input_ReadPort). pad_state is the copy
   of port 0 that the game tests. */
typedef struct {
    u16 held;                       /* 0x00 buttons held, the stick directions among them */
    u16 pressed;                    /* 0x02 buttons that went down this frame */
    s32 type;                       /* 0x04 what Pad_GetMode says: 1 has sticks, 0xFF is no controller */
    u16 look_buttons;               /* 0x08 the right stick's up and down as the buttons 0x200 and 0x100 */
    u16 prev_look_buttons;          /* 0x0A */
    u16 move_buttons;               /* 0x0C the left stick as the direction buttons (0x80, 0x20, 0x10, 0x40) */
    u16 prev_move_buttons;          /* 0x0E */
} PadState;

/* the texts of the debug start menu (Debug_RunStartMenu), local arrays it copies whole from
   read-only data: blank lines above the text, the PUSH START line, the four menu lines */
typedef struct {
    char text[0x1A];
} DebugStartMenuSpacer;

typedef struct {
    char text[0x46];
} DebugStartMenuPrompt;

typedef struct {
    char text[4][0x80];
} DebugStartMenuItems;

/* what Geometry_IndexOfLargestOfSeven is given; nothing calls that function, so what this is stays unknown */
typedef struct {
    char pad00[0x70];
    s16 *largest_search_values;     /* 0x70 Seven s16 values searched for the largest */
} Unk20D008Owner;

/* core/game.c */
void Game_SetDefaultOptions(void);
void Game_DrawWobblingScreen(int red, int green, int blue, int alpha, float blend);
void Game_ResetProgress(void);
void Game_NormalizeStartState(void);
int Game_IsMapVisited(int flag);
void Game_MarkMapVisited(int flag);
int Game_GetSpawnDeathCount(int map, int entry);
void Game_CountSpawnDeath(u32 map, int entry);
void Game_SetObjectFlag(int map, int entry, u32 bit, int value);
int Game_GetObjectFlag(int map, int entry, u32 bit);
int Game_GetObjectActive(int map, int entry);
void Game_SetObjectActive(int map, int entry, int value);
int Game_GetObjectState(int map, int entry);
void Game_SetObjectState(int map, int entry, int value);
void Game_MarkMessageHeard(int flag);
void Game_MoveRecollectionPage(u32 source_page, u32 dest_page);
int Game_FindTriggerOfItem(int item, int *map_tag, int *trigger_id);
int Game_GetTriggerItem(int map_tag, int trigger_id);
int Game_ClearTriggerItem(int map_tag, int trigger_id);
int Game_AddTriggerItem(int map_tag, int trigger_id, int item);
void Game_WriteSaveImage(void);
void Game_ReadSaveImage(void);
int Game_CheckSaveImage(void);
void Game_StartFadeIn(int red, int green, int blue, int frames);
void Game_StepFade(void);
void Game_DrawFade(void);
void Game_ProjectHazeLayers(void);
void Game_DrawHazeLayers(void);
void Game_DrawDepthTestSprite(void);
void Game_BlendScreenOffsetDown(void);
void Game_BlendScreenOffsetUpDown(void);
void Game_BlendPreviousFrame(int alpha);
void Game_ShiftScreenByViewTurn(void);
void Game_ApplyBrightness(int brightness);
void Game_TintScreen(int red, int green, int blue);

/* core/game_loop.c */
void GameLoop_Run(int arg);
void GameLoop_RunSoundLoads(void);
void GameLoop_DrawFrame(int arg);
void GameLoop_ResetSystems(int arg);
void GameLoop_CancelMenuRequests(void);
void GameLoop_UpdateCameraShake(void);
void GameLoop_UpdateCamera(void);
void GameLoop_SetCameraMatrices(int projection, int mode);
void GameLoop_OpenRequestedMenu(void);
void GameLoop_UpdatePause(void);
void GameLoop_DrawFrameWithoutHud(void);
void GameLoop_ShowGrabbedFrame(int divisor);
void GameLoop_RedrawFrame(void);
void GameLoop_ClearBothBuffers(void);

/* core/game_state.c */
extern u8 player_can_walk[];
extern u32 game_status_flags[];
extern int suspended_updates[3];
extern s32 save_area_name_id[];

/* data still in asm */
extern float view_ray_previous_direction[];
extern float view_ray_length[];
extern s64 game_dbuff_clear_color[];
extern s64 game_dbuff_context2_clear_color[];
extern const DefinitionByteSet initial_trade_stock_1;
extern const DefinitionByteSet initial_trade_stock_2;
extern const DefinitionByteSet initial_trade_stock_3;
extern const DefinitionByteSet initial_trade_stock_4;
extern const DefinitionByteSet initial_trade_stock_5;
extern GsColor haze_sprite_color;
extern LayerOffsets haze_layer_x_offsets;
extern char z_value_format[];
extern ScreenRect full_screen_rect;
extern ScreenRect inset_screen_rect;
extern int message_dim_alpha;
extern int save_loaded;  /* nonzero: a saved game was loaded */
extern s32 saved_map_tag;
extern int depth_test_sprite_z;
extern int wobble_grid_reset;
extern u32 haze_layer_count;
/* the pad bits of each control */
extern u16 forward_button;  /* forward */
extern u16 back_button;  /* back */
extern u16 turn_right_button;  /* turn right */
extern u16 turn_left_button;  /* turn left */
extern u16 step_right_button;  /* step right */
extern u16 step_left_button;  /* step left */
extern u16 look_down_button;  /* look down */
extern u16 look_up_button;  /* look up */
extern u16 attack_button;
extern u16 magic_button;
extern u16 action_button;  /* run; pressed: use what is in front */
extern u16 menu_button;
extern u16 shortcut_button;
extern u16 pause_button;
extern s32 item_drop_counter;
extern int game_clock_speed_flags;
extern int context1_buffer_half;  /* the half of the first double buffer in use; each swap flips it */
extern int fade_frames_left;
extern int fade_frames;

void Map_UpdateDrawState(void);

/* core/game_state.c */
extern GameStatus game_status;
extern MapProp *map_slot_props[2][0xFF];

/* data still in asm */
extern ClockTime current_play_time;
extern s32 key_item_slots[12];

#endif
