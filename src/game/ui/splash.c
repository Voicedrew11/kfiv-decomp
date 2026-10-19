#include "ui/splash.h"
#include "core/file.h"
#include "core/game.h"
#include "gfx/gs.h"

void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
int sprintf(char *, const char *, ...);

extern GameStatus game_status;
extern int bgm_load_pending[3];
extern int suspended_updates[3];
extern u16 pad_pressed[];
extern u16 action_button;
extern u32 *splash_image_packet;

void File_Lock(void);
void File_Unlock(void);
int File_IsLocked(void);
int File_Close(int);
int File_Read(int, void *, int);
void Packet_AppendToChain(u32 *, u32 *);
u32 *Packet_Alloc(int);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_QueueTexFlush(void);
void GsDraw_QueueTexturedSprite(void *);
void GsDraw_FillScreen(int, int, int, int, s64);
void Player_StopWalking(void);
static void Splash_BuildImagePacket(int tbp, int cbp);

static s32 splash_state;              /* 0x447358 state machine index (0 = idle) */
static s32 splash_image_id;           /* 0x44735C msg/mNNNN.tm2 index */
static s32 splash_image_file_open;    /* 0x447360 nonzero while the image file is open */
static s32 splash_brightness;         /* 0x447364 fade brightness 0..0x80 */
static s32 splash_start_delay;        /* 0x447368 frames before loading (state 1) */
static s32 splash_end_delay;          /* 0x44736C frames before release (state 6) */
static s32 splash_hold_frames;        /* 0x447370 hold timer; negative means forever */
static s32 splash_skippable;          /* 0x447374 button mask to skip the hold */
static s32 splash_fade_in_frames;     /* 0x447378 */
static s32 splash_fade_in_counter;    /* 0x44737C */
static s32 splash_fade_out_frames;    /* 0x447380 */
static s32 splash_fade_out_counter;   /* 0x447384 */

static SplashSprite splash_sprite;    /* 0x5F9B00 GS sprite packet for the splash image */

void Splash_StateIdle(void);
void Splash_StateStartDelay(void);
void Splash_StateLoadImage(void);
void Splash_StateFadeIn(void);
void Splash_StateHold(void);
void Splash_StateFadeOut(void);
void Splash_StateClose(void);

static void (*splash_state_table[8])(void) = { /* 0x42C008 */
    Splash_StateIdle,
    Splash_StateStartDelay,
    Splash_StateLoadImage,
    Splash_StateFadeIn,
    Splash_StateHold,
    Splash_StateFadeOut,
    Splash_StateClose,
    NULL,
};

/* 0x42C028 the bits per pixel of each pixel format (& 7), read by Splash_BuildImagePacket */
static int psm_bits_per_pixel[6] = { 0x20, 0x18, 0x10, 0x08, 0x04, 0x00 };

/* 0x446E84 where Splash_BuildImagePacket builds the image packet: 0x80000 into the file-load buffer */
u32 *splash_image_packet = (u32 *)((char *)&file_load_buffer + 0x80000);

/* Clears splash state (idle, no image id, file not open). */
void Splash_Reset(void) {
    splash_state = 0;
    splash_image_id = 0;
    splash_image_file_open = 0;
}

/* Returns the splash state (0 = idle). */
int Splash_GetState(void) {
    return splash_state;
}

/* Starts splash image splash_id with default hold, fade and skip settings. */
int Splash_Start(int splash_id) {
    return Splash_Begin(splash_id, -1, 0, 0x10, 0x10);
}

/* Starts splash image splash_id: hold_frames (<0 = forever), skippable, fade-in and fade-out frames. Returns 1 if started. */
int Splash_Begin(int splash_id, int hold_frames, int skippable, int fade_in_frames, int fade_out_frames) {
    int skip_value;
    int fade_in;
    int fade_out;

    if (!game_status.bit2 && !(game_status.bit3 && game_status.save_requested) && !(game_status.bit4 && !game_status.bit8) &&
        !game_status.bit14) {
        if (splash_state != 0) {
            return 0;
        }
        if (game_status.update_hold_flags != 0) {
            return 0;
        }
        splash_start_delay = 2;
        splash_state = 1;
        skip_value = skippable != 0 ? skippable : 1;
        if (hold_frames < 0) {
            skippable = skip_value;
        }
        fade_in = fade_in_frames > 0 ? fade_in_frames : 1;
        fade_out = fade_out_frames > 0 ? fade_out_frames : 1;
        splash_image_id = splash_id;
        splash_hold_frames = hold_frames;
        splash_skippable = skippable;
        splash_fade_in_frames = fade_in;
        splash_fade_out_frames = fade_out;
        splash_fade_in_counter = 0;
        splash_fade_out_counter = 0;
        game_status.update_hold_flags = 0x87F;
        Player_StopWalking();
        return 1;
    }
    return 0;
}

/* Aborts the splash image: jumps to the close state (6) with a short end delay. */
void Splash_Abort(void) {
    if (splash_state != 0) {
        splash_state = 6;
        splash_end_delay = 2;
    }
}

