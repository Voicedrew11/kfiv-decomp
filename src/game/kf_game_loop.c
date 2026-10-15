/* The game session: resetting the systems, the main loop, drawing a frame, the camera, the sound-load
   queue, opening menus between frames with their fades, and pause. */
#include "kf_gold.h"
#include "kf_held_weapon.h"
#include "kf_item_effect.h"
#include "kf_linked_point.h"
#include "kf_player.h"
#include "kf_shot.h"
#include "kf_splash.h"
#include "kf_effect.h"
#include "kf_game.h"
#include "kf_entity_world.h"
#include "kf_hud.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "kf_player_update.h"
#include "kf_sound.h"
#include "kf_message.h"
#include "kf_magic_ladder.h"
#include "kf_menu_screens.h"
#include "kf_packet.h"

void func_001029A8(void);
void func_001B5810(void);
void Ini_Load(int);
void func_001030B8(void);
void func_001BA310(void);
void Map_ResetGameState(void);
void Map_ClearSlots(void);
void MapLoad_Reset(void);
void MapViewer_Init(void);
void func_0011C6E8(void);
void MapItem_Init(void);
void func_001B4EE0(void);
int func_001060C0(void);
void func_00104EC8(void);
void func_001B5A10(void);
void Sound_LoadMgBank(int);
void Sound_ApplyStereoOption(void);
void Map_LoadAtOnce(int);
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
void MapLoad_Update(void);
void func_00112BD0(void);
void func_00123288(void);
void MapItem_UpdateFalling(void);
void Player_UseItem(void);
void func_00101618(void);
void Sound_Update(void);
void func_001061E8(void);
void func_00100FC8(void);
void func_001012D8(int, int);
void func_0010A7E0(void *);
int func_001005E8(int);
void func_00109800(void);
void MapItem_UpdatePickup(void);
void Map_RunFreeCamera(void);
void func_001017A8(void);
void MapChange_RunToEnd(void);
void func_00101F90(void);
void func_001B5778(void);
void Sound_SetBgmVolume(int);
void Sound_SetBankVolumes(int);
int sceGsSyncV(int);
int Sound_BgmLoadCanYield(void);
void func_00102558(void);
void func_001112B0(void);
void func_001095D0(void);
void func_0010A008(void);
void func_0010AE58(void);
void Map_DrawRecords(int);
void MapItem_DrawAll(int);
void Map_DrawLateRecords(int);
void func_0010B140(u32, u32);
void func_00109EB8(void);
void MapItem_DrawPicked(int);
void Map_CoverScreenWithMapTint(void);
void func_0010A698(void);
void Entity_DrawPoolModels(void);
void Map_SetOffscreenTarget(void);
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
MapRecordInfo *Map_FindRecordAtInSlot(CameraState *, f32, int);
int rand(void);
float Angle_Wrap(float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, int *, float *);
void Vector_Copy(void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
void Vector_Add(void *, void *, void *);
int Angle_Approach(float *, float, float);
float atan2f(float, float);
float sqrtf(float);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0InversMatrix(void *, void *);
void sceVu0MulMatrix(void *, void *, void *);
void Sound_StopAll(int);
int Sound_LoadMapBankStep(int, int);
int Sound_LoadEntityBankStep(int, int, int);
int Sound_LoadBgm(int, int);
void MapLoad_RequestHold(void);
int MapLoad_IsHeld(void);
int func_00102230(void);
void func_00102228(void);
void func_00101CE0(void);
void Trade_Run(s32);
void Smith_Run(void);
void SaveScreen_Run(void);
void MapViewer_OpenForItem(int);
void func_00101D48(int);
void func_00101C88(void);
void MapViewer_Run(void);
void func_00102218(void);
void MapLoad_Release(void);
void func_00101EF0(void);
void *memset(void *, int, u32);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);
void func_001D20E0(void);
void func_001B57D8(void);
void func_001B5800(void);
extern PlayerState player_state;
extern int D_00446734;          /* nonzero: a saved game was loaded */
extern u8 game_options[0x10];
extern PlayTime save_play_time[];   /* the play time in the loaded save (save_image + 8) */
extern SoundLoadQueue sound_load_queue;
extern GameStatus game_status;
extern MapSlot map_slots[];
extern MapLoadState map_load_state;
extern u16 pad_state[];
extern f32 D_004323E0[16];
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern u32 D_00446ED4;
extern u8 D_00349F56[];
extern MapDrawStateOverlays map_draw_state;
extern int D_00446708;
extern int D_00446770;
extern f32 D_00431B30[4][4];
extern f32 D_00431C80[4];
extern CameraState D_004325A0;
extern EventRunner D_00420D00;
extern f32 D_00431AF0[16];      /* the unit matrix */
extern f32 D_004322A0[16];      /* view to screen */
extern f32 D_00432320[16];      /* world to view */
extern f32 D_00432360[16];      /* the camera's rotation, inverted */
extern LevelRecord weapon_level_records[];
extern PlayerState player_previous;
extern ItemCommonDef item_common_defs[];
extern PlayerItemDef weapon_defs[];
extern int D_00446F78;
extern int D_00446F7C;
extern u16 D_00446946; // match: read as u16 (lhu) here; the save code declares it s16
extern PlayTime D_00446A28;

