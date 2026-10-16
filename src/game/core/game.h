/* Game-wide state blocks. */
#ifndef KF_GAME_H
#define KF_GAME_H

#include "common.h"

/* game_status (cleared as 0x48 bytes by Map_ResetGameState). Other files still
   declare it as char[]/int[], so the extern stays in each file. */
typedef struct {
    char player_control;            /* 0x00 nonzero: script blends are skipped */
    char unk01;                     /* 0x01 */
    char paused;                    /* 0x02 1 while GameLoop_UpdatePause holds the game */
    s8 camera_mode;                 /* 0x03 0 the camera follows the player, 1 the script places it */
    u8 camera_look_mode;            /* 0x04 0 or 1 */
    s8 camera_move_enabled;         /* 0x05 nonzero: the camera moves to CameraState.move_to_point */
    char player_run_disabled;       /* 0x06 nonzero: the player cannot run (Player_Update) */
    char movement_input_enabled;    /* 0x07 0: the player's move buttons are ignored */
    char pad08[0x18];
    int item_to_use;                /* 0x20 the item to use (Player_UseItem), with bit6 */
    u32 bit0 : 1;                   /* 0x24 */
    u32 bit1 : 1;                   /* set while MapObject_UpdateClosedChest runs an event */
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
    void *effect_owner;             /* 0x40 the model of the item effect that asked for unk30 = 0xC5F (ItemEffect_StepRequestGameStatus) */
    int item_use_cooldown;          /* 0x44 counts down to 0; 1 when an item was just used (Player_UseItem) */
} GameStatus;

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

/* current_play_time: the play time as PlayTime_ComputeCurrent splits it (byte 1 seconds, 2 minutes, 3 and 5 hours), passed by value */
typedef struct {
    char bytes[8];
} PlayTime;

/* the first 8 bytes of the save buffer save_image (Game_WriteSaveImage writes them, Game_CheckSaveImage checks them) */
typedef struct {
    s32 magic;                      /* 0x00 SAVE_MAGIC */
    s32 size;                       /* 0x04 bytes of state, header included, rounded up to SAVE_BLOCK_SIZE; one checksum
                                       byte for each block of that size follows */
} SaveHeader;

#define SAVE_MAGIC      0x15400
#define SAVE_BLOCK_SIZE 0x80

/* camera_state (the camera, it seems). Other files still declare it as float[],
   so the extern stays in each file. */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 pitch;                      /* 0x10 rotation about x */
    f32 yaw;                        /* 0x14 rotation about y */
    f32 roll;                       /* 0x18 rotation about z */
    char pad1C[0x04];
    f32 look_at_point[4];           /* 0x20 the point to look at (GameStatus.camera_look_mode) */
    f32 move_to_point[4];           /* 0x30 the point to move to (GameStatus.camera_move_enabled) */
    f32 turn_speed;                 /* 0x40 how fast it turns to unk20 */
    f32 move_speed;                 /* 0x44 how far it moves to unk30 in a frame */
    f32 screen_scale;               /* 0x48 512.0 after Map_ResetGameState */
    f32 height_offset;              /* 0x4C added to the height above the player */
    f32 script_shake_offset[4];     /* 0x50 the first shake's offset (GameLoop_UpdateCameraShake) */
    f32 entity_shake_offset[4];     /* 0x60 the second shake's offset */
    f32 script_shake_reach;         /* 0x70 how far the first shake reaches */
    s32 script_shake_frames;        /* 0x74 frames of it left */
    f32 entity_shake_reach;         /* 0x78 how far the second shake reaches (EntityAi_ShakeCamera) */
    s32 entity_shake_frames;        /* 0x7C frames of it left */
} CameraState;

/* view_ray_status: the view ray from the player's eyes and what it finds in front of him (Interact_UpdateViewRay,
   Interact_FindTarget), and the event being run */
typedef struct {
    f32 view_origin[4];             /* 0x00 a position the camera is put in front of (GameLoop_UpdateCamera) */
    f32 view_direction[4];          /* 0x10 a direction from it */
    f32 prev_view_origin[4];        /* 0x20 unk00 as it was the frame before (Interact_UpdateViewRay) */
    f32 prev_view_direction[4];     /* 0x30 unk10 as it was the frame before */
    f32 view_distance;              /* 0x40 how far along unk10 */
    f32 prev_view_distance;         /* 0x44 unk40 as it was the frame before */
    f32 nearest_entity_distance;    /* 0x48 distance to the nearest entity around the player (Interact_FindTarget) */
    f32 target_distance;            /* 0x4C distance to what `current`, unk54 or unk58 names */
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
    int unk04;                      /* 0x04 */
    int count;                      /* 0x08 the number of pictures; viewer_picture stays below it */
} ViewerEntry;

/* four texture coordinates, one per corner of the viewer's player marker: a local table MapViewer_DrawPictureMap
   copies whole from read-only data (viewer_marker_corner_us, viewer_marker_corner_vs) */
typedef struct {
    s32 values[4];                  /* 0x00 */
} ViewerCornerTable;

/* an 8-byte wall-clock time passed by value (PlayTime_DayOfYear, PlayTime_ClockSecondsBetween, PlayTime_ClockMinutesBetween) */
typedef struct {
    char unk0;                      /* 0x00 */
    u8 sec;                         /* 0x01 */
    u8 min;                         /* 0x02 */
    u8 hour;                        /* 0x03 */
    char unk4;                      /* 0x04 */
    u8 mday;                        /* 0x05 */
    u8 mon;                         /* 0x06 */
    u8 year;                        /* 0x07 years since 2000 */
} ClockTime;

