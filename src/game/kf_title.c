#include "kf_title.h"
#include "kf_file.h"
#include "kf_game.h"
#include "kf_pad.h"
#include "kf_sound.h"
#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>
#include "kf_menu_screens.h"

#define PAD_UP    0x0010
#define PAD_DOWN  0x0040
#define PAD_START 0x0008
#define PAD_CROSS 0x4000

#define LOGO_FRAMES         300  /* each logo stays this long */
#define PRESS_START_FRAMES  3600 /* the PRESS START screen goes back to the logos after this long */
#define TITLE_FADE_FRAMES   30

extern char *D_00446F50;

int func_001CED78(int, char *, int);
s32 Sound_PlayMenu(s32);
int func_001CEBE0(void);
void Texture_SendTexFlush(void);

typedef void (*TitleScreenInit)(void);

static int (*screen_updates[4])(void) = { /* 0x42C1A0 */
    Title_UpdateLogos,
    Title_UpdatePressStart,
    Title_UpdateMenu,
    NULL,
};
static TitleScreenInit screen_inits[4] = { /* 0x42C1B0 */
    Title_InitLogos,
    (TitleScreenInit)Title_InitPressStart,
    (TitleScreenInit)Title_InitMenu,
    NULL,
};
static u64 sprite_blend_alpha[4] = { 0x48, 0x42, 0x44, 0x0A }; /* 0x42C1C0 GS ALPHA_1 by blend mode */

static BootSpriteTrack logo_track = { { 0, 60, 240, 300 }, { 0, 96, 96, 0 }, 300 };                  /* 0x42C1E0 */
static BootSpriteTrack logo2_track = { { 0, 60, 240, 300 }, { 0, 96, 96, 0 }, 300 };                 /* 0x42C208 */
static BootSpriteTrack picture_left_track = { { 0, 32, 64, 100 }, { 32, 32, 32, 0 }, 64 };           /* 0x42C230 */
static BootSpriteTrack picture_middle_track = { { 0, 32, 64, 100 }, { 32, 32, 32, 0 }, 64 };         /* 0x42C258 */
static BootSpriteTrack picture_right_track = { { 0, 32, 64, 100 }, { 32, 32, 32, 0 }, 64 };          /* 0x42C280 */
static BootSpriteTrack title_logo_left_track = { { 0, 32, 64, 100 }, { 128, 128, 128, 0 }, 64 };     /* 0x42C2A8 */
static BootSpriteTrack title_logo_right_track = { { 0, 32, 64, 100 }, { 128, 128, 128, 0 }, 64 };    /* 0x42C2D0 */
static BootSpriteTrack press_start_track = { { 0, 60, 120, 999 }, { 32, 96, 32, 0 }, 120 };          /* 0x42C2F8 */
static BootSpriteTrack copyright_track = { { 0, 32, 64, 100 }, { 96, 96, 96, 0 }, 64 };              /* 0x42C320 */
static BootSpriteTrack menu_choice_tracks[4] = {                                                     /* 0x42C348 */
    { { 0, 60, 120, 999 }, { 32, 96, 32, 96 }, 0 },
    { { 0, 60, 120, 999 }, { 32, 96, 32, 96 }, 0 },
    { { 0, 60, 120, 999 }, { 32, 96, 32, 96 }, 0 },
    { { 0, 60, 120, 999 }, { 32, 96, 32, 96 }, 0 },
};
static BootSpriteTrack menu_confirm_track = { { 0, 2, 4, 100 }, { 64, 96, 64, 96 }, 4 };             /* 0x42C3D8 */
static BootSpriteTrack game_over_choice_tracks[2] = {                                                /* 0x42C400 */
    { { 0, 60, 120, 999 }, { 32, 96, 32, 96 }, 0 },
    { { 0, 60, 120, 999 }, { 32, 96, 32, 96 }, 0 },
};
static BootSpriteTrack game_over_confirm_track = { { 0, 2, 4, 100 }, { 64, 96, 64, 96 }, 4 };        /* 0x42C448 */
static BootSpriteTrack game_over_ok_track = { { 0, 32, 64, 100 }, { 64, 64, 64, 64 }, 64 };          /* 0x42C470 */
static BootSpriteTrack game_over_track = { { 0, 32, 64, 100 }, { 32, 32, 32, 32 }, 64 };             /* 0x42C498 */
static sceGsDBuff title_dbuff = { 0 };                                                               /* 0x42C4C0 */

