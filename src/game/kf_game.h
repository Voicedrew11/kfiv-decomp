/* Game-wide state blocks. */
#ifndef KF_GAME_H
#define KF_GAME_H

#include "common.h"

/* D_0038CC50 (cleared as 0x48 bytes by func_001127F8). Other files still
   declare it as char[]/int[], so the extern stays in each file. */
typedef struct {
    char unk00;                     /* 0x00 nonzero: script blends are skipped */
    char unk01;                     /* 0x01 */
    char paused;                    /* 0x02 1 while func_00101F90 holds the game */
    s8 unk03;                       /* 0x03 */
    u8 unk04;                       /* 0x04 0 or 1 */
    s8 unk05;                       /* 0x05 nonzero: the camera moves to CameraState.unk30 */
    char unk06;                     /* 0x06 nonzero: the player cannot run (func_001B5A48) */
    char unk07;                     /* 0x07 */
    char pad08[0x18];
    int unk20;                      /* 0x20 index into D_0041A6F8 (TriggerTypeDef), used with bit6 */
    u32 bit0 : 1;                   /* 0x24 */
    u32 bit1 : 1;                   /* set while func_0011FDD0 runs an event */
    u32 bit2 : 1;
    u32 bit3 : 1;
    u32 bit4 : 1;
    u32 bit5 : 1;
    u32 bit6 : 1;
    u32 bit7 : 1;
    u32 bit8 : 1;
    u32 bit9 : 1;                   /* set to leave the main loop (func_001001C0) */
    u32 bits10 : 2;
    u32 bit12 : 1;                  /* bits 12 and 13 both set: the main loop skips drawing and input */
    u32 bit13 : 1;
    u32 bit14 : 1;
    int unk28;                      /* 0x28 */
    int unk2C;                      /* 0x2C */
    int unk30;                      /* 0x30 */
    int unk34;                      /* 0x34 set with bit5 by func_001226E0 */
    char pad38[0x08];
    void *unk40;                    /* 0x40 the model of the item effect that asked for unk30 = 0xC5F (KF_ItemEffect_StepRequestGameStatus) */
    int unk44;                      /* 0x44 counts down to 0; 1 when an item was just used (func_001BC2D0) */
} GameStatus;

/* GameStatus with bits 10 and 11 of the flag word (GameStatus.bits10) apart, as func_00214A20
   sets them */
typedef struct {
    char pad00[0x24];
    u32 bits0 : 9;                  /* 0x24 */
    u32 bit9 : 1;
    u32 bit10 : 1;
    u32 bit11 : 1;
    u32 bits12 : 20;
} GameStatusSplitBits;

/* D_00446A28: the play time as func_001D22B8 splits it (byte 1 seconds, 2 minutes, 3 and 5 hours), passed by value */
typedef struct {
    char bytes[8];
} PlayTime;

/* the first 8 bytes of the save buffer D_0036A130 (func_00103C18 writes them, func_001060C0 checks them) */
typedef struct {
    s32 magic;                      /* 0x00 SAVE_MAGIC */
    s32 size;                       /* 0x04 bytes of state, header included, rounded up to SAVE_BLOCK_SIZE; one checksum
                                       byte for each block of that size follows */
} SaveHeader;

#define SAVE_MAGIC      0x15400
#define SAVE_BLOCK_SIZE 0x80

/* D_004325A0 (the camera, it seems). Other files still declare it as float[],
   so the extern stays in each file. */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 unk10;                      /* 0x10 rotation about x */
    f32 unk14;                      /* 0x14 rotation about y */
    f32 unk18;                      /* 0x18 rotation about z */
    char pad1C[0x04];
    f32 unk20[4];                   /* 0x20 the point to look at (GameStatus.unk04) */
    f32 unk30[4];                   /* 0x30 the point to move to (GameStatus.unk05) */
    f32 unk40;                      /* 0x40 how fast it turns to unk20 */
    f32 unk44;                      /* 0x44 how far it moves to unk30 in a frame */
    char pad48[0x04];
    f32 unk4C;                      /* 0x4C added to the height above the player */
    f32 unk50[4];                   /* 0x50 the first shake's offset (func_00100D10) */
    f32 unk60[4];                   /* 0x60 the second shake's offset */
    f32 unk70;                      /* 0x70 how far the first shake reaches */
    s32 unk74;                      /* 0x74 frames of it left */
    f32 unk78;                      /* 0x78 set with unk7C by func_00195878 */
    s32 unk7C;                      /* 0x7C */
} CameraState;