/* the C library's struct tm, which mktime fills in (PlayTime_GetWeekday) */
typedef struct {
    int tm_sec;                     /* 0x00 */
    int tm_min;                     /* 0x04 */
    int tm_hour;                    /* 0x08 */
    int tm_mday;                    /* 0x0C */
    int tm_mon;                     /* 0x10 0 to 11 */
    int tm_year;                    /* 0x14 years since 1900 */
    int tm_wday;                    /* 0x18 0: Sunday */
    int tm_yday;                    /* 0x1C */
    int tm_isdst;                   /* 0x20 */
} CalendarTime;

/* game_clock: the game clock (core/game_clock.c) */
typedef struct {
    char pad00[0x08];
    int ticks;                      /* 0x08 */
} GameClock;

/* D_00432820: what decides whether GameClock_Tick advances GameClock.ticks */
typedef struct {
    char pad00[0x208];
    char unk208;                    /* 0x208 System_InitMemory clears it */
    char unk209;                    /* 0x209 System_InitMemory clears it */
    char unk20A;                    /* 0x20A System_InitMemory clears it */
    char unk20B;                    /* 0x20B System_InitMemory sets it to 2 */
    char pad20C[0x7];
    char clock_stopped;             /* 0x213 nonzero: the clock stops */
    char tick_held;                 /* 0x214 nonzero: the clock stops */
    char pad215[0x73];
    int flags;                      /* 0x288 0x200: tick every fourth frame */
} ClockControl;

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
    char pad34[0x08];
    u8 starting_level_percent;      /* 0x3C the percent a level record's level starts at for it */
    char pad3D[0x01];
    u16 weight_tenths_of_pound;     /* 0x3E unk33 scaled by D_0043E568, worked out after loading */
} GameDefinition;

/* a GameDefinition as the menu code reads it: the record is word-aligned (LevelDef in player/player.h
   shows the word at 0x30 is bit-fields), and its low 16 bits are the number in the model's file name */
typedef struct {
    char pad00[0x30];
    u32 file_number : 16;           /* 0x30 */
    u32 unk30_16 : 16;
    char pad34[0x0C];
} GameDefinitionWords;

/* a GameDefinition as the save code reads it (Game_WriteSaveImage, Game_ReadSaveImage): unk33 is the top byte of flags */
typedef struct {
    char pad00[0x30];
    u32 flags;                      /* 0x30 bits 18 and 19 are a state; below 2 it is saved as one bit */
    char pad34[0x0C];
} GameDefinitionFlags;

/* item_counts and the five tables after it: one byte per GameDefinition */
typedef struct {
    u8 bytes[380];
} DefinitionByteSet;

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
    struct PoolSlot *unk40;         /* 0x40 another slot, picked by a byte of the part's record (PoolSlot_LoadGroup) */
    struct PoolPart *part;          /* 0x44 the 0x50-byte part this slot shows, NULL: none */
    struct PoolPartRecord *unk48;   /* 0x48 */
    char pad4C[0x04];
    struct PoolSlot *next;          /* 0x50 */
    s32 key_index;                  /* 0x54 the key of its node's animation that PoolNode_Pose is at */
    u64 unk58;                      /* 0x58 copied from the texture header's tex0 */
    u64 unk60;                      /* 0x60 */
    char pad68[0x08];
} PoolSlot;

/* a texture header's two GS register values (TextureHeader.tex0 and unk18), which PoolSlot_LoadGroup copies as one */
typedef struct {
    u64 tex0;                       /* 0x00 */
    u64 unk08;                      /* 0x08 */
} PoolSlotRegisters;

/* a PoolSlot with unk58 and unk60 as the one member they are copied as */
typedef struct {
    char pad00[0x58];
    PoolSlotRegisters registers;    /* 0x58 */
    char pad68[0x08];
} PoolSlotRegistersView;

#define POOL_SLOT_REGISTERS(slot) (((PoolSlotRegistersView *)(slot))->registers)

/* one 0x10-byte record of a RelocEntry, with two file offsets that PoolNodeFile_Relocate turns into pointers */
typedef struct {
    char pad00[0x08];
    void *unk08;                    /* 0x08 */
    void *unk0C;                    /* 0x0C */
} RelocLayer;

/* one 0x40-byte entry of a RelocFile */
typedef struct {
    u16 key_count;                  /* 0x00 */
    u16 layer_count;                /* 0x02 */
    u16 slot;                       /* 0x04 the PoolSlot the node poses (PoolNode_Pose) */
    char pad06[0x2A];
    void *unk30;                    /* 0x30 a file offset until PoolNodeFile_Relocate has run, like the next three */
    RelocLayer *layers;             /* 0x34 */
    void *unk38;                    /* 0x38 */
    void *unk3C;                    /* 0x3C */
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
    u16 unk04;                      /* 0x04 the value of GS ALPHA_2 */
    u16 prim;                       /* 0x06 GS PRIM bits */
    u64 tex0;                       /* 0x08 GS TEX0, set from the texture header's before the part is drawn */
    u64 unk10;                      /* 0x10 GS TEX0 of a second texture drawn over the first (PoolPart_DrawTinted), 0: none */
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
    f32 unkA0[4];                   /* 0xA0 128.0 each */
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
    s32 unk1A0[4];                  /* 0x1A0 0, 0, 0, 0x80; stored as one quadword */
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

/* one 0x80-byte animation key of a RelocEntry (its unk30), interpolated towards the next one by PoolNode_Pose */
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
    s32 unk00;                      /* 0x00 above 0: the part's slot points at this record */
    s32 unk04;                      /* 0x04 how many of the part's normals go by the parent slot's matrix (unk00: points) */
    s32 unk08;                      /* 0x08 index, in the group's slot list, of the slot PoolSlot.unk40 is set to */
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
    f32 unk2C;                      /* 0x2C 0 */
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
    s16 *unk70;                     /* 0x70 seven values */
} Unk20D008Owner;

#endif
