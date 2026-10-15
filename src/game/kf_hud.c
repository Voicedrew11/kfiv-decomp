#include "kf_hud.h"
#include "kf_file.h"
#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_texture.h"

void TextureTable_SetBasePointers(char *, int, int);
void func_001D1378(ScreenSprite *, s64, u64, u64, u8, u8, u8, u8, s32, s32, u32, s32, s32, s32, s32, s32, s32);
void TextureTable_QueueAll(TextureTableHeader *);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_QueueTexturedSprite(void *);

extern char game_options[0x10]; // match: read as a signed byte here
extern s32 D_0038A218[];
extern PlayerState player_state;
extern char *D_00446F6C;        /* the HUD area, where gage.tx2 is loaded */

static TextureTableHeader *pause_texture_table; /* 0x4471F0 pause.tx2, loaded after gage.tx2 */
static ScreenSprite hud_sprites[10];            /* 0x5CDE18 the frame and bars of the HUD */
static ScreenSprite pause_sprite;               /* 0x5CE098 the pause label */

/* Fill a screen sprite: GS state, colour, and a pos_x/pos_y/width/height box (in pixels) with its texture box. */
void func_001D1378(ScreenSprite *sprite, s64 prim, u64 tex0, u64 tex1, u8 red, u8 green, u8 blue, u8 alpha,
                   s32 pos_x, s32 pos_y, u32 depth, s32 width, s32 height, s32 first_s, s32 first_t, s32 second_s,
                   s32 second_t) {
    sprite->prim = prim;
    sprite->tex0 = tex0;
    sprite->tex1 = tex1;
    sprite->rgba[0] = red;
    sprite->rgba[1] = green;
    sprite->rgba[2] = blue;
    sprite->rgba[3] = alpha;
    pos_x += 0x10;
    pos_y += 0x10;
    sprite->vertex[0].x = (pos_x + 0x6C0) << 4;
    sprite->vertex[0].y = (pos_y + 0x720) << 4;
    width += 0x6C0;
    height += 0x720;
    sprite->vertex[1].x = (pos_x + width) << 4;
    sprite->vertex[1].y = (pos_y + height) << 4;
    sprite->vertex[0].z = depth;
    sprite->vertex[0].tex_t = first_t << 4;
    sprite->vertex[1].z = depth;
    sprite->vertex[0].tex_s = first_s << 4;
    sprite->vertex[1].tex_s = second_s << 4;
    sprite->vertex[1].tex_t = second_t << 4;
}

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Loads the HUD textures (gage.tx2) into the HUD area and the pause label (pause.tx2) after them, uploads the
   latter at once and sets up the sprites drawn from them. */
void Hud_Load(void) {
    char *address = D_00446F6C;
    int file;
    int size;

    file = File_Open("gage.tx2", 0, 1);
    size = File_Read(file, address, -1);
    File_Close(file);
    address += size;
    address = align16(address);
    pause_texture_table = (TextureTableHeader *)address;
    file = File_Open("pause.tx2", 0, 1);
    File_Read(file, address, -1);
    File_Close(file);
    TextureTable_SetBasePointers((char *)pause_texture_table, 0x2BC0, 0x3FE0);
    func_001D1378(&hud_sprites[0], 0x156, 0x4007DA85E1313640, 0x60, 0x5A, 0x5A, 0x5A, 0x80,
                  0, 0, 0, 0xDA, 0x5A, 0, 0, 0x100, 0x80);
    func_001D1378(&hud_sprites[1], 0x156, 0x8007DC8599307780, 0x60, 0x6C, 0x2A, 0xD, 0x80,
                  0x56, 0x15, 0, 0x7C, 0xA, 0, 0, 0x40, 0x14);
    func_001D1378(&hud_sprites[2], 0x156, 0x8007DC8599307780, 0x60, 0x39, 0x51, 0x6D, 0x80,
                  0x56, 0x20, 0, 0x7C, 0xB, 0, 0x14, 0x40, 0x28);
    func_001D1378(&hud_sprites[3], 0x156, 0x8007DB85DD30B700, 0x60, 0x6D, 0x26, 0, 0x80,
                  0x17, 0x14, 0, 0x33, 0x31, 0, 0, 0x80, 0x80);
    func_001D1378(&hud_sprites[4], 0x156, 0x8007DD05DD30B540, 0x60, 0x77, 0x7D, 0x52, 0x80,
                  0x17, 0x14, 0, 0x33, 0x31, 0, 0, 0x80, 0x80);
    func_001D1378(&hud_sprites[5], 0x156, 0x8007DA05DD30B740, 0, 0x80, 0x80, 0x80, 0x80,
                  5, 4, 0, 0x50, 0x52, 0, 0, 0x80, 0x80);
    func_001D1378(&hud_sprites[6], 0x156, 0x8007DB05DD30B6C0, 0x60, 0x44, 0x2E, 0x4A, 0x80,
                  5, 4, 0, 0x50, 0x52, 0, 0, 0x80, 0x80);
    func_001D1378(&hud_sprites[7], 0x156, 0x8007DC8599307780, 0x60, 0, 0xB4, 0xD0, 0x80,
                  0x6E, 0x33, 0, 0x33, 4, 0, 0x14, 0x40, 0x28);
    func_001D1378(&hud_sprites[8], 0x156, 0x8007DB85DD30B700, 0x60, 0, 0, 0, 0x80,
                  0x5A, 0x33, 0, 0xB, 0xC, 0, 0, 0x80, 0x80);
    func_001D1378(&hud_sprites[9], 0x156, 0x8007DD05DD30B540, 0x60, 0x80, 0x80, 0x80, 0x50,
                  0x5A, 0x33, 0, 0xB, 0xC, 0, 0, 0x80, 0x80);
    func_001D1378(&pause_sprite, 0x156, 0x4007FC055D30ABC0, 0x60, 0x80, 0x80, 0x80, 0x80,
                  0x100, 0xD0, 0, 0x80, 0x20, 0, 0, 0x80, 0x20);
}