/* one of the 0x24-byte slots at D_005C5940, linked in a ring. D_004471E4 is the
   one being shown and D_004471E8 the next free one. */
typedef struct MessageSlot {
    char *text;                     /* 0x00 */
    int unk04;                      /* 0x04 */
    int unk08;                      /* 0x08 */
    int length;                     /* 0x0C */
    struct MessageSlot *next;       /* 0x10 */
    int unk14;                      /* 0x14 */
    int unk18;                      /* 0x18 */
    int unk1C;                      /* 0x1C */
    int unk20;                      /* 0x20 */
} MessageSlot;

/* D_00420C10[3]: how the glyphs of one kind (0 full width, 1 half width, 2 the debug font) lie on their texture */
typedef struct {
    f32 step_s;                     /* 0x00 from one column to the next */
    f32 step_t;                     /* 0x04 from one row to the next */
    f32 size_s;                     /* 0x08 of one glyph */
    f32 size_t;                     /* 0x0C */
    f32 base_s;                     /* 0x10 of the first glyph */
    f32 base_t;                     /* 0x14 */
    s32 columns;                    /* 0x18 glyphs in a row */
} GlyphSheet;

/* D_00420C68[font][kind]: the screen box of a glyph, relative to its position */
typedef struct {
    s32 left;                       /* 0x00 */
    s32 top;                        /* 0x04 */
    s32 right;                      /* 0x08 */
    s32 bottom;                     /* 0x0C */
} GlyphBox;

/* D_00420D00: the event runner */
typedef struct {
    f32 unk00[4];                   /* 0x00 a position the camera is put in front of (func_00100FC8) */
    f32 unk10[4];                   /* 0x10 a direction from it */
    f32 unk20[4];                   /* 0x20 unk00 as it was the frame before (func_001D0800) */
    f32 unk30[4];                   /* 0x30 unk10 as it was the frame before */
    f32 unk40;                      /* 0x40 how far along unk10 */
    f32 unk44;                      /* 0x44 unk40 as it was the frame before */
    f32 unk48;                      /* 0x48 distance to the nearest entity around the player (func_001D0970) */
    f32 unk4C;                      /* 0x4C distance to what `current`, unk54 or unk58 names */
    void *current;                  /* 0x50 the event being run */
    void *unk54;                    /* 0x54 an Entity whose spawn info can answer the use of an item (func_001BC2D0) */
    void *unk58;                    /* 0x58 the ItemNode (kf_model.h) in front of the player, NULL: none (func_001D0970) */
} EventRunner;

/* an event func_00120130 hands to func_0011FDD0 */
typedef struct {
    char pad00[0x10];
    u32 flags;                      /* 0x10 */
} GameEvent;

/* the viewers func_0011A500 runs, indexed by picked_viewer (kf_map_viewer.c), 0xC bytes each */
typedef struct {
    void (*run)(void);              /* 0x00 loops until the viewer is left */
    int unk04;                      /* 0x04 */
    int count;                      /* 0x08 the number of pictures; viewer_picture stays below it */
} ViewerEntry;

/* four texture coordinates, one per corner of the viewer's player marker: a local table func_0011AAE8
   copies whole from read-only data (D_0043E760, D_0043E770) */
typedef struct {
    s32 values[4];                  /* 0x00 */
} ViewerCornerTable;

/* an 8-byte wall-clock time passed by value (func_001D2390, func_001D23F0, func_001D24A8) */
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

/* a LevelRecord (kf_player.h; D_003679A0, D_003682F0) seen with its time as the ClockTime it is: func_001ED9A8
   passes the member by value, which the char[8] of LevelRecord.unk19 cannot give */
typedef struct {
    s16 unk00[9];                   /* 0x00 */
    u8 level;                       /* 0x12 */
    char pad13[0x05];
    u8 unk18;                       /* 0x18 nonzero: the time below counts */
    ClockTime unk19;                /* 0x19 not aligned */
    char pad21[0x01];
} LevelRecordTimed;