static int display_buffer_index = 0;                   /* 0x446E88 the frame buffer drawn to next */
static u16 menu_held_stick = 0xFFFF;                  /* 0x446E8C PAD_UP/PAD_DOWN from the stick last frame */
static u16 game_over_held_stick = 0xFFFF;             /* 0x446E8E */

static TextureHeader *texture_cursor;                  /* 0x4473B4 where the next texture file is loaded */
static int texture_cursor_unused;                      /* 0x4473B8 never used by this code */
static u32 *packet_cursor;                             /* 0x4473BC */
static int packet_cursor_unused;                       /* 0x4473C0 never used by this code */
static int (*current_screen_update)(void);             /* 0x4473C4 one frame of the screen shown; nonzero ends Title_Run */
static int screen_timer;                               /* 0x4473C8 frames left on the screen, or of its fade-out */
static int logo_phase;                                 /* 0x4473CC 0 while logo2 shows, then 1 for logo */
static s8 press_start_fading_out;                      /* 0x4473D0 */
static int menu_choice;                                /* 0x4473D4 */
static s8 menu_confirmed;                              /* 0x4473D8 */
static int game_over_choice;                           /* 0x4473DC 0 load, 1 quit */
static s8 game_over_confirmed;                         /* 0x4473E0 */

static Uint128 sprite_packet[0x100];                   /* 0x5F9B60 */
static BootTexture title_textures[32];                 /* 0x5FAB60 */
static BootSprite logo_sprite;                         /* 0x5FAD60 */
static BootSprite logo2_sprite;                        /* 0x5FAD80 */
static BootSprite picture_left_sprite;                 /* 0x5FADA0 */
static BootSprite picture_middle_sprite;               /* 0x5FADC0 */
static BootSprite picture_right_sprite;                /* 0x5FADE0 */
static BootSprite title_logo_left_sprite;              /* 0x5FAE00 */
static BootSprite title_logo_right_sprite;             /* 0x5FAE20 */
static BootSprite press_start_sprite;                  /* 0x5FAE40 */
static BootSprite copyright_sprite;                    /* 0x5FAE60 */
static BootSprite menu_choice_sprites[4];              /* 0x5FAE80 */
static BootSprite game_over_choice_sprites[2];         /* 0x5FAEF0 */
static BootSprite game_over_ok_sprite;                 /* 0x5FAF28 */
static BootSprite game_over_sprite;                    /* 0x5FAF48 */

/* Runs the title screens from `screen` (0 logos, 1 PRESS START, 2 menu) until one returns a result. */
int Title_Run(int screen) {
    int result;

    Title_Init();
    current_screen_update = screen_updates[screen];
    screen_inits[screen]();
    do {
        result = current_screen_update();
        Title_SwapBuffers();
    } while (result == 0);
    Title_StopMusic();
    return result;
}

/* Sets up the display for the title screens and loads their thirteen textures. */
int Title_Init(void) {
    sceGsSyncV(0);
    sceGsResetGraph(0, 1, 2, 1);
    sceGsSetDefDBuff(&title_dbuff, 0, 640, 224, 3, 0, 1);
    title_dbuff.clear1.rgbaq = title_dbuff.clear0.rgbaq = 0x180000000;
    ((int (*)(void))Title_SwapBuffers)(); // match: called as if it returned a value, which keeps $2 busy
    texture_cursor = (TextureHeader *)D_00446F50;
    Title_LoadTexture("title\\logo.tm2", TITLE_TEXTURE_LOGO);
    Title_LoadTexture("title\\title_menu1.tm2", TITLE_TEXTURE_PRESS_START);
    Title_LoadTexture("title\\title_menu2.tm2", TITLE_TEXTURE_MENU_CHOICE + 0);
    Title_LoadTexture("title\\title_menu3.tm2", TITLE_TEXTURE_MENU_CHOICE + 1);
    Title_LoadTexture("title\\title_menu4.tm2", TITLE_TEXTURE_MENU_CHOICE + 2);
    Title_LoadTexture("title\\title_menu5.tm2", TITLE_TEXTURE_MENU_CHOICE + 3);
    Title_LoadTexture("title\\copy.tm2", TITLE_TEXTURE_COPYRIGHT);
    Title_LoadTexture("title\\title_logo1.tm2", TITLE_TEXTURE_TITLE_LOGO_LEFT);
    Title_LoadTexture("title\\title_logo2.tm2", TITLE_TEXTURE_TITLE_LOGO_RIGHT);
    Title_LoadTexture("title\\itimaie_00.tm2", TITLE_TEXTURE_PICTURE_LEFT);
    Title_LoadTexture("title\\itimaie_01.tm2", TITLE_TEXTURE_PICTURE_MIDDLE);
    Title_LoadTexture("title\\itimaie_02.tm2", TITLE_TEXTURE_PICTURE_RIGHT);
    Title_LoadTexture("title\\logo2.tm2", TITLE_TEXTURE_LOGO2);
    Texture_SendTexFlush();
    return 0;
}