/* Resets every game system for a new session; a loaded save is then applied on top. */
void func_001000C8(int arg) {
    func_001029A8();
    func_001B5810();
    Ini_Load(arg);
    func_001030B8();
    func_001BA310();
    Map_ResetGameState();
    Shot_ClearAll();
    LinkedPoint_Init();
    Map_ClearSlots();
    MapLoad_Reset();
    MapViewer_Init();
    HeldWeapon_Init();
    Entity_ResetAllLists();
    func_0011C6E8();
    MapItem_Init();
    Effect_Init();
    func_001B4EE0();
    Gold_Init();
    Splash_Reset();
    ItemEffect_Reset();
    if (D_00446734 != 0) {
        func_001060C0();
        func_00104EC8();
    }
    func_001B5A10();
    Player_LoadArmModels();
    Sound_LoadMgBank(player_state.equipped_magic);
    Sound_ApplyStereoOption();
}

/* The game proper: places the player (new game) or resumes a loaded one, runs the main loop until
   GameStatus.bit9 is set, then waits for the sound loads to end. */
void func_001001C0(int arg) {
    MapSlotUnk08 *header;
    u8 saved;

    Map_LoadAtOnce(arg);
    if (D_00446734 == 0) {
        func_001D20A0();
        header = (MapSlotUnk08 *)map_slots[0].header;
        player_state.position.x = header->unk60;
        player_state.position.y = header->unk64;
        player_state.position.z = header->unk68;
        player_state.rotation.y = header->unk6C;
    } else {
        D_00446734 = 0;
        func_001D2048(save_play_time[0]);
        func_00106178(0, 0, 0, 0x1E);
    }
    map_load_state.flags.raw |= 1;
    func_001095E0();
    func_001095E0();
    while (!game_status.bit9) {
        if (!(game_status.bit12 && game_status.bit13)) {
            Display_SwapBuffers(1);
        }
        D_00446ED4++;
        Timer_GetTicks();
        Timer_Restart();
        if (!(game_status.bit12 && game_status.bit13)) {
            func_001096E8();
            Timer_GetTicks();
            func_001095E0();
            func_0010B000();
            func_00111D58();
            func_001B5460();
        }
        if (game_status.bit12 && !game_status.bit13) {
            if (pad_state[1] & 8) {
                pad_state[1] = 0;
                game_status.bit13 = TRUE;
                pad_state[0] = 0;
            }
        }
        if (!(game_status.unk30 & 0x40) && player_state.unk1A8 == 0) {
            Script_UpdateAll();
        }
        Player_Update();
        func_00100D10();
        func_001D0970();
        MapLoad_Update();
        Player_UpdateArmModels();
        func_00112BD0();
        MapObject_UpdateAll();
        func_00123288();
        Entity_UpdateAll();
        MapItem_UpdateFalling();
        Gold_Update();
        Player_UseItem();
        Player_UpdateWeaponAim();
        Effect_UpdateAll();
        LinkedPoint_UpdateFollows();
        Hud_Update();
        if (!(game_status.bit12 && game_status.bit13)) {
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
        if (!(game_status.bit12 && game_status.bit13)) {
            func_001005E8(1);
            func_00109800();
        }
        MapItem_UpdatePickup();
        Map_RunFreeCamera();
        func_001017A8();
        MapChange_RunToEnd();
        func_00101F90();
    }
    D_00432E20[0] = 0xC000L << 17;
    D_00432E20[30] = 0xC000L << 17;
    D_00433050[0] = 0xC000L << 17;
    D_00433050[30] = 0xC000L << 17;
    func_001B5778();
    Message_CloseBox();
    Message_ClearNotices();
    saved = game_options[1];
    game_options[1] = 0;
    Sound_SetBgmVolume(0);
    Sound_SetBankVolumes(0);
    while (sound_load_queue.unk0C != 0 ||
           ((sound_load_queue.unk04 != 0 || sound_load_queue.unk18 != 0) &&
            (sound_load_queue.unk18 != 0 || sound_load_queue.unk04 == 0 || Sound_BgmLoadCanYield() == 0))) {
        while (sceGsSyncV(0) != 1) {
        }
        Timer_Restart();
        func_00101618();
        Sound_Update();
    }
    game_options[1] = saved;
    game_status.bit9 = FALSE;
    func_00102558();
    func_001095E0();
    func_00109800();
}

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
    switch (map_draw_state.mode) {
    case 0:
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        Map_DrawRecords(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        MapItem_DrawAll(0);
        ItemEffect_Draw();
        Gold_Draw(0);
        func_0010AE58();
        Map_DrawLateRecords(0);
        Effect_DrawAll(0);
        func_0010B140(0, 0);
        func_001012D8(0, 2);
        func_0010A7E0(D_004323E0);
        func_0010B140(1, 0);
        func_0010B140(2, 0);
        func_00109EB8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        MapItem_DrawPicked(0);
        break;
    case 1:
        func_001012D8(0, 1);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        Map_DrawRecords(1);
        MapObject_DrawAll(1, 0, 0);
        Entity_DrawAll(1);
        MapItem_DrawAll(1);
        Gold_Draw(1);
        Map_DrawLateRecords(1);
        func_0010AE58();
        Effect_DrawAll(1);
        func_0010B140(0, 1);
        func_00109EB8();
        func_001012D8(0, 1);
        func_0010A7E0(D_004323E0);
        MapItem_DrawPicked(1);
        Map_CoverScreenWithMapTint();
        func_0010A698();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        Map_DrawRecords(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        MapItem_DrawAll(0);
        ItemEffect_Draw();
        Gold_Draw(0);
        func_0010AE58();
        Map_DrawLateRecords(0);
        Effect_DrawAll(0);
        func_0010B140(0, 0);
        func_001012D8(0, 2);
        func_0010A7E0(D_004323E0);
        func_0010B140(1, 0);
        func_0010B140(2, 0);
        func_00109EB8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        MapItem_DrawPicked(0);
        break;
    case 2:
        Map_SetOffscreenTarget();
        func_0010A5D8();
        func_001012D8(1, 1);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        Map_DrawRecords(2);
        MapObject_DrawAll(1, 0, 0);
        Entity_DrawAll(1);
        MapItem_DrawAll(1);
        ItemEffect_Draw();
        Gold_Draw(1);
        func_0010AE58();
        Map_DrawLateRecords(2);
        Effect_DrawAll(1);
        func_0010B140(0, 1);
        func_00109EB8();
        func_001012D8(1, 1);
        func_0010A7E0(D_004323E0);
        MapItem_DrawPicked(1);
        func_0010A008();
        func_00109E88();
        func_0010A698();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_0010AE58();
        Map_DrawRecords(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        MapItem_DrawAll(0);
        Gold_Draw(0);
        func_0010AE58();
        Map_DrawLateRecords(0);
        Effect_DrawAll(0);
        func_0010B140(0, 0);
        func_001012D8(0, 2);
        func_0010A7E0(D_004323E0);
        func_0010B140(1, 0);
        func_0010B140(2, 0);
        func_00109EB8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        MapItem_DrawPicked(0);
        break;
    }
    func_001012D8(0, 0);
    func_0010A7E0(D_004323E0);
    if ((s32)(player_state.unk1C8 >> 39) & 1) {
        func_0010A040(0x2BC0);
        tinted = TRUE;
        func_00108020(0x80, 0x80, 0x80, 0x80, 1.0f);
    }
    if (player_state.unk29C != 0) {
        tint = (f32)player_state.unk29C * 0.125f;
        red = (int)(tint * -58.0f + 128.0f);
        green = (int)(tint * -78.0f + 128.0f);
        blue = (int)(tint * 12.0f + 128.0f);
        func_0010A040(0x2BC0);
        tinted = TRUE;
        func_00108020(red, green, blue, 0x80, tint);
    }
    func_001BBA58();
    if (map_draw_state.bit44) {
        func_00106520();
    }
    if (map_draw_state.bit41) {
        func_00106980();
    }
    if (map_draw_state.bit42) {
        func_00106C58();
    }
    if (map_draw_state.bit43) {
        func_00106DC8();
    }
    if (map_draw_state.bit40) {
        func_00106FB8(map_draw_state.unk88);
    }
    if (player_state.aim_speed > 0.0f) {
        func_00106FB8(0x40);
    }
    if (arg != 0 && map_draw_state.bit45) {
        func_00107528();
    }
    func_0010A698();
    if (tinted) {
        Effect_UploadTextures();
    }
    func_0010B140(3, 0);
    Player_DrawArm();
    Player_DrawScreenTints();
    if ((s32)(player_state.unk1C8 >> 39) & 1) {
        if (map_slots[D_00446770].tag == 0x23 && D_00349F56[0] == 0) {
            func_0010A4E8(0x40, 0x80, 0x40, 0x78, 0x44);
        } else {
            func_00107E70(0x5E, 0x87, 0x92);
        }
    }
    Hud_Draw();
    if (map_draw_state.unk78 > 0) {
        // match: the last argument is passed as the 64-bit word
        ((void (*)(int, int, int, int, s64))func_0010A4E8)(map_draw_state.unk6C, map_draw_state.unk70, map_draw_state.unk74,
                                                             map_draw_state.unk78, map_draw_state.unk80);
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
    if (game_status.paused) {
        func_0010A4E8(0, 0, 0, 0x40, 0x44);
        Hud_DrawPaused();
    }
    func_00107CD8(game_options[10]);
    func_001095D8();
}

/* Finds the map record the camera is in and works out this frame's two camera shake offsets. */
void func_00100D10(void) {
    f32 matrix[4][4];
    MapRecordInfo *record;
    f32 angle;

    if (game_status.unk03 != 0) {
        record = Map_FindRecordAtInSlot(&D_004325A0, 100.0f, D_00446770);
        if (record != NULL) {
            player_state.map_index = (u32)((char *)record - map_slots[D_00446770].records) >> 14; // match: the byte offset of 0x80-byte records shifted by 14; 7 would give the record number
            player_state.unk250 = record->unk38;
        }
    }
    if (D_004325A0.unk74 != 0 && D_004325A0.unk70 > 0.0f) {
        angle = (f32)(int)((f32)rand() * 360.0f / 2147483648.0f) * 0.017453292f;
        angle = Angle_Wrap(angle + D_004325A0.unk18);
        D_004325A0.unk50[0] = D_004325A0.unk70;
        D_004325A0.unk50[1] = 0.0f;
        D_004325A0.unk50[2] = 0.0f;
        D_004325A0.unk50[3] = 1.0f;
        sceVu0CopyMatrix(matrix, D_00431B30);
        sceVu0RotMatrixZ(matrix[0], matrix[0], angle);
        sceVu0RotMatrixX(matrix[0], matrix[0], D_004325A0.unk10);
        sceVu0RotMatrixY(matrix[0], matrix[0], D_004325A0.unk14);
        sceVu0ApplyMatrix(D_004325A0.unk50, (int *)matrix, D_004325A0.unk50);
        if (D_004325A0.unk74 > 0) {
            D_004325A0.unk74--;
        }
    } else {
        Vector_Copy(D_004325A0.unk50, D_00431C80);
    }
    if (D_004325A0.unk7C != 0 && D_004325A0.unk78 > 0.0f) {
        angle = (f32)(int)((f32)rand() * 360.0f / 2147483648.0f) * 0.017453292f;
        angle = Angle_Wrap(angle + D_004325A0.unk18);
        D_004325A0.unk60[0] = D_004325A0.unk78;
        D_004325A0.unk60[1] = 0.0f;
        D_004325A0.unk60[2] = 0.0f;
        D_004325A0.unk60[3] = 1.0f;
        sceVu0CopyMatrix(matrix, D_00431B30);
        sceVu0RotMatrixZ(matrix[0], matrix[0], angle);
        sceVu0RotMatrixX(matrix[0], matrix[0], D_004325A0.unk10);
        sceVu0RotMatrixY(matrix[0], matrix[0], D_004325A0.unk14);
        sceVu0ApplyMatrix(D_004325A0.unk60, (int *)matrix, D_004325A0.unk60);
        if (D_004325A0.unk7C > 0) {
            D_004325A0.unk7C--;
        }
    } else {
        Vector_Copy(D_004325A0.unk60, D_00431C80);
    }
}

/* Moves and turns the camera for the current camera mode, then adds the two shake offsets. */
void func_00100FC8(void) {
    f32 unused_matrix[4][4];
    f32 direction[4];
    f32 target[4];
    f32 unused_vector[4];
    f32 yaw;
    f32 pitch;
    f32 length;

    switch (game_status.unk03) {
    case 0:
        Vector_Scale(target, D_00420D00.unk10, D_00420D00.unk40);
        Vector_Add(target, target, D_00420D00.unk00);
        D_004325A0.position[0] = player_state.position.x;
        D_004325A0.position[2] = player_state.position.z;
        D_004325A0.position[1] =
            player_state.position.y - 220.0f + player_state.unk1C4 + player_state.unk1F0 + D_004325A0.unk4C;
        Vector_Subtract(direction, target, D_004325A0.position);
        D_004325A0.unk10 =
            atan2f(-direction[1], sqrtf(direction[0] * direction[0] + direction[2] * direction[2]));
        D_004325A0.unk14 = atan2f(direction[0], direction[2]);
        D_004325A0.unk18 = 0.0f;
        break;
    case 1:
        if (game_status.unk05 != 0) {
            Vector_Subtract(direction, D_004325A0.unk30, D_004325A0.position);
            length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
            if (D_004325A0.unk44 < length) {
                Vector_Scale(direction, direction, D_004325A0.unk44 / length);
                Vector_Add(D_004325A0.position, D_004325A0.position, direction);
            } else {
                Vector_Copy(D_004325A0.position, D_004325A0.unk30);
            }
        }
        break;
    case 2:
        break;
    }
    if (game_status.unk03 != 0 && (s8)game_status.unk04 != 0) {
        Vector_Subtract(direction, D_004325A0.unk20, D_004325A0.position);
        yaw = atan2f(direction[0], direction[2]);
        length = sqrtf(direction[0] * direction[0] + direction[2] * direction[2]);
        pitch = atan2f(-direction[1], length);
        switch ((s8)game_status.unk04) {
        case 1:
            D_004325A0.unk10 = pitch;
            D_004325A0.unk14 = yaw;
            break;
        case 2:
            Angle_Approach(&D_004325A0.unk10, pitch, D_004325A0.unk40);
            Angle_Approach(&D_004325A0.unk14, yaw, D_004325A0.unk40);
            break;
        }
    }
    Vector_Add(D_004325A0.position, D_004325A0.position, D_004325A0.unk50);
    Vector_Add(D_004325A0.position, D_004325A0.position, D_004325A0.unk60);
}

/* Builds the camera matrices: the projection, world to view, and world to screen for the given mode. */
void func_001012D8(int projection, int mode) {
    f32 local[16];
    f32 other[16];
    f32 translation[4];
    f32 angle;

    switch (projection) {
    case 0:
        sceVu0ViewScreenMatrix(D_004322A0, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
        break;
    case 1:
        sceVu0ViewScreenMatrix(D_004322A0, 512.0f, 0.4f, 0.5714286f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f,
                               65535.0f);
        break;
    }
    translation[0] = -D_004325A0.position[0];
    translation[1] = -D_004325A0.position[1];
    translation[2] = -D_004325A0.position[2];
    translation[3] = 0.0f;
    sceVu0TransMatrix(D_00432320, D_00431AF0, translation);
    sceVu0RotMatrixY(D_00432320, D_00432320, -D_004325A0.unk14);
    sceVu0RotMatrixX(D_00432320, D_00432320, -D_004325A0.unk10);
    sceVu0RotMatrixY(D_00432360, D_00431AF0, Angle_Wrap(3.1415927f - D_004325A0.unk14));
    sceVu0RotMatrixX(D_00432360, D_00432360, Angle_Wrap(-D_004325A0.unk10));
    sceVu0RotMatrixZ(D_00432360, D_00432360, D_004325A0.unk18);
    sceVu0InversMatrix(D_00432360, D_00432360);
    switch (mode) {
    case 0:
        sceVu0MulMatrix(D_004323E0, D_004322A0, D_00432320);
        break;
    case 1:
        translation[1] = ((MapDrawStateMirror *)&map_draw_state)->mirror_height;
        translation[0] = translation[2] = translation[3] = 0.0f;
        sceVu0TransMatrix(local, D_00431AF0, translation);
        sceVu0CopyMatrix(other, D_00431AF0);
        other[5] = -1.0f;
        sceVu0MulMatrix(local, local, other);
        translation[1] = -((MapDrawStateMirror *)&map_draw_state)->mirror_height;
        sceVu0TransMatrix(other, D_00431AF0, translation);
        sceVu0MulMatrix(local, local, other);
        sceVu0MulMatrix(D_004323E0, D_004322A0, D_00432320);
        sceVu0MulMatrix(D_004323E0, D_004323E0, local);
        break;
    case 2:
        translation[1] = 220.0f;
        translation[0] = translation[2] = translation[3] = angle = 0.0f;
        sceVu0TransMatrix(local, D_00431AF0, translation);
        sceVu0RotMatrixY(local, local, angle);
        sceVu0RotMatrixX(local, local, angle);
        sceVu0MulMatrix(D_004323E0, D_004322A0, local);
        break;
    }
}

/* Runs the queued sound loads, one at a time. */
void func_00101618(void) {
    if (sound_load_queue.unk10 == NULL && sound_load_queue.unk14 != NULL) {
        sound_load_queue.unk10 = sound_load_queue.unk14;
        sound_load_queue.unk0C = 1;
        sound_load_queue.unk14 = NULL;
    }
    if (sound_load_queue.step == 3 && (sound_load_queue.unk0C != 0 || sound_load_queue.unk18 != 0)) {
        if (Sound_BgmLoadCanYield() != 0) {
            sound_load_queue.step = 0;
        }
    }
    if (sound_load_queue.step == 0) {
        if (sound_load_queue.unk0C != 0) {
            sound_load_queue.step = 1;
            Sound_StopAll(0);
        } else if (sound_load_queue.unk18 != 0) {
            sound_load_queue.step = 2;
        } else if (sound_load_queue.unk04 != 0) {
            sound_load_queue.step = 3;
        }
    }
    switch (sound_load_queue.step) {
    case 1:
        sound_load_queue.unk0C = Sound_LoadMapBankStep((int)sound_load_queue.unk10, sound_load_queue.unk0C);
        if (sound_load_queue.unk0C == 0) {
            sound_load_queue.unk10 = NULL;
            sound_load_queue.step = 0;
        }
        break;
    case 2:
        if (sound_load_queue.unk18 != 0) {
            sound_load_queue.unk18 = Sound_LoadEntityBankStep(sound_load_queue.unk1C, sound_load_queue.unk20, sound_load_queue.unk24 + 7);
            if (sound_load_queue.unk18 == 0) {
                sound_load_queue.step = 0;
            }
        }
        break;
    case 3:
        if (sound_load_queue.unk04 != 0) {
            sound_load_queue.unk04 = Sound_LoadBgm(sound_load_queue.unk00, sound_load_queue.unk04);
            if (sound_load_queue.unk04 == 0) {
                sound_load_queue.step = 0;
            }
        } else {
            sound_load_queue.step = 0;
        }
        break;
    }
}

/* Opens the menu or screen GameStatus asks for (bits 2 to 5), once the sound loads and the map load allow it,
   then puts back what the menu changed and fades the game in again. */
void func_001017A8(void) {
    PlayerState *player = &player_state;
    int menu_ran = FALSE;
    int faded_out = FALSE;
    u8 brightness = game_options[10];
    int load_held;
    PlayerItemDef *item_def;

    if (!game_status.bit2 && !game_status.bit3 && !game_status.bit4 && !game_status.save_requested && !game_status.bit8 &&
        !game_status.bit14) {
        return;
    }
    if (D_00446F78 == 0) {
        return;
    }
    if (game_status.unk30 & 1) {
        return;
    }
    if (player->unk1A8 != 0) {
        return;
    }
    if (sound_load_queue.unk0C != 0) {
        return;
    }
    if (sound_load_queue.unk18 != 0) {
        return;
    }
    if (sound_load_queue.unk04 != 0 && Sound_BgmLoadCanYield() == 0) {
        return;
    }
    load_held = FALSE;
    if (map_load_state.step >= 2 && map_load_state.step <= 4) {
        MapLoad_RequestHold();
        if (MapLoad_IsHeld() == 0) {
            return;
        }
        load_held = func_00102230();
        if (load_held) {
            func_00102228();
        }
    } else if (func_00102230() != 0) {
        return;
    }
    func_001B5778();
    if (game_status.bit14 && player_state.unk254 >= 2) {
        Player_LoadArmModels();
        game_status.bit14 = FALSE;
    }
    if (game_status.bit3) {
        if (!faded_out) { // match: always true here, and so in the three branches below; without the test 39 instructions differ
            faded_out = TRUE;
            func_00101CE0();
        }
        game_status.bit3 = FALSE;
        menu_ran = TRUE;
        Trade_Run(game_status.unk2C);
        func_001095E0();
        func_001095E0();
    } else if (game_status.bit4) {
        if (!faded_out) {
            faded_out = TRUE;
            func_00101CE0();
        }
        game_status.bit4 = FALSE;
        menu_ran = TRUE;
        Smith_Run();
        func_001095E0();
        func_001095E0();
    } else if (game_status.save_requested && game_status.unk00 != 0) {
        if (!faded_out) {
            faded_out = TRUE;
            func_00101CE0();
        }
        game_status.save_requested = FALSE;
        SaveScreen_Run();
        func_001095E0();
        func_001095E0();
    } else if (game_status.bit2 && game_status.unk00 != 0) {
        if (!faded_out) {
            faded_out = TRUE;
            func_00101CE0();
        }
        game_status.bit2 = FALSE;
        game_status.bit6 = FALSE;
        game_status.unk28 = 0;
        menu_ran = TRUE;
        Menu_Run(0);
        func_001095E0();
        func_001095E0();
    }
    if (menu_ran) {
        Player_LoadArmModels();
        if (game_status.unk28 != 0) {
            Player_CastMagic(game_status.unk28);
            game_status.unk28 = 0;
        }
        if (player_state.weapon != player_previous.weapon) {
            player_state.unkBC = 0;
        }
        if (player_state.equipped_magic != 0 && player_previous.equipped_magic != player_state.equipped_magic) {
            player_state.unk264 = player_state.equipped_magic;
            player_state.unkC0 = 0;
        } else if (player_state.shortcut_type == 2 &&
                   (player_previous.shortcut_type != player_state.shortcut_type || player_state.shortcut != player_previous.shortcut)) {
            player_state.unkC0 = 0;
            player_state.unk264 = player_state.shortcut;
        } else if (player_state.weapon != player_previous.weapon) {
            item_def = &weapon_defs[player_state.weapon];
            if (item_def->unk63 != 2) {
                if (item_def->unk42 != 0xFFFF) {
                    player_state.unkC0 = 0;
                    player_state.unk264 = item_def->unk42;
                } else if (item_def->unk52 != 0xFFFF && weapon_level_records[player_state.weapon].unk14 + 1 == 3) {
                    player_state.unkC0 = 0;
                    player_state.unk264 = item_def->unk52;
                }
            }
        }
        if (game_status.bit6 && item_common_defs[game_status.unk20].type == 6) {
            MapViewer_OpenForItem(game_status.unk20);
            game_status.unk20 = 0;
            game_status.bit6 = FALSE;
        }
    }
    if (game_status.bit8 && !game_status.bit9) {
        if (faded_out) {
            func_00101D48(brightness);
            faded_out = FALSE;
        } else {
            func_00101C88();
        }
        MapViewer_Run();
    }
    if (map_load_state.step != 0) {
        if (load_held) {
            func_00102218();
        }
        MapLoad_Release();
    }
    if (faded_out) {
        if (!game_status.bit9) {
            func_00101D48(brightness);
        } else {
            func_00101EF0();
        }
    }
}

void func_00101C88(void) {
    func_001095E0();
    func_001095E0();
    func_0010B000();
    func_001005E8(0);
    func_00109800();
    Display_SwapBuffers(0);
    func_001096E8();
    func_00109800();
    Display_SwapBuffers(0);
}

void func_00101CE0(void) {
    int saved_unk68;

    func_001095E0();
    func_001095E0();
    func_0010B000();
    saved_unk68 = map_draw_state.unk68;
    map_draw_state.unk68 = 0;
    func_001005E8(0);
    map_draw_state.unk68 = saved_unk68;
    func_00109800();
    Display_SwapBuffers(0);
    func_001096E8();
    func_00109800();
}

/* Draws the centred 0x280 x 0x1C0 logo sprite into both frame buffers, dimmed by the divisor. */
void func_00101D48(int divisor) {
    ScreenSprite sprite;
    int brightness;
    int pass;

    brightness = (game_options[10] << 7) / divisor;
    if (brightness > 0xFF) {
        brightness = 0xFF;
    }
    memset(&sprite, 0, sizeof(sprite));
    sprite.tex0 = 0x26802B240;
    sprite.tex1 = 0x60;
    sprite.prim = 0x156;
    sprite.rgba[2] = brightness;
    sprite.rgba[1] = brightness;
    sprite.rgba[0] = brightness;
    sprite.rgba[3] = 0x80;
    sprite.vertex[0].z = 0xFFFF;
    sprite.vertex[0].x = 0x6BF8;
    sprite.vertex[1].x = 0x93F8;
    sprite.vertex[1].z = 0xFFFF;
    sprite.vertex[1].tex_s = 0x2800;
    sprite.vertex[0].y = 0x7200;
    sprite.vertex[1].y = 0x8E00;
    sprite.vertex[1].tex_t = 0xE00;
    sprite.vertex[0].tex_s = 0;
    sprite.vertex[0].tex_t = 0;
    for (pass = 0; pass < 2; pass++) {
        func_001095E0();
        func_0010B000();
        func_001095D0();
        D_00446F78 = pass;
        D_00446F7C = pass;
        func_0010A008();
        func_00109A50();
        func_00109A80(0x47, 0x31001);
        func_00109A80(0x42, 0x44);
        func_00109A80(8, 5);
        func_00109AD0();
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(8, 0);
        func_00109AD0();
        func_001095D8();
        if (pass == 0) {
            D_00446F78 = 1;
            D_00446F7C = 1;
            func_001096E8();
            func_00109800();
            Display_SwapBuffers(0);
        }
    }
}

/* Runs two passes of the frame setup, syncing after the first. */
void func_00101EF0(void) {
    int pass;

    for (pass = 0; pass < 2; pass++) {
        func_001095E0();
        func_0010B000();
        func_001095D0();
        D_00446F78 = pass;
        D_00446F7C = pass;
        func_0010A008();
        func_0010A4E8(0, 0, 0, 0x80, 0x44);
        func_001095D8();
        if (pass == 0) {
            D_00446F78 = 1;
            D_00446F7C = 1;
            func_001096E8();
            func_00109800();
            Display_SwapBuffers(0);
        }
    }
}

/* Pause: when the pause button is pressed, holds the game until it is pressed again. */
void func_00101F90(void) {
    PlayTime play_time;

    if (pad_state[1] & D_00446946) {
        func_001D20E0();
        game_status.paused = TRUE;
        play_time = D_00446A28;
        Display_SwapBuffers(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_00111D58();
        func_001005E8(0);
        func_00109800();
        Display_SwapBuffers(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_00111D58();
        func_001005E8(0);
        func_00109800();
        Display_SwapBuffers(0);
        func_001B57D8();
        do {
            while (sceGsSyncV(0) != 1) {
            }
            func_001B5460();
        } while (!(pad_state[1] & D_00446946));
        func_001B5800();
        game_status.paused = FALSE;
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_00111D58();
        func_001005E8(0);
        func_00109800();
        func_001D2048(play_time);
    }
}

void func_001020F8(void) {
    if (game_status.bit2 || game_status.bit3 || game_status.bit4 || game_status.save_requested ||
        game_status.bit8 || game_status.bit14) {
        game_status.bit2 = 0;
        game_status.bit3 = 0;
        game_status.save_requested = 0;
        game_status.bit4 = 0;
        game_status.bit8 = 0;
        game_status.bit14 = 0;
        MapLoad_Release();
    }
}