/* D_00431CE8: func_0020CD00 advances its counter once a frame */
typedef struct {
    char pad00[0x08];
    int ticks;                      /* 0x08 */
} GameClock;

/* D_00432820: what decides whether func_0020CD00 advances GameClock.ticks */
typedef struct {
    char pad00[0x208];
    char unk208;                    /* 0x208 func_0020B6A0 clears it */
    char unk209;                    /* 0x209 func_0020B6A0 clears it */
    char unk20A;                    /* 0x20A func_0020B6A0 clears it */
    char unk20B;                    /* 0x20B func_0020B6A0 sets it to 2 */
    char pad20C[0x7];
    char unk213;                    /* 0x213 nonzero: the clock stops */
    char unk214;                    /* 0x214 nonzero: the clock stops */
    char pad215[0x73];
    int flags;                      /* 0x288 0x200: tick every fourth frame */
} ClockControl;

/* auto-repeat timing of one button of a PadSlot (set by func_002177F8, stepped by func_00216EA0) */
typedef struct {
    u8 delay;                       /* 0x00 frames before the first repeat */
    u8 interval;                    /* 0x01 frames between repeats */
    u8 countdown;                   /* 0x02 */
    u8 fired;                       /* 0x03 0 none, 1 first press, 2 repeat */
} PadRepeatKey;

/* one controller of D_01E0DB00 (0x1C0 bytes), kept up to date by func_00217008 */
typedef struct {
    u8 dma_buffer[0x100];           /* 0x000 work area handed to scePadPortOpen */
    u8 report_status;               /* 0x100 the 0x24 bytes scePadRead gave (func_00216DD0) */
    u8 report_id;                   /* 0x101 */
    u16 buttons;                    /* 0x102 held buttons, a set bit is a pressed button */
    u8 report_analog[0x1C];         /* 0x104 sticks and button pressures */
    u16 pressed;                    /* 0x120 buttons that went down this frame */
    u16 repeated;                   /* 0x122 buttons that fired through auto-repeat */
    PadRepeatKey repeat_keys[16];   /* 0x124 */
    u16 repeat_mask;                /* 0x164 buttons that auto-repeat */
    char pad166[0x02];
    int state;                      /* 0x168 scePadGetState */
    int request_state;              /* 0x16C scePadGetReqState */
    int actuator_count;             /* 0x170 scePadInfoAct(-1) */
    u8 actuators[6];                /* 0x174 [0] small motor on/off, [1] big motor power */
    u8 phase;                       /* 0x17A step of the set-up state machine in func_00217008 */
    u8 next_phase;                  /* 0x17B phase to go to after the actuators are aligned */
    u8 pad_type;                    /* 0x17C report_id >> 4 */
    u8 features;                    /* 0x17D 1: mode can be switched, 2: has actuators, 4: pressure sensitive */
    u8 mode;                        /* 0x17E 0xFF unknown, 0 digital, 1 analog, 2 analog with pressure */
    u8 mode_request;                /* 0x17F mode asked for, 0xFF: none */
    u8 mode_lock;                   /* 0x180 passed to scePadSetMainMode */
    u8 actuators_aligned;           /* 0x181 */
    char pad182[0x3E];
} PadSlot;

/* D_01E0DB00: two ports with four multitap slots each */
typedef struct {
    PadSlot slots[8];               /* 0x000 index is port * 4 + slot */
    u8 open_slots[2];               /* 0xE00 per port, bit n: slot n was opened */
} PadSystem;

/* one corner of a character's sprite as KF_TextLog_PutChar queues it: UV, RGBAQ and XYZ2 for the GS */
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

/* text_log_packet: the GIF packet KF_TextLog_Flush sends, a tag and room for 37 characters */
typedef struct {
    int tag[4];                     /* 0x00 */
    TextLogGlyph glyphs[37];        /* 0x10 */
} TextLogPacket;