/* Run the handler for the splash image's current state. */
void Splash_Update(void) {
    splash_state_table[splash_state]();
}

/* State 0: idle, no work. */
void Splash_StateIdle(void) {
}

/* State 1: count down the start delay, then advance to loading the image. */
void Splash_StateStartDelay(void) {
    Player_StopWalking();
    splash_start_delay -= 1;
    if (splash_start_delay < 0) {
        splash_state += 1;
    }
}

/* Loads the splash image file into file_load_buffer and sets up its display. */
void Splash_StateLoadImage(void) {
    char image_file_name[256];
    int image_fd;

    if (bgm_load_pending[0] != 0) {
        return;
    }
    if (File_IsLocked() != 0) {
        return;
    }
    sprintf(image_file_name, "msg/m%4.4d.tm2", splash_image_id);
    image_fd = File_Open(image_file_name, 0, 1);
    if (image_fd >= 0) {
        File_Lock();
        splash_image_file_open = 1;
        memset(&file_load_buffer, 0, 0x80000);
        File_Read(image_fd, &file_load_buffer, 0x80000);
        File_Close(image_fd);
        Splash_BuildImagePacket(0x2E40, 0x3D40);
        Splash_BuildSprite();
        splash_state += 1;
    } else {
        splash_state = 0;
        suspended_updates[0] = 0;
        splash_image_file_open = 0;
    }
}

/* State 3: fade the image in over the fade-in frames. */
void Splash_StateFadeIn(void) {
    int counter;

    counter = splash_fade_in_counter + 1;
    splash_fade_in_counter = counter;
    if (counter < splash_fade_in_frames) {
        splash_brightness = (int)(counter << 7) / (int)splash_fade_in_frames;
        return;
    }
    splash_brightness = 0x80;
    splash_state += 1;
}

/* State 4: hold the image until the hold time runs out or a skippable button is pressed. */
void Splash_StateHold(void) {
    if (splash_hold_frames > 0) {
        splash_hold_frames -= 1;
        if (splash_hold_frames < 1) {
            splash_state += 1;
        }
    }
    if (splash_skippable != 0 && (pad_pressed[0] & action_button) != 0) {
        splash_state += 1;
    }
}

/* State 5: fade the image out over the fade-out frames, then advance to closing. */
void Splash_StateFadeOut(void) {
    splash_fade_out_counter += 1;
    if (splash_fade_out_counter < splash_fade_out_frames) {
        splash_brightness = 0x80 - (splash_fade_out_counter << 7) / splash_fade_out_frames;
        return;
    }
    splash_end_delay = 2;
    splash_brightness = 0;
    splash_state += 1;
}

/* State 6: counts down the end delay, then releases the splash and closes its file. */
void Splash_StateClose(void) {
    splash_end_delay -= 1;
    if (splash_end_delay < 0) {
        suspended_updates[0] = 0;
        splash_state = 0;
        if (splash_image_file_open != 0) {
            File_Unlock();
            splash_image_file_open = 0;
        }
    }
}

/* Draw the splash image while it is on screen (states 3..5). */
void Splash_Draw(void) {
    u32 *packet;
    u32 *cursor;

    if (splash_state - 3U < 3) {
        GsDraw_FillScreen(0, 0, 0, splash_brightness / 2, 0x44);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_TEST_1, 0x3000D);
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
        Packet_AddGsRegister(8, 5);
        Packet_EndGsRegisters();

        packet = cursor = Packet_Alloc(2);
        *cursor++ = *splash_image_packet | 0x30000000;
        *cursor++ = (u32)(splash_image_packet + 4);
        *cursor++ = 0;
        *cursor++ = 0;
        Packet_AppendToChain(packet, cursor);
        GsDraw_QueueTexFlush();

        splash_sprite.alpha = splash_brightness;
        GsDraw_QueueTexturedSprite(&splash_sprite);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
        Packet_AddGsRegister(8, 0);
        Packet_EndGsRegisters();
    }
}

/* Build splash_sprite for the image at file_load_buffer. */
void Splash_BuildSprite(void) {
    int height;
    int width;
    u64 tex0;

    width = 1 << ((file_load_buffer.tex0 >> 26) & 0xF);
    height = 1 << ((file_load_buffer.tex0 >> 30) & 0xF);

    memset(&splash_sprite, 0, sizeof(splash_sprite));
    tex0 = file_load_buffer.tex0;
    tex0 &= 0xFFFFFFFFFFFFC000UL;
    tex0 |= 0x2E40;
    tex0 &= 0xFFF8001FFFFFFFFFUL;
    tex0 |= 0xF500UL << 35;
    splash_sprite.tex0 = tex0;
    splash_sprite.unk08 = file_load_buffer.unk18;
    splash_sprite.gif_reg = 0x156;
    splash_sprite.red = 0x80;
    splash_sprite.green = 0x80;
    splash_sprite.blue = 0x80;
    splash_sprite.vertex0_x = (0x800 - width / 2) * 16;
    splash_sprite.vertex0_y = (0x800 - height / 2) * 16;
    splash_sprite.vertex0_z = 0xFFFFFFFF;
    splash_sprite.vertex0_s = 0;
    splash_sprite.vertex0_t = 0;
    splash_sprite.vertex1_x = (width / 2) * 16 + 0x7FF8;
    splash_sprite.vertex1_y = (height / 2) * 16 + 0x7FF8;
    splash_sprite.vertex1_z = 0xFFFFFFFF;
    splash_sprite.vertex1_s = width * 16;
    splash_sprite.vertex1_t = height * 16;
}

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Builds, at splash_image_packet, the DMA packet that sends the splash image to GS memory at `tbp`, and its palette
   (when it has one) to `cbp`. */
