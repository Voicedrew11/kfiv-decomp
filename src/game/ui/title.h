/* Title screens (logos, PRESS START, the main menu) and the game-over screen, drawn from title\*.tm2. */
#ifndef KF_TITLE_H
#define KF_TITLE_H

#include "common.h"
#include "ui/splash.h"
#include "gfx/texture.h"

/* indices into title_textures: the slot each title\*.tm2 file is loaded to */
#define TITLE_TEXTURE_LOGO             0x00 /* logo.tm2 */
#define TITLE_TEXTURE_PRESS_START      0x01 /* title_menu1.tm2 */
#define TITLE_TEXTURE_MENU_CHOICE      0x02 /* title_menu2..5.tm2, one per menu choice */
#define TITLE_TEXTURE_COPYRIGHT        0x06 /* copy.tm2 */
#define TITLE_TEXTURE_TITLE_LOGO_LEFT  0x07 /* title_logo1.tm2 */
#define TITLE_TEXTURE_TITLE_LOGO_RIGHT 0x08 /* title_logo2.tm2 */
#define TITLE_TEXTURE_PICTURE_LEFT     0x09 /* itimaie_00.tm2, the background picture in three strips */
#define TITLE_TEXTURE_PICTURE_MIDDLE   0x0A /* itimaie_01.tm2 */
#define TITLE_TEXTURE_PICTURE_RIGHT    0x0B /* itimaie_02.tm2 */
#define TITLE_TEXTURE_LOGO2            0x0C /* logo2.tm2 */
#define TITLE_TEXTURE_GAME_OVER_LOAD   0x10 /* gameover_load.tm2 */
#define TITLE_TEXTURE_GAME_OVER_QUIT   0x11 /* gameover_quit.tm2 */
#define TITLE_TEXTURE_GAME_OVER_OK     0x12 /* gameover_ok.tm2 */
#define TITLE_TEXTURE_GAME_OVER        0x16 /* gameover.tm2 */

/* how one sprite of the title screens changes over time: four keys */
typedef struct {
    int key_frames[4];              /* 0x00 the frame each key starts at */
    int key_values[4];              /* 0x10 the brightness at each key, 0..0x80 */
    int length;                     /* 0x20 the frame counter starts over after this many frames */
} BootSpriteTrack;

/* one sprite of the title screens, set up by Title_SetSprite and drawn by Title_DrawSprite */
typedef struct {
    int texture;                    /* 0x00 index into title_textures */
    int pos_x;                      /* 0x04 left edge, from the screen centre */
    int pos_y;                      /* 0x08 top edge in frame lines (halved when drawn), from the screen centre */
    int width;                      /* 0x0C */
    int height;                     /* 0x10 in frame lines */
    int frame;                      /* 0x14 counts up to track->length */
    BootSpriteTrack *track;         /* 0x18 */
} BootSprite;

/* one texture of the title screens, loaded by Title_LoadTexture */
typedef struct {
    BootTex0 tex0;                  /* 0x00 GS TEX0, copied from the file */
    TextureHeader *file;            /* 0x08 the file as loaded */
} BootTexture;

/* the GS packet Title_DrawSprite writes to the start of sprite_packet */
typedef struct {
    u32 tag[4];                     /* 0x00 GIF tag: two address+data pairs */
    u64 alpha;                      /* 0x10 from sprite_blend_alpha by blend mode */
    u64 alpha_reg;                  /* 0x18 0x42, ALPHA_1 */
    u64 test;                       /* 0x20 */
    u64 test_reg;                   /* 0x28 0x47, TEST_1 */
    u32 list_tag[4];                /* 0x30 GIF tag: a list of eight registers */
    u32 prim[2];                    /* 0x40 a textured sprite with alpha blending */
    BootTex0 tex0;                  /* 0x48 */
    u32 rgba;                       /* 0x50 */
    f32 rgba_q;                     /* 0x54 */
    f32 first_s;                    /* 0x58 texture coordinates of the top left corner */
    f32 first_t;                    /* 0x5C */
    u32 first_xy;                   /* 0x60 x and y in 16ths of a pixel, y in the high half */
    u32 first_z;                    /* 0x64 */
    f32 second_s;                   /* 0x68 the bottom right corner */
    f32 second_t;                   /* 0x6C */
    u32 second_xy;                  /* 0x70 */
    u32 second_z;                   /* 0x74 */
    u32 nop[2];                     /* 0x78 */
} BootSpritePacket;

int Title_Run(int screen);
int Title_Init(void);
void Title_LoadTexture(char *name, int index);
int Title_UploadTexture(int index);
void Title_SetSprite(int texture, int pos_x, int pos_y, int width, int height, BootSprite *sprite,
                        BootSpriteTrack *track);
int Title_DrawSprite(float scale, BootSprite *sprite, int blend);
int Title_SendPacket(u32 packet, u32 qword_count);
void Title_SwapBuffers(void);
void Title_PlayMusic(void);
void Title_StopMusic(void);
void Title_InitLogos(void);
int Title_UpdateLogos(void);
int Title_InitPressStart(void);
int Title_UpdatePressStart(void);
int Title_InitMenu(void);
int Title_UpdateMenu(void);
int Title_RunGameOver(void);
int Title_InitGameOver(void);
int Title_UpdateGameOver(void);

#endif
