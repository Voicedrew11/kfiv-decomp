#include "kf_game.h"
#include "kf_gold.h"
#include "kf_held_weapon.h"
#include "kf_entity_world.h"
#include "kf_hud.h"
#include "kf_item_effect.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "kf_player.h"
#include "kf_player_update.h"
#include "kf_sound.h"
#include "kf_linked_point.h"
#include "kf_splash.h"
#include "kf_message.h"
#include "kf_effect.h"

extern u8 D_00349680[0x10];
extern PlayTime D_0036A138[];   /* the play time in the loaded save (D_0036A130 + 8) */
extern SoundLoadQueue D_0038A130;
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern MapLoadState D_0038D610;
extern u16 D_00413DA8[];
extern PlayerState D_00413F80;
extern f32 D_004323E0[16];
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern int D_00446734;
extern u32 D_00446ED4;

void func_00112948(int);
int func_001D20A0(void);
void func_001D2048(PlayTime);
void func_00106178(int, int, int, int);
void func_001095E0(void);
void Display_SwapBuffers(int);
int Timer_GetTicks(void);
void Timer_Restart(void);
void func_001096E8(void);
void func_0010B000(void);
void func_00111D58(void);
void func_001B5460(void);
int Script_UpdateAll(void);
void func_00100D10(void);
void func_001D0970(void);
void func_00117CC8(void);
void func_00112BD0(void);
void func_00123288(void);
void func_001C95B0(void);
void Player_UseItem(void);
void func_00101618(void);
void Sound_Update(void);
void func_001061E8(void);
void func_00100FC8(void);
void func_001012D8(int, int);
void func_0010A7E0(void *);
int func_001005E8(int);
void func_00109800(void);
void func_001C9C00(void);
void func_001161A8(void);
void func_001017A8(void);
void func_00118958(void);
void func_00101F90(void);
void func_001B5778(void);
void Sound_SetBgmVolume(int);
void Sound_SetBankVolumes(int);
int sceGsSyncV(int);
int func_0019F6B0(void);
void func_00102558(void);

/* The game proper: places the player (new game) or resumes a loaded one, runs the main loop until
   GameStatus.bit9 is set, then waits for the sound loads to end. */
void func_001001C0(int arg) {
    MapSlotUnk08 *header;
    u8 saved;

    func_00112948(arg);
    if (D_00446734 == 0) {
        func_001D20A0();
        header = (MapSlotUnk08 *)D_0038CD20[0].header;
        D_00413F80.position.x = header->unk60;
        D_00413F80.position.y = header->unk64;
        D_00413F80.position.z = header->unk68;
        D_00413F80.rotation.y = header->unk6C;
    } else {
        D_00446734 = 0;
        func_001D2048(D_0036A138[0]);
        func_00106178(0, 0, 0, 0x1E);
    }
    D_0038D610.flags.raw |= 1;
    func_001095E0();
    func_001095E0();
    while (!D_0038CC50.bit9) {
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            Display_SwapBuffers(1);
        }
        D_00446ED4++;
        Timer_GetTicks();
        Timer_Restart();
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            func_001096E8();
            Timer_GetTicks();
            func_001095E0();
            func_0010B000();
            func_00111D58();
            func_001B5460();
        }
        if (D_0038CC50.bit12 && !D_0038CC50.bit13) {
            if (D_00413DA8[1] & 8) {
                D_00413DA8[1] = 0;
                D_0038CC50.bit13 = TRUE;
                D_00413DA8[0] = 0;
            }
        }
        if (!(D_0038CC50.unk30 & 0x40) && D_00413F80.unk1A8 == 0) {
            Script_UpdateAll();
        }
        Player_Update();
        func_00100D10();
        func_001D0970();
        func_00117CC8();
        Player_UpdateArmModels();
        func_00112BD0();
        MapObject_UpdateAll();
        func_00123288();
        Entity_UpdateAll();
        func_001C95B0();
        Gold_Update();
        Player_UseItem();
        Player_UpdateWeaponAim();
        Effect_UpdateAll();
        LinkedPoint_UpdateFollows();
        Hud_Update();
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            func_00101618();
            Sound_Update();
        }
        func_001061E8();
        Splash_Update();
        ItemEffect_Update();
        func_00100FC8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        WeaponTrail_Update();
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            func_001005E8(1);
            func_00109800();
        }
        func_001C9C00();
        func_001161A8();
        func_001017A8();
        func_00118958();
        func_00101F90();
    }
    D_00432E20[0] = 0xC000L << 17;
    D_00432E20[30] = 0xC000L << 17;
    D_00433050[0] = 0xC000L << 17;
    D_00433050[30] = 0xC000L << 17;
    func_001B5778();
    Message_CloseBox();
    Message_ClearNotices();
    saved = D_00349680[1];
    D_00349680[1] = 0;
    Sound_SetBgmVolume(0);
    Sound_SetBankVolumes(0);
    while (D_0038A130.unk0C != 0 ||
           ((D_0038A130.unk04 != 0 || D_0038A130.unk18 != 0) &&
            (D_0038A130.unk18 != 0 || D_0038A130.unk04 == 0 || func_0019F6B0() == 0))) {
        while (sceGsSyncV(0) != 1) {
        }
        Timer_Restart();
        func_00101618();
        Sound_Update();
    }
    D_00349680[1] = saved;
    D_0038CC50.bit9 = FALSE;
    func_00102558();
    func_001095E0();
    func_00109800();
}
