/* Prebuilt GS packets that the game copies and adjusts before sending. */
#ifndef KF_PACKET_H
#define KF_PACKET_H

#include "common.h"

/* the GS XYZ2 register: 12.4 fixed-point x and y, 32-bit z */
typedef struct {
    u64 X : 16;
    u64 Y : 16;
    u64 Z : 32;
} GsXyz;

/* a 0x60-byte sprite packet (two corners), like the static copies ui/menu_draw.c's sprite functions keep */
typedef struct {
    GsXyz xyz0;                     /* 0x00 */
    s64 unk08;                      /* 0x08 */
    GsXyz xyz1;                     /* 0x10 */
    f32 second_s;                   /* 0x18 the right edge's texture coordinate (TexturedRectDef.second_s) */
    char pad1C[0x04];
    u8 red;                         /* 0x20 */
    u8 green;                       /* 0x21 */
    u8 blue;                        /* 0x22 */
    u8 alpha;                       /* 0x23 */
    char pad24[0x0D];
    u8 kind;                        /* 0x31 the kind (KindObject.kind); 2: MenuDraw_Number asks MenuDraw_GetNumber for the number */
    u8 word_index;                  /* 0x32 which word of menu_word_starts MenuDraw_Word draws (kind 1) */
    u8 digit_count;                 /* 0x33 how many digits MenuDraw_Number draws; 0: three digits, a point and one more */
    s32 number_id;                  /* 0x34 which number the sprite stands for (MenuDraw_GetNumber gives its current value) */
    char pad38[0x06];
    u8 handler_index;               /* 0x3E 1: a menu screen's own sprite, 2: one Menu_DrawBackdropItemSelParts/Menu_DrawBackdropPreshopParts draw */
    s8 slot;                        /* 0x3F which slot the packet belongs to */
    char pad40[0x01];
    s8 slide_range;                 /* 0x41 the amount of movement a slide handler takes as half a screen */
    u8 blend_mode;                  /* 0x42 handed to Menu_SetBlendMode before the sprite is drawn */
    s8 has_cursor_bar;              /* 0x43 1: the cursor bar is drawn just before this one (Smith_UpdateSelectScreen) */
    char pad44[0x1C];
} SpritePacket;

/* SpritePacket's colour (0x20) as four 8-bit fields of one unsigned word, the shape of Sony's sceGsRgbaq: a field
   read on its own is an unsigned int, so turning it into a float takes the unsigned conversion (Trade_UpdateOwnedList) */
typedef struct {
    u32 red : 8;
    u32 green : 8;
    u32 blue : 8;
    u32 alpha : 8;
} SpritePacketColor;

#define SPRITE_PACKET_COLOR(packet) ((SpritePacketColor *)&(packet)->red)

/* the same four colour fields seen from the start of the packet: reads and writes are then based on the packet's
   own address (lbu 0x23(packet)), as retail's are in Menu_UpdateCardSlot */
typedef struct {
    char pad00[0x20];
    u32 red : 8;                    /* 0x20 */
    u32 green : 8;                  /* 0x21 */
    u32 blue : 8;                   /* 0x22 */
    u32 alpha : 8;                  /* 0x23 */
    char pad24[0x3C];
} SpritePacketColorView;

#define SPRITE_PACKET_COLOR_VIEW(packet) ((SpritePacketColorView *)(packet))

/* one quadword DMA tag at the head of a packet chain */
typedef struct {
    s32 id_and_qwc;                 /* 0x00 the DMA tag ID (bits 28-30) and quadword count */
    s32 address;                    /* 0x04 the next packet, or 0 */
    s32 unk08;                      /* 0x08 */
    s32 unk0C;                      /* 0x0C */
} DmaTag;

/* one corner of a ScreenSprite */
typedef struct {
    u16 x;                          /* 0x00 12.4 fixed point */
    u16 y;                          /* 0x02 */
    u32 z;                          /* 0x04 */
    s32 tex_s;                      /* 0x08 texture coordinate, 12.4 fixed point (unused when untextured) */
    s32 tex_t;                      /* 0x0C */
} ScreenSpriteVertex;