/* text_log: the 0x800-byte text buffer KF_TextLog_AppendV appends to, and the window KF_TextLog_Flush prints it in */
typedef struct {
    TextLogGlyph *packet;           /* 0x00 where the next character goes in text_log_packet */
    char pad04[0x0C];
    int left;                       /* 0x10 the window, in GS coordinates (KF_TextLog_SetWindow) */
    int top;                        /* 0x14 */
    int width;                      /* 0x18 */
    int height;                     /* 0x1C */
    int depth;                      /* 0x20 */
    char pad24[0x0C];
    int red;                        /* 0x30 text colour */
    int green;                      /* 0x34 */
    int blue;                       /* 0x38 */
    char pad3C[0x04];
    int font_base;                  /* 0x40 GS page of the font texture (KF_TextLog_LoadFont) */
    int palette_base;               /* 0x44 GS page of its palette */
    int font_buffer_width;          /* 0x48 in units of 64 pixels */
    int used;                       /* 0x4C bytes of text */
    char text[0x800];               /* 0x50 */
} TextLog;

/* one of the three 0x180-byte records at *D_0044733C, cleared by func_001FBAC8 */
typedef struct {
    char flag0;                     /* 0x00 */
    char flag1;                     /* 0x01 */
    char pad02[0x02];
    int words[10];                  /* 0x04 */
    char pad2C[0x14C];
    PlayTime tail;                  /* 0x178 */
} ScratchSlot;

/* D_004147F8[380]: 0x40-byte definitions read from a data file by func_00112648
   (150, then 70, then 160 of them: see func_001029A8) */
typedef struct {
    char pad00[0x30];
    u16 unk30;                      /* 0x30 the number in the name of its item_arm file (KF_Player_LoadArmModels) */
    char pad32[0x01];
    u8 unk33;                       /* 0x33 */
    char pad34[0x08];
    u8 unk3C;                       /* 0x3C */
    char pad3D[0x01];
    u16 unk3E;                      /* 0x3E unk33 scaled by D_0043E568, worked out after loading */
} GameDefinition;

/* a GameDefinition as the menu code reads it: the record is word-aligned (LevelDef in kf_player.h
   shows the word at 0x30 is bit-fields), and its low 16 bits are the number in the model's file name */
typedef struct {
    char pad00[0x30];
    u32 file_number : 16;           /* 0x30 */
    u32 unk30_16 : 16;
    char pad34[0x0C];
} GameDefinitionWords;

/* a GameDefinition as the save code reads it (func_00103C18, func_00104EC8): unk33 is the top byte of flags */
typedef struct {
    char pad00[0x30];
    u32 flags;                      /* 0x30 bits 18 and 19 are a state; below 2 it is saved as one bit */
    char pad34[0x0C];
} GameDefinitionFlags;

/* a GameDefinition with the word at 0x30 as the bit-fields the menu code reads (func_001EF648): as a bit-field the
   file number keeps its 0x30 in the load (D_004147F8[i].file_number), where the u16 member folds it into the address */
typedef struct {
    char pad00[0x30];
    u32 file_number : 16;           /* 0x30 GameDefinition.unk30 */
    u32 unk30_16 : 2;
    u32 state : 2;                  /* GameDefinitionFlags.flags bits 18 and 19 */
    u32 unk30_20 : 12;
    char pad34[0x0C];
} GameDefinitionBits;

/* D_00369830 and the five tables after it: one byte per GameDefinition */
typedef struct {
    u8 bytes[380];
} DefinitionByteSet;

/* D_0034C350[50]: the 50 flags of one area, a bit each */
typedef struct {
    u8 bits[8];
} AreaFlagBits;

/* a word read from any alignment (the save image header) */
typedef struct {
    int value;
} __attribute__((packed)) UnalignedInt;

/* D_0043E460: the x offsets func_001067E0 cycles its sprites through */
typedef struct {
    int x[3];
} LayerOffsets;

/* D_00467680, D_00467940, D_00467C00: an 8 x 11 grid of points func_00108020 wobbles */
typedef struct {
    int points[8][11][2];
} __attribute__((aligned(16))) RippleGrid;

/* a node of the doubly linked list at D_004469D4 */
typedef struct ListNode {
    char pad00[0x20];
    struct ListNode *prev;          /* 0x20 */
    struct ListNode *next;          /* 0x24 */
} ListNode;

/* what D_004471E4 and D_004471E8 point at; func_001CFC80 clears unk04 */
typedef struct {
    char pad00[0x04];
    int unk04;                      /* 0x04 */
} PendingSlot;