/* Loads a texture file of the title screens to the free memory at texture_cursor. */
void Title_LoadTexture(char *name, int index) {
    int file;
    int size;

    file = func_00102238(name, 0, 1);
    size = func_001025C0(file, texture_cursor, -1);
    func_001024B0(file);
    title_textures[index].file = texture_cursor;
    title_textures[index].tex0 = *(BootTex0 *)&texture_cursor->tex0;
    texture_cursor = (TextureHeader *)((char *)texture_cursor + size);
}

/* Sends a texture of the title screens to GS memory, and its palette when it has 8-bit pixels. */
int Title_UploadTexture(int index) {
    sceGsLoadImage load_image;
    int width;
    int height;

    width = 1 << title_textures[index].tex0.tw;
    height = 1 << title_textures[index].tex0.th;
    sceGsSetDefLoadImage(&load_image, title_textures[index].tex0.tbp0, title_textures[index].tex0.tbw,
                         title_textures[index].tex0.psm, 0, 0, width, height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)title_textures[index].file + sizeof(TextureHeader));
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    if (title_textures[index].tex0.psm == 0x13) {
        sceGsSetDefLoadImage(&load_image, title_textures[index].tex0.cbp, 1, title_textures[index].tex0.cpsm, 0, 0, 16, 16);
        FlushCache(0);
        sceGsExecLoadImage(&load_image,
                           (char *)title_textures[index].file + (title_textures[index].file->image_size + sizeof(TextureHeader)));
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
        }
    }
    Texture_SendTexFlush();
    return 0;
}

void Title_SetSprite(int texture, int pos_x, int pos_y, int width, int height, BootSprite *sprite,
                        BootSpriteTrack *track) {
    sprite->track = track;
    sprite->texture = texture;
    sprite->pos_x = pos_x;
    sprite->pos_y = pos_y;
    sprite->width = width;
    sprite->height = height;
    sprite->frame = 0;
}

/* Draws a sprite of the title screens: sends its texture, then a sprite packet (BootSpritePacket) written through
   packet_cursor. Its brightness is the track's value at the sprite's frame, times scale. */