/* what GsDraw_QueueTexturedSprite (textured) and GsDraw_QueueFlatSprite (flat) turn into a GS sprite; same layout as SplashSprite */
typedef struct {
    u64 tex0;                       /* 0x00 GS TEX0 */
    u64 tex1;                       /* 0x08 GS TEX1 */
    s32 prim;                       /* 0x10 GS PRIM bits; the sprite type (6) is added */
    u8 rgba[4];                     /* 0x14 */
    char pad18[0x08];
    ScreenSpriteVertex vertex[2];   /* 0x20 */
} ScreenSprite;

/* a screen rectangle with two corners, as GsDraw_QueueTexturedSprite draws it */
typedef struct {
    s64 tag;                        /* 0x00 */
    s64 unk08;                      /* 0x08 */
    s32 unk10;                      /* 0x10 */
    u8 red;                         /* 0x14 */
    u8 green;                       /* 0x15 */
    u8 blue;                        /* 0x16 */
    u8 alpha;                       /* 0x17 */
    char pad18[0x08];
    u16 first_x;                    /* 0x20 12.4 fixed point */
    u16 first_y;                    /* 0x22 */
    u32 first_z;                    /* 0x24 */
    s32 first_u;                    /* 0x28 */
    s32 first_v;                    /* 0x2C */
    u16 second_x;                   /* 0x30 */
    u16 second_y;                   /* 0x32 */
    u32 second_z;                   /* 0x34 */
    s32 second_u;                   /* 0x38 */
    s32 second_v;                   /* 0x3C */
} ScreenQuad;

/* one vertex of the textured primitives Wobble_DrawRippledModel builds: texture coordinates, then where it is on the screen */
typedef struct {
    f32 tex_s;                      /* 0x00 */
    f32 tex_t;                      /* 0x04 */
    f32 tex_q;                      /* 0x08 1.0 */
    char pad0C[0x04];
    s32 x;                          /* 0x10 */
    s32 y;                          /* 0x14 */
    s32 z;                          /* 0x18 */
    s32 flags;                      /* 0x1C 0x8000: this vertex draws nothing */
} PacketStVertex;

/* the GS registers a draw environment sets, each followed by its register address (SDK sceGsDrawEnv1) */
typedef struct {
    s64 frame;                      /* 0x00 */
    s64 frame_addr;                 /* 0x08 */
    s64 zbuf;                       /* 0x10 */
    s64 zbuf_addr;                  /* 0x18 */
    s64 xyoffset;                   /* 0x20 */
    s64 xyoffset_addr;              /* 0x28 */
    s64 scissor;                    /* 0x30 */
    s64 scissor_addr;               /* 0x38 */
    s64 prmodecont;                 /* 0x40 */
    s64 prmodecont_addr;            /* 0x48 */
    s64 colclamp;                   /* 0x50 */
    s64 colclamp_addr;              /* 0x58 */
    s64 dthe;                       /* 0x60 */
    s64 dthe_addr;                  /* 0x68 */
    s64 test;                       /* 0x70 */
    s64 test_addr;                  /* 0x78 */
} GsDrawEnv;

/* a GIF tag followed by a draw environment, the packet sceGsPutDrawEnv sends */
typedef struct {
    int tag[4];                     /* 0x00 */
    GsDrawEnv env;                  /* 0x10 */
} GsDrawPacket;

/* 32 bytes of GS data: offscreen_square_corners, two (x, y, z, 0) word vectors */
typedef struct {
    s64 unk00;                      /* 0x00 */
    s64 unk08;                      /* 0x08 */
    s64 unk10;                      /* 0x10 */
    s64 unk18;                      /* 0x18 */
} GsDataBlock;

/* the local frame DrawEnv_PutOffscreenAndBack and DrawEnv_PutOffscreen build: a cleared header, a data block, the packet */
typedef struct {
    char pad00[0x10];
    GsDataBlock block;              /* 0x10 */
    GsDrawPacket packet;            /* 0x30 */
} GsDrawFrame;