static void Splash_BuildImagePacket(int tbp, int cbp) {
    SplashImageBits *image;
    SplashLoadPacket *packet;
    char *source;
    char *output;
    u32 *cursor;
    int width;
    int height;
    int bits;
    int size;
    int qwords;
    int palette_qwords;
    int palette_width;
    int palette_height;
    int palette_buffer_width;
    int total;
    int has_palette;
    int psm;
    u64 format;

    image = (SplashImageBits *)&file_load_buffer;
    psm = image->tex0.psm;
    width = 1 << image->tex0.tw;
    height = 1 << image->tex0.th;
    bits = psm_bits_per_pixel[psm & 7];
    size = width * height * bits / 8;
    qwords = (size + 15) / 16;
    if (bits == 4) {
        palette_qwords = 2;
        if (image->tex0.cpsm == 0) {
            palette_qwords = 4;
        }
        palette_width = 8;
        palette_height = 2;
    } else {
        palette_qwords = 0x20;
        if (image->tex0.cpsm == 0) {
            palette_qwords = 0x40;
        }
        palette_width = 16;
        palette_height = 16;
    }
    palette_buffer_width = 1;
    has_palette = bits <= 8;
    if (has_palette) {
        total = (qwords + 7) + (palette_qwords + 7);
    } else {
        total = qwords + 7;
    }

    /* the image: a DMA tag, then the same layout as SplashLoadPacket, written word by word */
    cursor = splash_image_packet;
    *cursor++ = (total + 1) | 0x10000000;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = total | 0x51000000;
    *((u64 *)cursor)++ = 0x1000000000000005;
    *cursor++ = 0xE;
    *cursor++ = 0;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = 0x3F;
    *cursor++ = 0;
    format = image->tex0.psm;
    *((u64 *)cursor)++ = ((u64)tbp << 32) | ((u64)image->tex0.tbw << 48) | (format << 56);
    *cursor++ = 0x50;
    *cursor++ = 0;
    *((u64 *)cursor)++ = 0;
    *cursor++ = 0x51;
    *cursor++ = 0;
    *((u64 *)cursor)++ = width | ((u64)height << 32);
    *cursor++ = 0x52;
    *cursor++ = 0;
    *((u64 *)cursor)++ = 0;
    *cursor++ = 0x53;
    *cursor++ = 0;
    if (has_palette) {
        *((u64 *)cursor)++ = qwords | 0x800000000000000;
    } else {
        *((u64 *)cursor)++ = (qwords | 0x8000) | 0x800000000000000;
    }
    *cursor++ = 0;
    *cursor++ = 0;
    source = (char *)&file_load_buffer;
    source += sizeof(TextureHeader);
    memcpy(cursor, source, size);
    output = (char *)cursor + size;
    output = align16(output);

    if (has_palette) {
        packet = (SplashLoadPacket *)output;
        packet->tag[0] = 0x1000000000000005;
        packet->tag[1] = 0xE;
        packet->texflush[0] = 0xFFFFFFFF;
        packet->texflush[1] = 0;
        packet->texflush_reg[0] = 0x3F;
        packet->texflush_reg[1] = 0;
        format = image->tex0.cpsm;
        // match: the width has to be a variable, a literal 1 << 48 is combined with the base pointer first
        packet->bitbltbuf = ((u64)cbp << 32) | (((u64)palette_buffer_width << 48) | (format << 56));
        packet->bitbltbuf_reg[0] = 0x50;
        packet->bitbltbuf_reg[1] = 0;
        packet->trxpos = 0;
        packet->trxpos_reg[0] = 0x51;
        packet->trxpos_reg[1] = 0;
        packet->trxreg = palette_width | ((u64)palette_height << 32);
        packet->trxreg_reg[0] = 0x52;
        packet->trxreg_reg[1] = 0;
        packet->trxdir = 0;
        packet->trxdir_reg[0] = 0x53;
        packet->trxdir_reg[1] = 0;
        packet->image_tag = (palette_qwords | 0x8000) | 0x800000000000000;
        packet->image_tag_high[0] = 0;
        packet->image_tag_high[1] = 0;
        output += sizeof(SplashLoadPacket);
        source = (char *)&file_load_buffer;
        source += image->image_size + sizeof(TextureHeader);
        memcpy(output, source, palette_qwords << 4);
        // match: nothing reads this, but with no statement after it the call above becomes a tail jump
        output += palette_qwords << 4;
    }
}