/* D_005C58E8: the message box (func_001CF988 opens it, func_001CF4D0 runs it, func_001CF708 draws it) */
typedef struct {
    s16 *start;                     /* 0x00 first glyph of the page on screen */
    s16 *cursor;                    /* 0x04 glyph being typed */
    s16 *end;                       /* 0x08 end of the page (func_001D0658) */
    int delay;                      /* 0x0C frames per glyph */
    int timer;                      /* 0x10 frames since the last glyph */
    int unk14;                      /* 0x14 */
    int waiting;                    /* 0x18 1 = page typed out, waiting for a button */
    int typed;                      /* 0x1C glyphs typed on this page (control codes not counted) */
    int closing;                    /* 0x20 1 = closing */
    int close_timer;                /* 0x24 frames left of the closing */
    int left;                       /* 0x28 */
    int top;                        /* 0x2C */
    int font;                       /* 0x30 */
    int unk34;                      /* 0x34 */
    int line_offsets[4];            /* 0x38 left edge of each line (func_001D0658) */
} MessageBox;

/* D_0042A8F0[32]: a pool of 0x70-byte slots chained into a free list through `next`;
   D_00446DF0 holds the head */
typedef struct PoolSlot {
    char pad00[0x40];
    struct PoolSlot *unk40;         /* 0x40 another slot, picked by a byte of the part's record (func_002032E0) */
    struct PoolPart *part;          /* 0x44 the 0x50-byte part this slot shows, NULL: none */
    struct PoolPartRecord *unk48;   /* 0x48 */
    char pad4C[0x04];
    struct PoolSlot *next;          /* 0x50 */
    s32 key_index;                  /* 0x54 the key of its node's animation that func_00203820 is at */
    u64 unk58;                      /* 0x58 copied from the texture header's tex0 */
    u64 unk60;                      /* 0x60 */
    char pad68[0x08];
} PoolSlot;

/* a texture header's two GS register values (TextureHeader.tex0 and unk18), which func_002032E0 copies as one */
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

/* one 0x10-byte record of a RelocEntry, with two file offsets that func_00200820 turns into pointers */
typedef struct {
    char pad00[0x08];
    void *unk08;                    /* 0x08 */
    void *unk0C;                    /* 0x0C */
} RelocLayer;

/* one 0x40-byte entry of a RelocFile */
typedef struct {
    u16 key_count;                  /* 0x00 */
    u16 layer_count;                /* 0x02 */
    u16 slot;                       /* 0x04 the PoolSlot the node poses (func_00203820) */
    char pad06[0x2A];
    void *unk30;                    /* 0x30 a file offset until func_00200820 has run, like the next three */
    RelocLayer *layers;             /* 0x34 */
    void *unk38;                    /* 0x38 */
    void *unk3C;                    /* 0x3C */
} RelocEntry;

/* a file whose offsets func_00200820 turns into pointers: a 0x10-byte header, then the entries */
typedef struct {
    char pad00[0x08];
    u32 entry_count;                /* 0x08 */
    char pad0C[0x04];
    RelocEntry entries[1];          /* 0x10 */
} RelocFile;

/* lists of pool slot numbers that func_00203090 and func_002032E0 copy whole from read-only data, and
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
    u64 unk10;                      /* 0x10 GS TEX0 of a second texture drawn over the first (func_002008E8), 0: none */
    char pad18[0x08];
    PoolPartPoint points[1];        /* 0x20 point_count of them */
} PoolPartMaterial;

/* PoolPartMaterial.tex0 with the CLUT load control of GS TEX0 as a bit-field */
typedef struct {
    char pad00[0x08];
    u64 pad08 : 61;
    u64 cld : 3;                    /* bit 61 */
} PoolPartMaterialBits;

/* the scratchpad while a PoolPart is drawn (func_002008E8, func_00201518, func_002020A0) */
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
    f32 (*work)[4];                 /* 0x194 D_00446D40 */
    f32 (*normals)[4];              /* 0x198 the part's normals in the world */
    char pad19C[0x04];
    s32 unk1A0[4];                  /* 0x1A0 0, 0, 0, 0x80; stored as one quadword */
    u32 parent_point_count;         /* 0x1B0 func_002020A0: how many points go by the parent slot's matrix */
    u32 point_at;                   /* 0x1B4 */
    u32 point_index;                /* 0x1B8 */
    u32 parent_normal_count;        /* 0x1BC func_002020A0: how many normals go by the parent slot's matrix */
    u32 normal;                     /* 0x1C0 */
    u32 normal_index;               /* 0x1C4 */
} PoolDrawScratch;

