#include "kf_hud.h"
#include "kf_file.h"
#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_texture.h"

void func_0010B510(char *, int, int);
void func_001D1378(ScreenSprite *, s64, u64, u64, u8, u8, u8, u8, s32, s32, u32, s32, s32, s32, s32, s32, s32);
void func_0010B238(TextureTableHeader *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);

extern char D_00349680[0x10]; // match: read as a signed byte here
extern s32 D_0038A218[];
extern PlayerState D_00413F80;
extern char *D_00446F6C;        /* the HUD area, where gage.tx2 is loaded */

static TextureTableHeader *pause_texture_table; /* 0x4471F0 pause.tx2, loaded after gage.tx2 */
static ScreenSprite hud_sprites[10];            /* 0x5CDE18 the frame and bars of the HUD */
static ScreenSprite pause_sprite;               /* 0x5CE098 the pause label */

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

    file = func_00102238("gage.tx2", 0, 1);
    size = func_001025C0(file, address, -1);
    func_001024B0(file);
    address += size;
    address = align16(address);
    pause_texture_table = (TextureTableHeader *)address;
    file = func_00102238("pause.tx2", 0, 1);
    func_001025C0(file, address, -1);
    func_001024B0(file);
    func_0010B510((char *)pause_texture_table, 0x2BC0, 0x3FE0);
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

    if (D_00349680[9] != 0 && D_0038A218[0] != 0) {
        func_0010B238((TextureTableHeader *)D_00446F6C);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_TEST_1, 0x31001);
        func_00109A80(8, 5);
        func_00109AD0();
        sprite = hud_sprites[0];
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(8, 0);
        func_00109AD0();
        sprite = hud_sprites[1];
        sprite.vertex[1].x = sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * D_00413F80.unkBC / 5000;
        sprite.vertex[1].tex_s =
            sprite.vertex[0].tex_s + (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * D_00413F80.unkBC / 5000;
        func_0010A100(&sprite);
        sprite = hud_sprites[2];
        sprite.vertex[1].x = sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * D_00413F80.unkC0 / 5000;
        sprite.vertex[1].tex_s =
            sprite.vertex[0].tex_s + (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * D_00413F80.unkC0 / 5000;
        func_0010A100(&sprite);
        sprite = hud_sprites[3];
        sprite.vertex[0].y = sprite.vertex[1].y - (sprite.vertex[1].y - sprite.vertex[0].y) * D_00413F80.unk1A4 / 49;
        sprite.vertex[0].tex_t =
            sprite.vertex[1].tex_t - (sprite.vertex[1].tex_t - sprite.vertex[0].tex_t) * D_00413F80.unk1A4 / 49;
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        sprite = hud_sprites[4];
        func_0010A100(&sprite);
        sprite = hud_sprites[5];
        sprite.prim = 0x146;
        sprite.rgba[3] = 0;
        sprite.rgba[2] = 0;
        sprite.rgba[1] = 0;
        sprite.rgba[0] = 0;
        func_0010A100(&sprite);
        if (D_00413F80.unk5C > 0) {
            fade = D_00413F80.unk1A6 * 255 / D_00413F80.unk5C;
        } else {
            fade = 0;
        }
        func_00109A50();
        func_00109A80(GS_REG_TEST_1, ((s64)fade << 4) | 0x30005);
        func_00109A80(0x4A, 1);
        func_00109AD0();
        sprite = hud_sprites[5];
        sprite.prim = 0x156;
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_TEST_1, 0x3D001);
        func_00109A80(0x4A, 0);
        func_00109AD0();
        sprite = hud_sprites[6];
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_TEST_1, 0x31001);
        func_00109AD0();
        sprite = hud_sprites[7];
        if (D_00413F80.unkD0 != 0) {
            sprite.vertex[1].x =
                sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * D_00413F80.unkCC / D_00413F80.unkD0;
            sprite.vertex[1].tex_s = sprite.vertex[0].tex_s +
                (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * D_00413F80.unkCC / D_00413F80.unkD0;
            func_0010A100(&sprite);
        }
        sprite = hud_sprites[8];
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        sprite = hud_sprites[9];
        func_0010A100(&sprite);
    }
}

/* Draws the pause label over the paused game. */
void Hud_DrawPaused(void) {
    func_0010B238(pause_texture_table);
    func_0010A100(&pause_sprite);
}