/* Per-frame HUD update; empty in retail. */
int Hud_Update(void) { // match: callers read $2 after it, so it is declared to return a value
}

/* Draws the HUD: the frame, the two bars that fill from the left, the one that fills from the bottom, and the
   rest, each a copy of one of the sprites Hud_Load set up. */
void Hud_Draw(void) {
    ScreenSprite sprite;
    s32 fade;

    if (game_options[9] != 0 && D_0038A218[0] != 0) {
        TextureTable_QueueAll((TextureTableHeader *)D_00446F6C);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
        Packet_AddGsRegister(8, 5);
        Packet_EndGsRegisters();
        sprite = hud_sprites[0];
        GsDraw_QueueTexturedSprite(&sprite);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(8, 0);
        Packet_EndGsRegisters();
        sprite = hud_sprites[1];
        sprite.vertex[1].x = sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * player_state.unkBC / 5000;
        sprite.vertex[1].tex_s =
            sprite.vertex[0].tex_s + (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * player_state.unkBC / 5000;
        GsDraw_QueueTexturedSprite(&sprite);
        sprite = hud_sprites[2];
        sprite.vertex[1].x = sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * player_state.unkC0 / 5000;
        sprite.vertex[1].tex_s =
            sprite.vertex[0].tex_s + (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * player_state.unkC0 / 5000;
        GsDraw_QueueTexturedSprite(&sprite);
        sprite = hud_sprites[3];
        sprite.vertex[0].y = sprite.vertex[1].y - (sprite.vertex[1].y - sprite.vertex[0].y) * player_state.unk1A4 / 49;
        sprite.vertex[0].tex_t =
            sprite.vertex[1].tex_t - (sprite.vertex[1].tex_t - sprite.vertex[0].tex_t) * player_state.unk1A4 / 49;
        GsDraw_QueueTexturedSprite(&sprite);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
        Packet_EndGsRegisters();
        sprite = hud_sprites[4];
        GsDraw_QueueTexturedSprite(&sprite);
        sprite = hud_sprites[5];
        sprite.prim = 0x146;
        sprite.rgba[3] = 0;
        sprite.rgba[2] = 0;
        sprite.rgba[1] = 0;
        sprite.rgba[0] = 0;
        GsDraw_QueueTexturedSprite(&sprite);
        if (player_state.mp_max > 0) {
            fade = player_state.unk1A6 * 255 / player_state.mp_max;
        } else {
            fade = 0;
        }
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_TEST_1, ((s64)fade << 4) | 0x30005);
        Packet_AddGsRegister(0x4A, 1);
        Packet_EndGsRegisters();
        sprite = hud_sprites[5];
        sprite.prim = 0x156;
        GsDraw_QueueTexturedSprite(&sprite);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x3D001);
        Packet_AddGsRegister(0x4A, 0);
        Packet_EndGsRegisters();
        sprite = hud_sprites[6];
        GsDraw_QueueTexturedSprite(&sprite);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
        Packet_EndGsRegisters();
        sprite = hud_sprites[7];
        if (player_state.unkD0 != 0) {
            sprite.vertex[1].x =
                sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * player_state.unkCC / player_state.unkD0;
            sprite.vertex[1].tex_s = sprite.vertex[0].tex_s +
                (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * player_state.unkCC / player_state.unkD0;
            GsDraw_QueueTexturedSprite(&sprite);
        }
        sprite = hud_sprites[8];
        GsDraw_QueueTexturedSprite(&sprite);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
        Packet_EndGsRegisters();
        sprite = hud_sprites[9];
        GsDraw_QueueTexturedSprite(&sprite);
    }
}

/* Draws the pause label over the paused game. */
void Hud_DrawPaused(void) {
    TextureTable_QueueAll(pause_texture_table);
    GsDraw_QueueTexturedSprite(&pause_sprite);
}