/* the 0x1A0-byte GIF packet SaveScreen_DrawBackground fills: a tag, then 25 address+data register writes */
typedef struct {
    s32 gif_tag;                    /* 0x00 the GIF tag: 25 quadwords, end of packet */
    s32 gif_tag_prim;               /* 0x04 the tag's other word: PRE, PRIM 0x16 (a textured sprite), NREG 1 */
    s32 gif_tag_register_list;      /* 0x08 the tag's register list: register 0xE, A+D */
    s32 gif_tag_register_list_hi;   /* 0x0C the register list's second word, 0 */
    s64 work_target_frame;          /* 0x10 FRAME_1: the work buffer the frame is copied into */
    s32 work_target_frame_register; /* 0x18 the A+D write's register: FRAME_1 (0x4C) */
    s32 work_target_frame_pad;      /* 0x1C the A+D write's last word, always 0 */
    s32 xyoffset;                   /* 0x20 XYOFFSET_1: the x and y offsets, 0 */
    s32 xyoffset_hi;                /* 0x24 the XYOFFSET_1 value's upper half, 0 */
    s32 xyoffset_register;          /* 0x28 the A+D write's register: XYOFFSET_1 (0x18) */
    s32 xyoffset_pad;               /* 0x2C the A+D write's last word, always 0 */
    s32 test;                       /* 0x30 TEST_1: the z test passes whatever the depth is */
    s32 test_hi;                    /* 0x34 the TEST_1 value's upper half, 0 */
    s32 test_register;              /* 0x38 the A+D write's register: TEST_1 (0x47) */
    s32 test_pad;                   /* 0x3C the A+D write's last word, always 0 */
    s32 scissor_x;                  /* 0x40 SCISSOR_1's x range: 0 to 640 */
    s32 scissor_y;                  /* 0x44 SCISSOR_1's y range: 0 to 224 */
    s32 scissor_register;           /* 0x48 the A+D write's register: SCISSOR_1 (0x40) */
    s32 scissor_pad;                /* 0x4C the A+D write's last word, always 0 */
    s64 source_tex0;                /* 0x50 TEX0_1: the shown frame as a 1024 by 512 texture */
    s32 source_tex0_register;       /* 0x58 the A+D write's register: TEX0_1 (0x06) */
    s32 source_tex0_pad;            /* 0x5C the A+D write's last word, always 0 */
    s32 rgbaq_rgba;                 /* 0x60 RGBAQ: the colour the source quad is drawn in, 0x80 each */
    f32 rgbaq_q;                    /* 0x64 RGBAQ's Q, 1.0 */
    s32 rgbaq_register;             /* 0x68 the A+D write's register: RGBAQ (0x01) */
    s32 rgbaq_pad;                  /* 0x6C the A+D write's last word, always 0 */
    s32 source_st_corner0_s;        /* 0x70 ST: the source texture's s at the top left corner, 0 */
    s32 source_st_corner0_t;        /* 0x74 ST: the source texture's t at the top left corner, 0 */
    s32 source_st_corner0_register; /* 0x78 the A+D write's register: ST (0x02) */
    s32 source_st_corner0_pad;      /* 0x7C the A+D write's last word, always 0 */
    s32 source_xy_corner0;          /* 0x80 XYZ2: the top left corner of the quad, x and y in 12.4 */
    s32 source_z_corner0;           /* 0x84 XYZ2: its z, 0 */
    s32 source_xy_corner0_register; /* 0x88 the A+D write's register: XYZ2 (0x05) */
    s32 source_xy_corner0_pad;      /* 0x8C the A+D write's last word, always 0 */
    f32 source_st_corner1_s;        /* 0x90 ST: the source texture's s at the other corner, 0.625 */
    f32 source_st_corner1_t;        /* 0x94 ST: the source texture's t at the other corner, 0.875 */
    s32 source_st_corner1_register; /* 0x98 the A+D write's register: ST (0x02) */
    s32 source_st_corner1_pad;      /* 0x9C the A+D write's last word, always 0 */
    s32 source_xy_corner1;          /* 0xA0 XYZ2: the quad's other corner: 640.0 by 224.0 */
    s32 source_z_corner1;           /* 0xA4 XYZ2: its z, 0 */
    s32 source_xy_corner1_register; /* 0xA8 the A+D write's register: XYZ2 (0x05) */
    s32 source_xy_corner1_pad;      /* 0xAC the A+D write's last word, always 0 */
    s32 texflush;                   /* 0xB0 TEXFLUSH: the texture cache is dropped, 0 */
    s32 texflush_hi;                /* 0xB4 the TEXFLUSH value's upper half, 0 */
    s32 texflush_register;          /* 0xB8 the A+D write's register: TEXFLUSH (0x3F) */
    s32 texflush_pad;               /* 0xBC the A+D write's last word, always 0 */
    s64 first_frame;                /* 0xC0 FRAME_1: the first frame buffer the copy is drawn into */
    s32 first_frame_register;       /* 0xC8 the A+D write's register: FRAME_1 (0x4C) */
    s32 first_frame_pad;            /* 0xCC the A+D write's last word, always 0 */
    s64 work_tex0;                  /* 0xD0 TEX0_1: the copy of the frame as a 1024 by 512 texture */
    s32 work_tex0_register;         /* 0xD8 the A+D write's register: TEX0_1 (0x06) */
    s32 work_tex0_pad;              /* 0xDC the A+D write's last word, always 0 */
    s32 first_frame_st_corner0_s;   /* 0xE0 ST: the copy's s at the top left corner, 0 */
    s32 first_frame_st_corner0_t;   /* 0xE4 ST: the copy's t at the top left corner, 0 */
    s32 first_frame_st_corner0_register; /* 0xE8 the A+D write's register: ST (0x02) */
    s32 first_frame_st_corner0_pad; /* 0xEC the A+D write's last word, always 0 */
    s32 first_frame_xy_corner0;     /* 0xF0 XYZ2: the copy's top left corner, 0 by 0 */
    s32 first_frame_z_corner0;      /* 0xF4 XYZ2: its z, 0 */
    s32 first_frame_xy_corner0_register; /* 0xF8 the A+D write's register: XYZ2 (0x05) */
    s32 first_frame_xy_corner0_pad; /* 0xFC the A+D write's last word, always 0 */
    f32 first_frame_st_corner1_s;   /* 0x100 ST: the copy's s at the other corner, 0.625 */
    f32 first_frame_st_corner1_t;   /* 0x104 ST: the copy's t at the other corner, 0.4375 */
    s32 first_frame_st_corner1_register; /* 0x108 the A+D write's register: ST (0x02) */
    s32 first_frame_st_corner1_pad; /* 0x10C the A+D write's last word, always 0 */
    s32 first_frame_xy_corner1;     /* 0x110 XYZ2: the copy's other corner: 640.0 by 224.0 */
    s32 first_frame_z_corner1;      /* 0x114 XYZ2: its z, 0 */
    s32 first_frame_xy_corner1_register; /* 0x118 the A+D write's register: XYZ2 (0x05) */
    s32 first_frame_xy_corner1_pad; /* 0x11C the A+D write's last word, always 0 */
    s64 second_frame;               /* 0x120 FRAME_1: the second frame buffer the copy is drawn into */
    s32 second_frame_register;      /* 0x128 the A+D write's register: FRAME_1 (0x4C) */
    s32 second_frame_pad;           /* 0x12C the A+D write's last word, always 0 */
    s32 second_frame_st_corner0_s;  /* 0x130 ST: the copy's s at the top left corner, 0 */
    s32 second_frame_st_corner0_t;  /* 0x134 ST: the copy's t at the top left corner, 0 */
    s32 second_frame_st_corner0_register; /* 0x138 the A+D write's register: ST (0x02) */
    s32 second_frame_st_corner0_pad; /* 0x13C the A+D write's last word, always 0 */
    s32 second_frame_xy_corner0;    /* 0x140 XYZ2: the copy's top left corner, 0 by 0 */
    s32 second_frame_z_corner0;     /* 0x144 XYZ2: its z, 0 */
    s32 second_frame_xy_corner0_register; /* 0x148 the A+D write's register: XYZ2 (0x05) */
    s32 second_frame_xy_corner0_pad; /* 0x14C the A+D write's last word, always 0 */
    f32 second_frame_st_corner1_s;  /* 0x150 ST: the copy's s at the other corner, 0.625 */
    f32 second_frame_st_corner1_t;  /* 0x154 ST: the copy's t at the other corner, 0.4375 */
    s32 second_frame_st_corner1_register; /* 0x158 the A+D write's register: ST (0x02) */
    s32 second_frame_st_corner1_pad; /* 0x15C the A+D write's last word, always 0 */
    s32 second_frame_xy_corner1;    /* 0x160 XYZ2: the copy's other corner: 640.0 by 224.0 */
    s32 second_frame_z_corner1;     /* 0x164 XYZ2: its z, 0 */
    s32 second_frame_xy_corner1_register; /* 0x168 the A+D write's register: XYZ2 (0x05) */
    s32 second_frame_xy_corner1_pad; /* 0x16C the A+D write's last word, always 0 */
    s32 test_again;                 /* 0x170 TEST_1 once more: the z test changed */
    s32 test_again_hi;              /* 0x174 the TEST_1 value's upper half, 0 */
    s32 test_again_register;        /* 0x178 the A+D write's register: TEST_1 (0x47) */
    s32 test_again_pad;             /* 0x17C the A+D write's last word, always 0 */
    s32 texflush_again;             /* 0x180 TEXFLUSH: the texture cache is dropped again, 0 */
    s32 texflush_again_hi;          /* 0x184 the TEXFLUSH value's upper half, 0 */
    s32 texflush_again_register;    /* 0x188 the A+D write's register: TEXFLUSH (0x3F) */
    s32 texflush_again_pad;         /* 0x18C the A+D write's last word, always 0 */
    s32 alpha;                      /* 0x190 ALPHA_1: the blend the screen's later sprites use */
    s32 alpha_hi;                   /* 0x194 the ALPHA_1 value's upper half, 0 */
    s32 alpha_register;             /* 0x198 the A+D write's register: ALPHA_1 (0x42) */
    s32 alpha_pad;                  /* 0x19C the A+D write's last word, always 0 */
} GsFillPacket;