#define POOL_DRAW_SCRATCH ((PoolDrawScratch *)0x70000000)

/* PoolDrawScratch.cursor as a union (func_002008E8 advances it through this view once per point) */
typedef struct {
    char pad000[0x18C];
    union {
        u32 *words;                 /* 0x18C */
        u8 *bytes;
    } cursor;
} PoolDrawScratchCursorView;

#define POOL_DRAW_SCRATCH_CURSOR(scratch) (((PoolDrawScratchCursorView *)(scratch))->cursor)

/* the two places in GS memory that func_00201E10 and func_00201F58 pick a palette's from (D_00440DC0) */
typedef struct {
    u64 pages[2];                   /* 0x00 */
} PoolClutPages;

/* one 0x80-byte animation key of a RelocEntry (its unk30), interpolated towards the next one by func_00203820 */
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

/* the 0x10 bytes in front of a PoolPartFile's parts, copied as one (func_00203C88) */
typedef struct {
    PoolPartRecord *records_end;    /* 0x00 */
    char pad04[0x04];
    u32 part_count;                 /* 0x08 */
    char pad0C[0x04];
} PoolPartFileHeader;

/* the 0x20 bytes in front of a strip's points as the four words func_00203C88 copies them as */
typedef union {
    u64 words[4];                   /* 0x00 */
    char bytes[0x20];               /* 0x00 */
} PoolPartStripWords;

/* one 0x40-byte point of a strip in the copy func_00203C88 makes: the point and normal themselves
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

/* what func_002032E0 is handed: a part file and the texture its parts are drawn with */
typedef struct {
    PoolPartFile *file;             /* 0x00 */
    void *texture;                  /* 0x04 a TextureHeader (kf_texture.h) */
} PoolPartSource;

/* D_00413DB8[2]: a controller as the game reads it, one per port (func_001B5590). D_00413DA8 is the copy
   of port 0 that the game tests. */
typedef struct {
    u16 held;                       /* 0x00 buttons held, the stick directions among them */
    u16 pressed;                    /* 0x02 buttons that went down this frame */
    s32 type;                       /* 0x04 what func_002179F8 says: 1 has sticks, 0xFF is no controller */
    u16 look_buttons;               /* 0x08 the right stick's up and down as the buttons 0x200 and 0x100 */
    u16 prev_look_buttons;          /* 0x0A */
    u16 move_buttons;               /* 0x0C the left stick as the direction buttons (0x80, 0x20, 0x10, 0x40) */
    u16 prev_move_buttons;          /* 0x0E */
} PadState;

/* what func_00217760 returns: the controller's own report */
typedef struct {
    char pad00[0x02];
    u16 held;                       /* 0x02 */
    u8 right_stick_x;               /* 0x04 the four axes, 0x80 at rest */
    u8 right_stick_y;               /* 0x05 */
    u8 left_stick_x;                /* 0x06 */
    u8 left_stick_y;                /* 0x07 */
    char pad08[0x18];
    u16 pressed;                    /* 0x20 */
} PadReport;

/* PadReport's axis bytes two at a time: zero when the controller sends none */
typedef struct {
    char pad00[0x04];
    u16 right_stick;                /* 0x04 */
    u16 left_stick;                 /* 0x06 */
} PadReportAxes;

/* PadReport.pressed inside a union: a read through it may alias any store, so it stays behind one
   (the boot screens func_0020A490 and func_0020A8C8 only match this way) */
typedef union {
    u16 buttons;
    u8 bytes[2];
} PadButtonsUnion;

typedef struct {
    char pad00[0x20];
    PadButtonsUnion pressed;        /* 0x20 */
} PadReportUnion;

/* the texts of the debug start menu (func_001B0A78), local arrays it copies whole from
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

/* what func_0020D008 is given; nothing calls that function, so what this is stays unknown */
typedef struct {
    char pad00[0x70];
    s16 *unk70;                     /* 0x70 seven values */
} Unk20D008Owner;

#endif
