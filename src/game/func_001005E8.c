#include "kf_game.h"
#include "kf_entity_world.h"
#include "kf_hud.h"
#include "kf_item_effect.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "kf_player.h"
#include "kf_player_update.h"
#include "kf_gold.h"
#include "kf_held_weapon.h"
#include "kf_splash.h"
#include "kf_message.h"
#include "kf_effect.h"

extern u8 D_00349680[0x10];
extern u8 D_00349F56[];
extern MapDrawStateOverlays D_0038A1B0;
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;
extern f32 D_004323E0[16];
extern int D_00446708;
extern int D_00446770;

void func_001112B0(void);
void func_001095D0(void);
void func_0010A008(void);
void func_001012D8(int, int);
void func_0010A7E0(void *);
void func_0010AE58(void);
void func_00114198(int);
void func_001C9650(int);
void func_00114528(int);
void func_0010B140(u32, u32);
void func_00109EB8(void);
void func_001C97E8(int);
void func_00116AA0(void);
void func_0010A698(void);
void Entity_DrawPoolModels(void);
void func_00116970(void);
void func_0010A5D8(void);
void func_00109E88(void);
void func_0010A040(int);
void func_00108020(int, int, int, int, float);
void func_001BBA58(void);
void func_00106520(void);
void func_00106980(void);
void func_00106C58(void);
void func_00106DC8(void);
void func_00106FB8(int);
void func_00107528(void);
void func_0010A4E8(int, int, int, int, int);
void func_00107E70(int, int, int);
void func_00106208(void);
void func_00107CD8(int);
void func_001095D8(void);

/* Draws one frame of the game: the world (once, or with a second view first for the modes 1 and 2), the screen
   tints, the overlays MapDrawState asks for, the fades and the pause shade. */
int func_001005E8(int arg) {
    int tinted = FALSE;
    f32 tint;
    int red;
    int green;
    int blue;

    func_001112B0();
    func_001095D0();
    func_0010A008();
    switch (D_0038A1B0.mode) {
    case 0:
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        func_00114198(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        func_001C9650(0);
        ItemEffect_Draw();
        Gold_Draw(0);
        func_0010AE58();
        func_00114528(0);
        Effect_DrawAll(0);
        func_0010B140(0, 0);
        func_001012D8(0, 2);
        func_0010A7E0(D_004323E0);
        func_0010B140(1, 0);
        func_0010B140(2, 0);
        func_00109EB8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_001C97E8(0);
        break;
    case 1:
        func_001012D8(0, 1);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        func_00114198(1);
        MapObject_DrawAll(1, 0, 0);
        Entity_DrawAll(1);
        func_001C9650(1);
        Gold_Draw(1);
        func_00114528(1);
        func_0010AE58();
        Effect_DrawAll(1);
        func_0010B140(0, 1);
        func_00109EB8();
        func_001012D8(0, 1);
        func_0010A7E0(D_004323E0);
        func_001C97E8(1);
        func_00116AA0();
        func_0010A698();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        func_00114198(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        func_001C9650(0);
        ItemEffect_Draw();
        Gold_Draw(0);
        func_0010AE58();
        func_00114528(0);
        Effect_DrawAll(0);
        func_0010B140(0, 0);
        func_001012D8(0, 2);
        func_0010A7E0(D_004323E0);
        func_0010B140(1, 0);
        func_0010B140(2, 0);
        func_00109EB8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_001C97E8(0);
        break;
    case 2:
        func_00116970();
        func_0010A5D8();
        func_001012D8(1, 1);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        func_00114198(2);
        MapObject_DrawAll(1, 0, 0);
        Entity_DrawAll(1);
        func_001C9650(1);
        ItemEffect_Draw();
        Gold_Draw(1);
        func_0010AE58();
        func_00114528(2);
        Effect_DrawAll(1);
        func_0010B140(0, 1);
        func_00109EB8();
        func_001012D8(1, 1);
        func_0010A7E0(D_004323E0);
        func_001C97E8(1);
        func_0010A008();
        func_00109E88();
        func_0010A698();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        func_00114198(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        func_001C9650(0);
        Gold_Draw(0);
        func_0010AE58();
        func_00114528(0);
        Effect_DrawAll(0);
        func_0010B140(0, 0);
        func_001012D8(0, 2);
        func_0010A7E0(D_004323E0);
        func_0010B140(1, 0);
        func_0010B140(2, 0);
        func_00109EB8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_001C97E8(0);
        break;
    }
    func_001012D8(0, 0);
    func_0010A7E0(D_004323E0);
    if ((s32)(D_00413F80.unk1C8 >> 39) & 1) {
        func_0010A040(0x2BC0);
        tinted = TRUE;
        func_00108020(0x80, 0x80, 0x80, 0x80, 1.0f);
    }
    if (D_00413F80.unk29C != 0) {
        tint = (f32)D_00413F80.unk29C * 0.125f;
        red = (int)(tint * -58.0f + 128.0f);
        green = (int)(tint * -78.0f + 128.0f);
        blue = (int)(tint * 12.0f + 128.0f);
        func_0010A040(0x2BC0);
        tinted = TRUE;
        func_00108020(red, green, blue, 0x80, tint);
    }
    func_001BBA58();
    if (D_0038A1B0.bit44) {
        func_00106520();
    }
    if (D_0038A1B0.bit41) {
        func_00106980();
    }
    if (D_0038A1B0.bit42) {
        func_00106C58();
    }
    if (D_0038A1B0.bit43) {
        func_00106DC8();
    }
    if (D_0038A1B0.bit40) {
        func_00106FB8(D_0038A1B0.unk88);
    }
    if (D_00413F80.aim_speed > 0.0f) {
        func_00106FB8(0x40);
    }
    if (arg != 0 && D_0038A1B0.bit45) {
        func_00107528();
    }
    func_0010A698();
    if (tinted) {
        Effect_UploadTextures();
    }
    func_0010B140(3, 0);
    Player_DrawArm();
    Player_DrawScreenTints();
    if ((s32)(D_00413F80.unk1C8 >> 39) & 1) {
        if (D_0038CD20[D_00446770].tag == 0x23 && D_00349F56[0] == 0) {
            func_0010A4E8(0x40, 0x80, 0x40, 0x78, 0x44);
        } else {
            func_00107E70(0x5E, 0x87, 0x92);
        }
    }
    Hud_Draw();
    if (D_0038A1B0.unk78 > 0) {
        // match: the last argument is passed as the 64-bit word
        ((void (*)(int, int, int, int, s64))func_0010A4E8)(D_0038A1B0.unk6C, D_0038A1B0.unk70, D_0038A1B0.unk74,
                                                             D_0038A1B0.unk78, D_0038A1B0.unk80);
    }
    func_00106208();
    Splash_Draw();
    if (Message_IsBoxOpen() != 0) {
        if (D_00446708 < 0x40) {
            D_00446708 += 0x10;
            if (D_00446708 > 0x40) {
                D_00446708 = 0x40;
            }
        }
    } else if (D_00446708 > 0) {
        D_00446708 -= 0x10;
        if (D_00446708 < 0) {
            D_00446708 = 0;
        }
    }
    if (D_00446708 != 0) {
        func_0010A4E8(0, 0, 0, D_00446708, 0x44);
    }
    Message_Update();
    if (D_0038CC50.paused) {
        func_0010A4E8(0, 0, 0, 0x40, 0x44);
        Hud_DrawPaused();
    }
    func_00107CD8(D_00349680[10]);
    func_001095D8();
}