/* the two packet buffers MenuDraw_NextPacket alternates between (its local `buffers`) */
typedef struct {
    void *buffers[2];
} BufferPair;

/* hands out the next packet buffer to fill; callers use it as whichever packet they build */
void *MenuDraw_NextPacket(void);

/* one 32-bit word of a packet built in place (Menu_DrawBrightness), written as an integer or as a float */
typedef union {
    s32 as_int;
    f32 as_float;
} GsPacketWord;

/* the 14-byte header a .bmp file starts with (ScreenDump_Bmp writes screenshots) */
typedef struct {
    u8 type[2];                     /* 0x00 "BM" */
    u16 file_size_low;              /* 0x02 the file's size; the 32-bit fields are unaligned, so the game writes halves */
    u16 file_size_high;             /* 0x04 */
    u16 reserved[2];                /* 0x06 */
    u16 pixel_offset_low;           /* 0x0A where the pixels start in the file */
    u16 pixel_offset_high;          /* 0x0C */
} BmpFileHeader;

/* the 40-byte image header that follows BmpFileHeader */
typedef struct {
    u32 header_size;                /* 0x00 */
    s32 width;                      /* 0x04 */
    s32 height;                     /* 0x08 above 0: the rows run bottom to top */
    u16 planes;                     /* 0x0C */
    u16 bit_count;                  /* 0x0E */
    u32 compression;                /* 0x10 */
    u32 image_size;                 /* 0x14 */
    s32 pixels_per_metre_x;         /* 0x18 */
    s32 pixels_per_metre_y;         /* 0x1C */
    u32 colours_used;               /* 0x20 */
    u32 colours_important;          /* 0x24 */
} BmpInfoHeader;

#ifndef SCE_LIBGRAPH_LIBGRAPH_H /* a file that includes <libgraph.h> first uses its prototypes */
int sceGsPutDrawEnv(GsDrawPacket *);
int sceGsSetDefDrawEnv(GsDrawEnv *, s16, s16, s16, s16, s16);
int sceGsSyncPath(int, int);
#endif

#endif