int Title_DrawSprite(float scale, BootSprite *sprite, int blend) {
    int alpha;
    int next;
    int i;

    Title_UploadTexture(sprite->texture);
    for (i = 3; i >= 0; i--) {
        if (sprite->frame >= sprite->track->key_frames[i]) {
            // match: at the last key, key_frames[next] and key_values[next] are past their arrays (the ratio is 0 there)
            next = i + 1;
            alpha = sprite->track->key_values[next - 1] +
                    (sprite->track->key_values[next] - sprite->track->key_values[next - 1]) *
                        ((f32)(sprite->frame - sprite->track->key_frames[i]) /
                         (f32)(sprite->track->key_frames[next] - sprite->track->key_frames[i]));
            break;
        }
    }
    alpha *= scale;

    packet_cursor = (u32 *)sprite_packet;
    *packet_cursor++ = 0x8001;
    *packet_cursor++ = 0x20000000;
    *packet_cursor++ = 0xEE;
    *packet_cursor++ = 0;
    *((u64 *)packet_cursor)++ = sprite_blend_alpha[blend];
    *((u64 *)packet_cursor)++ = 0x42;
    *((u64 *)packet_cursor)++ = 0x3000D;
    *((u64 *)packet_cursor)++ = 0x47;
    *packet_cursor++ = 0x8001;
    *packet_cursor++ = 0x84000000;
    *packet_cursor++ = 0xF5252160;
    *packet_cursor++ = 0;
    *packet_cursor++ = 0x56;
    *packet_cursor++ = 0;
    *((BootTex0 *)packet_cursor)++ = title_textures[sprite->texture].tex0;
    *packet_cursor++ = (alpha << 24) + 0x808080;
    *((f32 *)packet_cursor)++ = 1.0f;
    *((f32 *)packet_cursor)++ = 0.0f;
    *((f32 *)packet_cursor)++ = 0.0f;
    *packet_cursor++ = ((sprite->pos_x + 0x800) << 4) + ((sprite->pos_y / 2 + 0x800) << 20);
    *packet_cursor++ = 0;
    *((f32 *)packet_cursor)++ = 1.0f;
    *((f32 *)packet_cursor)++ = 1.0f;
    *packet_cursor++ = ((sprite->pos_x + sprite->width + 0x800) << 4) + (((sprite->pos_y + sprite->height) / 2 + 0x800) << 20);
    *packet_cursor++ = 0xFFFF;
    *packet_cursor++ = 0;
    *packet_cursor++ = 0;
    Title_SendPacket((u32)sprite_packet, (u32)(packet_cursor - (u32 *)sprite_packet) / 4);
    sprite->frame++;
    if (sprite->track->length < sprite->frame) {
        sprite->frame = 0;
    }
    return 0;
}

/* Sends a packet to the GS through DMA channel 2 and waits for it. */
int Title_SendPacket(u32 packet, u32 qword_count) {
    FlushCache(0);
    while (*D2_CHCR & 0x100);
    *D2_QWC = qword_count;
    *D2_MADR = packet;
    *D2_CHCR = 0x100;
    sceGsSyncPath(0, 0);
    return 0;
}

/* Waits for the vertical blank and shows the frame just drawn. */
void Title_SwapBuffers(void) {
    int odd_field = !sceGsSyncV(0);

    sceGsSetHalfOffset(display_buffer_index == 1 ? &title_dbuff.draw1 : &title_dbuff.draw0, 0x800, 0x800, odd_field);
    FlushCache(0);
    sceGsSwapDBuff(&title_dbuff, display_buffer_index);
    display_buffer_index = 1 - display_buffer_index;
}

/* Starts the title music stream, TITLE.ADS, unless it is already playing. */
void Title_PlayMusic(void) {
    if (func_001CBAE0(1, 0x3090, 0) == 0) {
        func_001CED78(0, "cdrom0:\\DATA\\TITLE.ADS;1", 0x10000);
        do {
        } while (func_001CBAE0(1, 0x30B0) == 0);
        func_001CBAE0(1, 0x3020, 0, 1);
    }
}

/* Stops the title music stream. */
void Title_StopMusic(void) {
    func_001CBAE0(1, 0x3050, 0);
    func_001CBAE0(1, 0x3030, 0);
}

/* Sets up the logo screen: logo2, then logo, for LOGO_FRAMES each. */
void Title_InitLogos(void) {
    screen_timer = LOGO_FRAMES;
    Title_SetSprite(TITLE_TEXTURE_LOGO, -0x100, -0x20, 0x200, 0x40, &logo_sprite, &logo_track);
    Title_SetSprite(TITLE_TEXTURE_LOGO2, -0x100, -0x80, 0x200, 0x100, &logo2_sprite, &logo2_track);
    logo_phase = 0;
}

/* One frame of the logo screen; after both logos, or on START, goes on to PRESS START. */
int Title_UpdateLogos(void) {
    PadReport *report;
    int phase;

    screen_timer--;
    Pad_Update();
    report = Pad_GetReport(0, 0);
    phase = logo_phase;
    if (phase == 0 && screen_timer <= 0) {
        phase = 1;
        screen_timer = LOGO_FRAMES;
        logo_phase = 1;
    }
    if (phase == 0) {
        Title_DrawSprite(1.0f, &logo2_sprite, 0);
    } else {
        Title_DrawSprite(1.0f, &logo_sprite, 0);
    }
    // match: retail returns early here rather than guarding the switch below
    if (!((report->pressed & PAD_START) || (screen_timer <= 0 && logo_phase == 1))) {
        return 0;
    }
    current_screen_update = Title_UpdatePressStart;
    Title_InitPressStart();
    return 0;
}

/* Sets up the seven sprites of the PRESS START screen and starts the title music. */
int Title_InitPressStart(void) {
    screen_timer = PRESS_START_FRAMES;
    Title_SetSprite(TITLE_TEXTURE_PRESS_START, -0x80, 0x40, 0x100, 0x40, &press_start_sprite, &press_start_track);
    Title_SetSprite(TITLE_TEXTURE_COPYRIGHT, -0x140, 0xB4, 0x280, 0x20, &copyright_sprite, &copyright_track);
    Title_SetSprite(TITLE_TEXTURE_TITLE_LOGO_LEFT, -0x100, -0xC0, 0x100, 0x100, &title_logo_left_sprite, &title_logo_left_track);
    Title_SetSprite(TITLE_TEXTURE_TITLE_LOGO_RIGHT, 0, -0xC0, 0x100, 0x100, &title_logo_right_sprite, &title_logo_right_track);
    Title_SetSprite(TITLE_TEXTURE_PICTURE_LEFT, -0x140, -0xE0, 0x100, 0x200, &picture_left_sprite, &picture_left_track);
    Title_SetSprite(TITLE_TEXTURE_PICTURE_MIDDLE, -0x40, -0xE0, 0x100, 0x200, &picture_middle_sprite, &picture_middle_track);
    Title_SetSprite(TITLE_TEXTURE_PICTURE_RIGHT, 0xC0, -0xE0, 0x80, 0x200, &picture_right_sprite, &picture_right_track);
    press_start_fading_out = FALSE;
    Title_PlayMusic();
    return 0;
}

/* One frame of the PRESS START screen: it fades in over 30 frames and out over the last 30, then the
   logos return; START goes on to the menu. */
int Title_UpdatePressStart(void) {
    PadReport *pad;
    f32 brightness;

    screen_timer--;
    Pad_Update();
    brightness = 1.0f;
    pad = Pad_GetReport(0, 0);
    if (screen_timer > PRESS_START_FRAMES - TITLE_FADE_FRAMES) {
        brightness = (3600.0f - screen_timer) / 30.0f;
    }
    if (screen_timer <= TITLE_FADE_FRAMES) {
        press_start_fading_out = TRUE;
        brightness = screen_timer / 30.0f;
        if (screen_timer <= 0) {
            current_screen_update = Title_UpdateLogos;
            Title_StopMusic();
            Title_InitLogos();
            return 0;
        }
    }
    Title_DrawSprite(brightness, &picture_left_sprite, 2);
    Title_DrawSprite(brightness, &picture_middle_sprite, 2);
    Title_DrawSprite(brightness, &picture_right_sprite, 2);
    Title_DrawSprite(brightness, &press_start_sprite, 0);
    Title_DrawSprite(brightness, &copyright_sprite, 0);
    Title_DrawSprite(brightness, &title_logo_left_sprite, 2);
    Title_DrawSprite(brightness, &title_logo_right_sprite, 2);
    if (press_start_fading_out == FALSE && (pad->pressed & PAD_START)) {
        current_screen_update = Title_UpdateMenu;
        Title_InitMenu();
    }
    return 0;
}

/* Sets up the sprites of the menu: four choices in a column, the one Menu_CardsHaveSaves returns pulsing. */
int Title_InitMenu(void) {
    int i;

    Title_SetSprite(TITLE_TEXTURE_TITLE_LOGO_LEFT, -0x100, -0xC0, 0x100, 0x100, &title_logo_left_sprite, &title_logo_left_track);
    Title_SetSprite(TITLE_TEXTURE_TITLE_LOGO_RIGHT, 0, -0xC0, 0x100, 0x100, &title_logo_right_sprite, &title_logo_right_track);
    Title_SetSprite(TITLE_TEXTURE_MENU_CHOICE + 0, -0x80, 0x20, 0x100, 0x40, &menu_choice_sprites[0], &menu_choice_tracks[0]);
    Title_SetSprite(TITLE_TEXTURE_MENU_CHOICE + 1, -0x80, 0x40, 0x100, 0x40, &menu_choice_sprites[1], &menu_choice_tracks[1]);
    Title_SetSprite(TITLE_TEXTURE_MENU_CHOICE + 2, -0x80, 0x60, 0x100, 0x40, &menu_choice_sprites[2], &menu_choice_tracks[2]);
    Title_SetSprite(TITLE_TEXTURE_MENU_CHOICE + 3, -0x80, 0x80, 0x100, 0x40, &menu_choice_sprites[3], &menu_choice_tracks[3]);
    Title_SetSprite(TITLE_TEXTURE_COPYRIGHT, -0x140, 0xB4, 0x280, 0x20, &copyright_sprite, &copyright_track);
    Title_SetSprite(TITLE_TEXTURE_PICTURE_LEFT, -0x140, -0xE0, 0x100, 0x200, &picture_left_sprite, &picture_left_track);
    Title_SetSprite(TITLE_TEXTURE_PICTURE_MIDDLE, -0x40, -0xE0, 0x100, 0x200, &picture_middle_sprite, &picture_middle_track);
    Title_SetSprite(TITLE_TEXTURE_PICTURE_RIGHT, 0xC0, -0xE0, 0x80, 0x200, &picture_right_sprite, &picture_right_track);
    menu_choice = Menu_CardsHaveSaves();
    menu_confirmed = FALSE;
    for (i = 0; i < 4; i++) {
        if (menu_choice == i) {
            menu_choice_tracks[i].length = 120;
        } else {
            menu_choice_tracks[i].length = 0;
        }
    }
    Title_PlayMusic();
    Pad_Update();
    Pad_GetReport(0, 0);
    return 0;
}

/* One frame of the menu: moves the choice with the pad, and once one is confirmed
   fades the screen out and returns the choice plus one. Returns 0 until then. */
int Title_UpdateMenu(void) {
    f32 brightness;
    PadReport *report;
    u16 stick;
    u16 stick_pressed;
    int i;

    Pad_Update();
    brightness = 1.0f;
    report = Pad_GetReport(0, 0);
    // match: pressed is read through the union below, so that the reads alias the stores
    stick = (report->left_stick_y > 0x3FU) ? 0 : PAD_UP;
    if (report->left_stick_y > 0xC0) {
        stick |= PAD_DOWN;
    }
    stick_pressed = stick & ~menu_held_stick;
    menu_held_stick = stick;
    if (menu_confirmed) {
        screen_timer--;
        if (screen_timer <= 0) {
            menu_held_stick = 0xFFFF;
            return menu_choice + 1;
        }
        brightness = screen_timer / 30.0f;
    } else if (((PadReportUnion *)report)->pressed.buttons & (PAD_CROSS | PAD_START)) {
        menu_confirmed = TRUE;
        screen_timer = TITLE_FADE_FRAMES;
        menu_choice_sprites[menu_choice].track = &menu_confirm_track;
    } else if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & (PAD_UP | PAD_DOWN)) {
        menu_choice_tracks[menu_choice].length = 0;
        if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & PAD_UP) {
            menu_choice--;
        } else if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & PAD_DOWN) {
            menu_choice++;
        }
        menu_choice &= 3;
        menu_choice_sprites[menu_choice].frame = 60;
        menu_choice_tracks[menu_choice].length = 120;
    }
    Title_DrawSprite(brightness, &picture_left_sprite, 2);
    Title_DrawSprite(brightness, &picture_middle_sprite, 2);
    Title_DrawSprite(brightness, &picture_right_sprite, 2);
    Title_DrawSprite(brightness, &title_logo_left_sprite, 2);
    Title_DrawSprite(brightness, &title_logo_right_sprite, 2);
    for (i = 0; i < 4; i++) {
        Title_DrawSprite(brightness, &menu_choice_sprites[i], 0);
    }
    Title_DrawSprite(brightness, &copyright_sprite, 0);
    return 0;
}

/* Runs the game-over screen until a choice is confirmed; returns 1 for load, 2 for quit. */
int Title_RunGameOver(void) {
    int result;

    Title_InitGameOver();
    do {
        result = Title_UpdateGameOver();
        Title_SwapBuffers();
    } while (result == 0);
    return result;
}

/* Sets up the game-over screen: its frame buffers, its four textures and its sprites. */
int Title_InitGameOver(void) {
    int i;

    sceGsSyncV(0);
    sceGsResetGraph(0, 1, 2, 1);
    sceGsSetDefDBuff(&title_dbuff, 0, 640, 224, 3, 0, 1);
    title_dbuff.clear1.rgbaq = title_dbuff.clear0.rgbaq = 0x180000000L;
    ((int (*)(void))Title_SwapBuffers)(); // match: called as if it returned a value, which keeps $2 busy
    texture_cursor = (TextureHeader *)D_00446F50;
    Title_LoadTexture("title\\gameover_load.tm2", TITLE_TEXTURE_GAME_OVER_LOAD);
    Title_LoadTexture("title\\gameover_quit.tm2", TITLE_TEXTURE_GAME_OVER_QUIT);
    Title_LoadTexture("title\\gameover_ok.tm2", TITLE_TEXTURE_GAME_OVER_OK);
    Title_LoadTexture("title\\gameover.tm2", TITLE_TEXTURE_GAME_OVER);
    Texture_SendTexFlush();
    game_over_choice = 0;
    game_over_confirmed = FALSE;
    Title_SetSprite(TITLE_TEXTURE_GAME_OVER_LOAD, -0x80, -0x30, 0x100, 0x40, &game_over_choice_sprites[0], &game_over_choice_tracks[0]);
    Title_SetSprite(TITLE_TEXTURE_GAME_OVER_QUIT, -0x80, -0x10, 0x100, 0x40, &game_over_choice_sprites[1], &game_over_choice_tracks[1]);
    Title_SetSprite(TITLE_TEXTURE_GAME_OVER_OK, -0x40, 0x94, 0x80, 0x40, &game_over_ok_sprite, &game_over_ok_track);
    Title_SetSprite(TITLE_TEXTURE_GAME_OVER, -0x80, -0x80, 0x100, 0x100, &game_over_sprite, &game_over_track);
    for (i = 0; i < 2; i++) {
        if (game_over_choice == i) {
            game_over_choice_tracks[i].length = 120;
        } else {
            game_over_choice_tracks[i].length = 0;
        }
    }
    Pad_Update();
    Pad_GetReport(0, 0);
    return 0;
}

/* One frame of the game-over screen: moves the choice with the pad, and once one is confirmed
   fades the screen out and returns the choice plus one. Returns 0 until then. */
int Title_UpdateGameOver(void) {
    f32 brightness;
    PadReport *report;
    u16 stick;
    u16 stick_pressed;
    int i;

    Pad_Update();
    brightness = 1.0f;
    report = Pad_GetReport(0, 0);
    stick = (report->left_stick_y > 0x3FU) ? 0 : PAD_UP;
    if (report->left_stick_y > 0xC0) {
        stick |= PAD_DOWN;
    }
    stick_pressed = stick & ~game_over_held_stick;
    game_over_held_stick = stick;
    if (game_over_confirmed) {
        screen_timer--;
        if (screen_timer <= 0) {
            game_over_held_stick = 0xFFFF;
            return game_over_choice + 1;
        }
        brightness = screen_timer / 30.0f;
    } else if (report->pressed & (PAD_CROSS | PAD_START)) {
        Sound_PlayMenu(0);
        game_over_confirmed = TRUE;
        screen_timer = TITLE_FADE_FRAMES;
        game_over_choice_sprites[game_over_choice].track = &game_over_confirm_track;
    } else if ((stick_pressed | report->pressed) & (PAD_UP | PAD_DOWN)) {
        Sound_PlayMenu(1);
        game_over_choice_tracks[game_over_choice].length = 0;
        // match: read through the union, or the reload moves ahead of the store above
        if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & PAD_UP) {
            game_over_choice--;
        } else if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & PAD_DOWN) {
            game_over_choice++;
        }
        game_over_choice &= 1;
        game_over_choice_sprites[game_over_choice].frame = 60;
        game_over_choice_tracks[game_over_choice].length = 120;
    }
    for (i = 0; i < 2; i++) {
        Title_DrawSprite(brightness, &game_over_choice_sprites[i], 0);
    }
    Title_DrawSprite(brightness, &game_over_sprite, 0);
    Title_DrawSprite(brightness, &game_over_ok_sprite, 0);
    func_001CEBE0();
    return 0;
}
