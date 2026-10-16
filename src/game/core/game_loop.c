/* The game session: resetting the systems, the main loop, drawing a frame, the camera, the sound-load
   queue, opening menus between frames with their fades, and pause. */
#include "item/gold.h"
#include "player/held_weapon.h"
#include "effect/item_effect.h"
#include "model/linked_point.h"
#include "player/player.h"
#include "effect/shot.h"
#include "ui/splash.h"
#include "effect/effect.h"
#include "core/game.h"
#include "entity/entity_world.h"
#include "ui/hud.h"
#include "map/map.h"
#include "map_object/map_object.h"
#include "player/player_update.h"
#include "sound/sound.h"
#include "ui/message.h"
#include "player/magic_ladder.h"
#include "ui/menu_screens.h"
#include "gfx/packet.h"

void Game_ResetProgress(void);
void Player_ResetForNewGame(void);
void Ini_Load(int);
void Game_NormalizeStartState(void);
void Player_RecomputeStats(void);
void Map_ResetGameState(void);
void Map_ClearSlots(void);
void MapLoad_Reset(void);
void MapViewer_Init(void);
void MapObject_ClearAllMaps(void);
void MapItem_Init(void);
void Attack_LoadMagicTable(void);
int Game_CheckSaveImage(void);
void Game_ReadSaveImage(void);
void Player_ResetGauges(void);
void Sound_LoadMgBank(int);
void Sound_ApplyStereoOption(void);
void Map_LoadAtOnce(int);
int PlayTime_Start(void);
void PlayTime_Resume(PlayTime);
void Game_StartFadeIn(int, int, int, int);
void Packet_SwapBuffers(void);
void Display_SwapBuffers(int);
int Timer_GetTicks(void);
void Timer_Restart(void);
void Packet_SendChain(void);
void DrawList_Clear(void);
void Wobble_StepOscillators(void);
void Input_Update(void);
int Script_UpdateAll(void);
void GameLoop_UpdateCameraShake(void);
void Interact_FindTarget(void);
void MapLoad_Update(void);
void func_00112BD0(void);
void Entity_UpdatePool(void);
void MapItem_UpdateFalling(void);
void Player_UseItem(void);
void GameLoop_RunSoundLoads(void);
void Sound_Update(void);
void Game_StepFade(void);
void GameLoop_UpdateCamera(void);
void GameLoop_SetCameraMatrices(int, int);
void GsDraw_QueueVu1Matrix(void *);
int GameLoop_DrawFrame(int);
void Packet_WaitForSend(void);
void MapItem_UpdatePickup(void);
void Map_RunFreeCamera(void);
void GameLoop_OpenRequestedMenu(void);
void MapChange_RunToEnd(void);
void GameLoop_UpdatePause(void);
void Input_StopRumble(void);
void Sound_SetBgmVolume(int);
void Sound_SetBankVolumes(int);
int sceGsSyncV(int);
int Sound_BgmLoadCanYield(void);
void File_CloseAll(void);
void Wobble_AdvanceSines(void);
void func_001095D0(void);
void GsDraw_ResetDrawTarget(void);
void ModelTexture_ResetSlots(void);
void Map_DrawRecords(int);
void MapItem_DrawAll(int);
void Map_DrawLateRecords(int);
void DrawList_Draw(u32, u32);
void GsDraw_EnableDepthTest(void);
void MapItem_DrawPicked(int);
void Map_CoverScreenWithMapTint(void);
void GsDraw_ClearDepth(void);
void Entity_DrawPoolModels(void);
void Map_SetOffscreenTarget(void);
void GsDraw_ClearFrame(void);
void GsDraw_QueueTexFlush(void);
void GsDraw_CopyFrame(int);
void Game_DrawWobblingScreen(int, int, int, int, float);
void Player_DrawScreenFade(void);
void Game_DrawHazeLayers(void);
void Game_DrawDepthTestSprite(void);
void Game_BlendScreenOffsetDown(void);
void Game_BlendScreenOffsetUpDown(void);
void Game_BlendPreviousFrame(int);
void Game_ShiftScreenByViewTurn(void);
void GsDraw_FillScreen(int, int, int, int, int);
void Game_TintScreen(int, int, int);
void Game_DrawFade(void);
void Game_ApplyBrightness(int);
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
int File_IsLocked(void);
void File_Unlock(void);
void GameLoop_DrawFrameWithoutHud(void);
void Trade_Run(s32);
void Smith_Run(void);
void SaveScreen_Run(void);
void MapViewer_OpenForItem(int);
void GameLoop_ShowGrabbedFrame(int);
void GameLoop_RedrawFrame(void);
void MapViewer_Run(void);
void File_Lock(void);
void MapLoad_Release(void);
void GameLoop_ClearBothBuffers(void);
void *memset(void *, int, u32);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_QueueTexturedSprite(void *);
void PlayTime_Update(void);
void Input_PauseRumble(void);
void Input_ResumeRumble(void);
extern PlayerState player_state;
extern int save_loaded;          /* nonzero: a saved game was loaded */
extern u8 game_options[0x10];
extern PlayTime save_play_time[];   /* the play time in the loaded save (save_image + 8) */
extern SoundLoadQueue sound_load_queue;
extern GameStatus game_status;
extern MapSlot map_slots[];
extern MapLoadState map_load_state;
extern u16 pad_state[];
extern f32 world_screen_matrix[16];
extern s64 game_dbuff_clear_color[];
extern s64 game_dbuff_context2_clear_color[];
extern u32 frame_counter;
extern u8 guardians_room_water_purified[];
extern MapDrawStateOverlays map_draw_state;
extern int message_dim_alpha;
extern int current_map_slot;
extern f32 scale_matrices[4][4];
extern f32 origin_point[4];
extern CameraState camera_state;
extern EventRunner view_ray_status;
extern f32 identity_matrix[16];      /* the unit matrix */
extern f32 view_screen_matrix[16];      /* view to screen */
extern f32 world_view_matrix[16];      /* world to view */
extern f32 inverse_camera_rotation[16];      /* the camera's rotation, inverted */
extern LevelRecord weapon_level_records[];
extern PlayerState player_previous;
extern ItemCommonDef item_common_defs[];
extern PlayerItemDef weapon_defs[];
extern int context1_buffer_half;
extern int context2_buffer_half;
extern u16 pause_button;
extern PlayTime current_play_time;

/* Resets every game system for a new session; a loaded save is then applied on top. */
void GameLoop_ResetSystems(int arg) {
    Game_ResetProgress();
    Player_ResetForNewGame();
    Ini_Load(arg);
    Game_NormalizeStartState();
    Player_RecomputeStats();
    Map_ResetGameState();
    Shot_ClearAll();
    LinkedPoint_Init();
    Map_ClearSlots();
    MapLoad_Reset();
    MapViewer_Init();
    HeldWeapon_Init();
    Entity_ResetAllLists();
    MapObject_ClearAllMaps();
    MapItem_Init();
    Effect_Init();
    Attack_LoadMagicTable();
    Gold_Init();
    Splash_Reset();
    ItemEffect_Reset();
    if (save_loaded != 0) {
        Game_CheckSaveImage();
        Game_ReadSaveImage();
    }
    Player_ResetGauges();
    Player_LoadArmModels();
    Sound_LoadMgBank(player_state.equipped_magic);
    Sound_ApplyStereoOption();
}

/* The game proper: places the player (new game) or resumes a loaded one, runs the main loop until
   GameStatus.bit9 is set, then waits for the sound loads to end. */
void GameLoop_Run(int arg) {
    MapSlotUnk08 *header;
    u8 saved;

    Map_LoadAtOnce(arg);
    if (save_loaded == 0) {
        PlayTime_Start();
        header = (MapSlotUnk08 *)map_slots[0].header;
        player_state.position.x = header->player_start_x;
        player_state.position.y = header->player_start_y;
        player_state.position.z = header->player_start_z;
        player_state.rotation.y = header->player_start_heading;
    } else {
        save_loaded = 0;
        PlayTime_Resume(save_play_time[0]);
        Game_StartFadeIn(0, 0, 0, 0x1E);
    }
    map_load_state.flags.raw |= 1;
    Packet_SwapBuffers();
    Packet_SwapBuffers();
    while (!game_status.bit9) {
        if (!(game_status.bit12 && game_status.bit13)) {
            Display_SwapBuffers(1);
        }
        frame_counter++;
        Timer_GetTicks();
        Timer_Restart();
        if (!(game_status.bit12 && game_status.bit13)) {
            Packet_SendChain();
            Timer_GetTicks();
            Packet_SwapBuffers();
            DrawList_Clear();
            Wobble_StepOscillators();
            Input_Update();
        }
        if (game_status.bit12 && !game_status.bit13) {
            if (pad_state[1] & 8) {
                pad_state[1] = 0;
                game_status.bit13 = TRUE;
                pad_state[0] = 0;
            }
        }
        if (!(game_status.update_hold_flags & 0x40) && player_state.death_timer == 0) {
            Script_UpdateAll();
        }
        Player_Update();
        GameLoop_UpdateCameraShake();
        Interact_FindTarget();
        MapLoad_Update();
        Player_UpdateArmModels();
        func_00112BD0();
        MapObject_UpdateAll();
        Entity_UpdatePool();
        Entity_UpdateAll();
        MapItem_UpdateFalling();
        Gold_Update();
        Player_UseItem();
        Player_UpdateWeaponAim();
        Effect_UpdateAll();
        LinkedPoint_UpdateFollows();
        Hud_Update();
        if (!(game_status.bit12 && game_status.bit13)) {
            GameLoop_RunSoundLoads();
            Sound_Update();
        }
        Game_StepFade();
        Splash_Update();
        ItemEffect_Update();
        GameLoop_UpdateCamera();
        GameLoop_SetCameraMatrices(0, 0);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        WeaponTrail_Update();
        if (!(game_status.bit12 && game_status.bit13)) {
            GameLoop_DrawFrame(1);
            Packet_WaitForSend();
        }
        MapItem_UpdatePickup();
        Map_RunFreeCamera();
        GameLoop_OpenRequestedMenu();
        MapChange_RunToEnd();
        GameLoop_UpdatePause();
    }
    game_dbuff_clear_color[0] = 0xC000L << 17;
    game_dbuff_clear_color[30] = 0xC000L << 17;
    game_dbuff_context2_clear_color[0] = 0xC000L << 17;
    game_dbuff_context2_clear_color[30] = 0xC000L << 17;
    Input_StopRumble();
    Message_CloseBox();
    Message_ClearNotices();
    saved = game_options[1];
    game_options[1] = 0;
    Sound_SetBgmVolume(0);
    Sound_SetBankVolumes(0);
    while (sound_load_queue.map_bank_load_pending != 0 ||
           ((sound_load_queue.bgm_load_pending != 0 || sound_load_queue.entity_bank_load_pending != 0) &&
            (sound_load_queue.entity_bank_load_pending != 0 || sound_load_queue.bgm_load_pending == 0 || Sound_BgmLoadCanYield() == 0))) {
        while (sceGsSyncV(0) != 1) {
        }
        Timer_Restart();
        GameLoop_RunSoundLoads();
        Sound_Update();
    }
    game_options[1] = saved;
    game_status.bit9 = FALSE;
    File_CloseAll();
    Packet_SwapBuffers();
    Packet_WaitForSend();
}

/* Draws one frame of the game: the world (once, or with a second view first for the modes 1 and 2), the screen
   tints, the overlays MapDrawState asks for, the fades and the pause shade. */
int GameLoop_DrawFrame(int arg) {
    int tinted = FALSE;
    f32 tint;
    int red;
    int green;
    int blue;

    Wobble_AdvanceSines();
    func_001095D0();
    GsDraw_ResetDrawTarget();
    switch (map_draw_state.mode) {
    case 0:
        GameLoop_SetCameraMatrices(0, 0);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        ModelTexture_ResetSlots();
        Map_DrawRecords(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        MapItem_DrawAll(0);
        ItemEffect_Draw();
        Gold_Draw(0);
        ModelTexture_ResetSlots();
        Map_DrawLateRecords(0);
        Effect_DrawAll(0);
        DrawList_Draw(0, 0);
        GameLoop_SetCameraMatrices(0, 2);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        DrawList_Draw(1, 0);
        DrawList_Draw(2, 0);
        GsDraw_EnableDepthTest();
        GameLoop_SetCameraMatrices(0, 0);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        MapItem_DrawPicked(0);
        break;
    case 1:
        GameLoop_SetCameraMatrices(0, 1);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        ModelTexture_ResetSlots();
        Map_DrawRecords(1);
        MapObject_DrawAll(1, 0, 0);
        Entity_DrawAll(1);
        MapItem_DrawAll(1);
        Gold_Draw(1);
        Map_DrawLateRecords(1);
        ModelTexture_ResetSlots();
        Effect_DrawAll(1);
        DrawList_Draw(0, 1);
        GsDraw_EnableDepthTest();
        GameLoop_SetCameraMatrices(0, 1);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        MapItem_DrawPicked(1);
        Map_CoverScreenWithMapTint();
        GsDraw_ClearDepth();
        GameLoop_SetCameraMatrices(0, 0);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        ModelTexture_ResetSlots();
        Map_DrawRecords(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        MapItem_DrawAll(0);
        ItemEffect_Draw();
        Gold_Draw(0);
        ModelTexture_ResetSlots();
        Map_DrawLateRecords(0);
        Effect_DrawAll(0);
        DrawList_Draw(0, 0);
        GameLoop_SetCameraMatrices(0, 2);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        DrawList_Draw(1, 0);
        DrawList_Draw(2, 0);
        GsDraw_EnableDepthTest();
        GameLoop_SetCameraMatrices(0, 0);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        MapItem_DrawPicked(0);
        break;
    case 2:
        Map_SetOffscreenTarget();
        GsDraw_ClearFrame();
        GameLoop_SetCameraMatrices(1, 1);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        ModelTexture_ResetSlots();
        Map_DrawRecords(2);
        MapObject_DrawAll(1, 0, 0);
        Entity_DrawAll(1);
        MapItem_DrawAll(1);
        ItemEffect_Draw();
        Gold_Draw(1);
        ModelTexture_ResetSlots();
        Map_DrawLateRecords(2);
        Effect_DrawAll(1);
        DrawList_Draw(0, 1);
        GsDraw_EnableDepthTest();
        GameLoop_SetCameraMatrices(1, 1);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        MapItem_DrawPicked(1);
        GsDraw_ResetDrawTarget();
        GsDraw_QueueTexFlush();
        GsDraw_ClearDepth();
        GameLoop_SetCameraMatrices(0, 0);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        ModelTexture_ResetSlots();
        Map_DrawRecords(0);
        MapObject_DrawAll(0, 0, 0);
        Entity_DrawPoolModels();
        Entity_DrawAll(0);
        MapItem_DrawAll(0);
        Gold_Draw(0);
        ModelTexture_ResetSlots();
        Map_DrawLateRecords(0);
        Effect_DrawAll(0);
        DrawList_Draw(0, 0);
        GameLoop_SetCameraMatrices(0, 2);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        DrawList_Draw(1, 0);
        DrawList_Draw(2, 0);
        GsDraw_EnableDepthTest();
        GameLoop_SetCameraMatrices(0, 0);
        GsDraw_QueueVu1Matrix(world_screen_matrix);
        MapItem_DrawPicked(0);
        break;
    }
    GameLoop_SetCameraMatrices(0, 0);
    GsDraw_QueueVu1Matrix(world_screen_matrix);
    if ((s32)(player_state.status_bits >> 39) & 1) {
        GsDraw_CopyFrame(0x2BC0);
        tinted = TRUE;
        Game_DrawWobblingScreen(0x80, 0x80, 0x80, 0x80, 1.0f);
    }
    if (player_state.mp_drain_tint != 0) {
        tint = (f32)player_state.mp_drain_tint * 0.125f;
        red = (int)(tint * -58.0f + 128.0f);
        green = (int)(tint * -78.0f + 128.0f);
        blue = (int)(tint * 12.0f + 128.0f);
        GsDraw_CopyFrame(0x2BC0);
        tinted = TRUE;
        Game_DrawWobblingScreen(red, green, blue, 0x80, tint);
    }
    Player_DrawScreenFade();
    if (map_draw_state.bit44) {
        Game_DrawHazeLayers();
    }
    if (map_draw_state.bit41) {
        Game_DrawDepthTestSprite();
    }
    if (map_draw_state.bit42) {
        Game_BlendScreenOffsetDown();
    }
    if (map_draw_state.bit43) {
        Game_BlendScreenOffsetUpDown();
    }
    if (map_draw_state.bit40) {
        Game_BlendPreviousFrame(map_draw_state.previous_frame_alpha);
    }
    if (player_state.aim_speed > 0.0f) {
        Game_BlendPreviousFrame(0x40);
    }
    if (arg != 0 && map_draw_state.bit45) {
        Game_ShiftScreenByViewTurn();
    }
    GsDraw_ClearDepth();
    if (tinted) {
        Effect_UploadTextures();
    }
    DrawList_Draw(3, 0);
    Player_DrawArm();
    Player_DrawScreenTints();
    if ((s32)(player_state.status_bits >> 39) & 1) {
        if (map_slots[current_map_slot].tag == 0x23 && guardians_room_water_purified[0] == 0) {
            GsDraw_FillScreen(0x40, 0x80, 0x40, 0x78, 0x44);
        } else {
            Game_TintScreen(0x5E, 0x87, 0x92);
        }
    }
    Hud_Draw();
    if (map_draw_state.screen_fill_alpha > 0) {
        // match: the last argument is passed as the 64-bit word
        ((void (*)(int, int, int, int, s64))GsDraw_FillScreen)(map_draw_state.screen_fill_red, map_draw_state.screen_fill_green, map_draw_state.screen_fill_blue,
                                                             map_draw_state.screen_fill_alpha, map_draw_state.screen_fill_blend);
    }
    Game_DrawFade();
    Splash_Draw();
    if (Message_IsBoxOpen() != 0) {
        if (message_dim_alpha < 0x40) {
            message_dim_alpha += 0x10;
            if (message_dim_alpha > 0x40) {
                message_dim_alpha = 0x40;
            }
        }
    } else if (message_dim_alpha > 0) {
        message_dim_alpha -= 0x10;
        if (message_dim_alpha < 0) {
            message_dim_alpha = 0;
        }
    }
    if (message_dim_alpha != 0) {
        GsDraw_FillScreen(0, 0, 0, message_dim_alpha, 0x44);
    }
    Message_Update();
    if (game_status.paused) {
        GsDraw_FillScreen(0, 0, 0, 0x40, 0x44);
        Hud_DrawPaused();
    }
    Game_ApplyBrightness(game_options[10]);
    func_001095D8();
}

/* Finds the map record the camera is in and works out this frame's two camera shake offsets. */
void GameLoop_UpdateCameraShake(void) {
    f32 matrix[4][4];
    MapRecordInfo *record;
    f32 angle;

    if (game_status.camera_mode != 0) {
        record = Map_FindRecordAtInSlot(&camera_state, 100.0f, current_map_slot);
        if (record != NULL) {
            player_state.map_index = (u32)((char *)record - map_slots[current_map_slot].records) >> 14; // match: the byte offset of 0x80-byte records shifted by 14; 7 would give the record number
            player_state.record_flag_block = record->flag_block_bit;
        }
    }
    if (camera_state.script_shake_frames != 0 && camera_state.script_shake_reach > 0.0f) {
        angle = (f32)(int)((f32)rand() * 360.0f / 2147483648.0f) * 0.017453292f;
        angle = Angle_Wrap(angle + camera_state.roll);
        camera_state.script_shake_offset[0] = camera_state.script_shake_reach;
        camera_state.script_shake_offset[1] = 0.0f;
        camera_state.script_shake_offset[2] = 0.0f;
        camera_state.script_shake_offset[3] = 1.0f;
        sceVu0CopyMatrix(matrix, scale_matrices);
        sceVu0RotMatrixZ(matrix[0], matrix[0], angle);
        sceVu0RotMatrixX(matrix[0], matrix[0], camera_state.pitch);
        sceVu0RotMatrixY(matrix[0], matrix[0], camera_state.yaw);
        sceVu0ApplyMatrix(camera_state.script_shake_offset, (int *)matrix, camera_state.script_shake_offset);
        if (camera_state.script_shake_frames > 0) {
            camera_state.script_shake_frames--;
        }
    } else {
        Vector_Copy(camera_state.script_shake_offset, origin_point);
    }
    if (camera_state.entity_shake_frames != 0 && camera_state.entity_shake_reach > 0.0f) {
        angle = (f32)(int)((f32)rand() * 360.0f / 2147483648.0f) * 0.017453292f;
        angle = Angle_Wrap(angle + camera_state.roll);
        camera_state.entity_shake_offset[0] = camera_state.entity_shake_reach;
        camera_state.entity_shake_offset[1] = 0.0f;
        camera_state.entity_shake_offset[2] = 0.0f;
        camera_state.entity_shake_offset[3] = 1.0f;
        sceVu0CopyMatrix(matrix, scale_matrices);
        sceVu0RotMatrixZ(matrix[0], matrix[0], angle);
        sceVu0RotMatrixX(matrix[0], matrix[0], camera_state.pitch);
        sceVu0RotMatrixY(matrix[0], matrix[0], camera_state.yaw);
        sceVu0ApplyMatrix(camera_state.entity_shake_offset, (int *)matrix, camera_state.entity_shake_offset);
        if (camera_state.entity_shake_frames > 0) {
            camera_state.entity_shake_frames--;
        }
    } else {
        Vector_Copy(camera_state.entity_shake_offset, origin_point);
    }
}

/* Moves and turns the camera for the current camera mode, then adds the two shake offsets. */
void GameLoop_UpdateCamera(void) {
    f32 unused_matrix[4][4];
    f32 direction[4];
    f32 target[4];
    f32 unused_vector[4];
    f32 yaw;
    f32 pitch;
    f32 length;

    switch (game_status.camera_mode) {
    case 0:
        Vector_Scale(target, view_ray_status.view_direction, view_ray_status.view_distance);
        Vector_Add(target, target, view_ray_status.view_origin);
        camera_state.position[0] = player_state.position.x;
        camera_state.position[2] = player_state.position.z;
        camera_state.position[1] =
            player_state.position.y - 220.0f + player_state.head_bob_offset + player_state.landing_dip_offset + camera_state.height_offset;
        Vector_Subtract(direction, target, camera_state.position);
        camera_state.pitch =
            atan2f(-direction[1], sqrtf(direction[0] * direction[0] + direction[2] * direction[2]));
        camera_state.yaw = atan2f(direction[0], direction[2]);
        camera_state.roll = 0.0f;
        break;
    case 1:
        if (game_status.camera_move_enabled != 0) {
            Vector_Subtract(direction, camera_state.move_to_point, camera_state.position);
            length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
            if (camera_state.move_speed < length) {
                Vector_Scale(direction, direction, camera_state.move_speed / length);
                Vector_Add(camera_state.position, camera_state.position, direction);
            } else {
                Vector_Copy(camera_state.position, camera_state.move_to_point);
            }
        }
        break;
    case 2:
        break;
    }
    if (game_status.camera_mode != 0 && (s8)game_status.camera_look_mode != 0) {
        Vector_Subtract(direction, camera_state.look_at_point, camera_state.position);
        yaw = atan2f(direction[0], direction[2]);
        length = sqrtf(direction[0] * direction[0] + direction[2] * direction[2]);
        pitch = atan2f(-direction[1], length);
        switch ((s8)game_status.camera_look_mode) {
        case 1:
            camera_state.pitch = pitch;
            camera_state.yaw = yaw;
            break;
        case 2:
            Angle_Approach(&camera_state.pitch, pitch, camera_state.turn_speed);
            Angle_Approach(&camera_state.yaw, yaw, camera_state.turn_speed);
            break;
        }
    }
    Vector_Add(camera_state.position, camera_state.position, camera_state.script_shake_offset);
    Vector_Add(camera_state.position, camera_state.position, camera_state.entity_shake_offset);
}

/* Builds the camera matrices: the projection, world to view, and world to screen for the given mode. */
void GameLoop_SetCameraMatrices(int projection, int mode) {
    f32 local[16];
    f32 other[16];
    f32 translation[4];
    f32 angle;

    switch (projection) {
    case 0:
        sceVu0ViewScreenMatrix(view_screen_matrix, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
        break;
    case 1:
        sceVu0ViewScreenMatrix(view_screen_matrix, 512.0f, 0.4f, 0.5714286f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f,
                               65535.0f);
        break;
    }
    translation[0] = -camera_state.position[0];
    translation[1] = -camera_state.position[1];
    translation[2] = -camera_state.position[2];
    translation[3] = 0.0f;
    sceVu0TransMatrix(world_view_matrix, identity_matrix, translation);
    sceVu0RotMatrixY(world_view_matrix, world_view_matrix, -camera_state.yaw);
    sceVu0RotMatrixX(world_view_matrix, world_view_matrix, -camera_state.pitch);
    sceVu0RotMatrixY(inverse_camera_rotation, identity_matrix, Angle_Wrap(3.1415927f - camera_state.yaw));
    sceVu0RotMatrixX(inverse_camera_rotation, inverse_camera_rotation, Angle_Wrap(-camera_state.pitch));
    sceVu0RotMatrixZ(inverse_camera_rotation, inverse_camera_rotation, camera_state.roll);
    sceVu0InversMatrix(inverse_camera_rotation, inverse_camera_rotation);
    switch (mode) {
    case 0:
        sceVu0MulMatrix(world_screen_matrix, view_screen_matrix, world_view_matrix);
        break;
    case 1:
        translation[1] = ((MapDrawStateMirror *)&map_draw_state)->mirror_height;
        translation[0] = translation[2] = translation[3] = 0.0f;
        sceVu0TransMatrix(local, identity_matrix, translation);
        sceVu0CopyMatrix(other, identity_matrix);
        other[5] = -1.0f;
        sceVu0MulMatrix(local, local, other);
        translation[1] = -((MapDrawStateMirror *)&map_draw_state)->mirror_height;
        sceVu0TransMatrix(other, identity_matrix, translation);
        sceVu0MulMatrix(local, local, other);
        sceVu0MulMatrix(world_screen_matrix, view_screen_matrix, world_view_matrix);
        sceVu0MulMatrix(world_screen_matrix, world_screen_matrix, local);
        break;
    case 2:
        translation[1] = 220.0f;
        translation[0] = translation[2] = translation[3] = angle = 0.0f;
        sceVu0TransMatrix(local, identity_matrix, translation);
        sceVu0RotMatrixY(local, local, angle);
        sceVu0RotMatrixX(local, local, angle);
        sceVu0MulMatrix(world_screen_matrix, view_screen_matrix, local);
        break;
    }
}

/* Runs the queued sound loads, one at a time. */
void GameLoop_RunSoundLoads(void) {
    if (sound_load_queue.pending_map_slot == NULL && sound_load_queue.next_map_slot != NULL) {
        sound_load_queue.pending_map_slot = sound_load_queue.next_map_slot;
        sound_load_queue.map_bank_load_pending = 1;
        sound_load_queue.next_map_slot = NULL;
    }
    if (sound_load_queue.step == 3 && (sound_load_queue.map_bank_load_pending != 0 || sound_load_queue.entity_bank_load_pending != 0)) {
        if (Sound_BgmLoadCanYield() != 0) {
            sound_load_queue.step = 0;
        }
    }
    if (sound_load_queue.step == 0) {
        if (sound_load_queue.map_bank_load_pending != 0) {
            sound_load_queue.step = 1;
            Sound_StopAll(0);
        } else if (sound_load_queue.entity_bank_load_pending != 0) {
            sound_load_queue.step = 2;
        } else if (sound_load_queue.bgm_load_pending != 0) {
            sound_load_queue.step = 3;
        }
    }
    switch (sound_load_queue.step) {
    case 1:
        sound_load_queue.map_bank_load_pending = Sound_LoadMapBankStep((int)sound_load_queue.pending_map_slot, sound_load_queue.map_bank_load_pending);
        if (sound_load_queue.map_bank_load_pending == 0) {
            sound_load_queue.pending_map_slot = NULL;
            sound_load_queue.step = 0;
        }
        break;
    case 2:
        if (sound_load_queue.entity_bank_load_pending != 0) {
            sound_load_queue.entity_bank_load_pending = Sound_LoadEntityBankStep(sound_load_queue.entity_bank_args, sound_load_queue.entity_slot_index, sound_load_queue.entity_slot_voice_out + 7);
            if (sound_load_queue.entity_bank_load_pending == 0) {
                sound_load_queue.step = 0;
            }
        }
        break;
    case 3:
        if (sound_load_queue.bgm_load_pending != 0) {
            sound_load_queue.bgm_load_pending = Sound_LoadBgm(sound_load_queue.bgm_id, sound_load_queue.bgm_load_pending);
            if (sound_load_queue.bgm_load_pending == 0) {
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
void GameLoop_OpenRequestedMenu(void) {
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
    if (context1_buffer_half == 0) {
        return;
    }
    if (game_status.update_hold_flags & 1) {
        return;
    }
    if (player->death_timer != 0) {
        return;
    }
    if (sound_load_queue.map_bank_load_pending != 0) {
        return;
    }
    if (sound_load_queue.entity_bank_load_pending != 0) {
        return;
    }
    if (sound_load_queue.bgm_load_pending != 0 && Sound_BgmLoadCanYield() == 0) {
        return;
    }
    load_held = FALSE;
    if (map_load_state.step >= 2 && map_load_state.step <= 4) {
        MapLoad_RequestHold();
        if (MapLoad_IsHeld() == 0) {
            return;
        }
        load_held = File_IsLocked();
        if (load_held) {
            File_Unlock();
        }
    } else if (File_IsLocked() != 0) {
        return;
    }
    Input_StopRumble();
    if (game_status.bit14 && player_state.frames_since_item_action >= 2) {
        Player_LoadArmModels();
        game_status.bit14 = FALSE;
    }
    if (game_status.bit3) {
        if (!faded_out) { // match: always true here, and so in the three branches below; without the test 39 instructions differ
            faded_out = TRUE;
            GameLoop_DrawFrameWithoutHud();
        }
        game_status.bit3 = FALSE;
        menu_ran = TRUE;
        Trade_Run(game_status.trade_screen_id);
        Packet_SwapBuffers();
        Packet_SwapBuffers();
    } else if (game_status.bit4) {
        if (!faded_out) {
            faded_out = TRUE;
            GameLoop_DrawFrameWithoutHud();
        }
        game_status.bit4 = FALSE;
        menu_ran = TRUE;
        Smith_Run();
        Packet_SwapBuffers();
        Packet_SwapBuffers();
    } else if (game_status.save_requested && game_status.player_control != 0) {
        if (!faded_out) {
            faded_out = TRUE;
            GameLoop_DrawFrameWithoutHud();
        }
        game_status.save_requested = FALSE;
        SaveScreen_Run();
        Packet_SwapBuffers();
        Packet_SwapBuffers();
    } else if (game_status.bit2 && game_status.player_control != 0) {
        if (!faded_out) {
            faded_out = TRUE;
            GameLoop_DrawFrameWithoutHud();
        }
        game_status.bit2 = FALSE;
        game_status.bit6 = FALSE;
        game_status.magic_to_cast = 0;
        menu_ran = TRUE;
        Menu_Run(0);
        Packet_SwapBuffers();
        Packet_SwapBuffers();
    }
    if (menu_ran) {
        Player_LoadArmModels();
        if (game_status.magic_to_cast != 0) {
            Player_CastMagic(game_status.magic_to_cast);
            game_status.magic_to_cast = 0;
        }
        if (player_state.weapon != player_previous.weapon) {
            player_state.power_gauge = 0;
        }
        if (player_state.equipped_magic != 0 && player_previous.equipped_magic != player_state.equipped_magic) {
            player_state.charging_magic = player_state.equipped_magic;
            player_state.magic_gauge = 0;
        } else if (player_state.shortcut_type == 2 &&
                   (player_previous.shortcut_type != player_state.shortcut_type || player_state.shortcut != player_previous.shortcut)) {
            player_state.magic_gauge = 0;
            player_state.charging_magic = player_state.shortcut;
        } else if (player_state.weapon != player_previous.weapon) {
            item_def = &weapon_defs[player_state.weapon];
            if (item_def->action_handler_index != 2) {
                if (item_def->first_magic_id != 0xFFFF) {
                    player_state.magic_gauge = 0;
                    player_state.charging_magic = item_def->first_magic_id;
                } else if (item_def->second_magic_id != 0xFFFF && weapon_level_records[player_state.weapon].unk14 + 1 == 3) {
                    player_state.magic_gauge = 0;
                    player_state.charging_magic = item_def->second_magic_id;
                }
            }
        }
        if (game_status.bit6 && item_common_defs[game_status.item_to_use].type == 6) {
            MapViewer_OpenForItem(game_status.item_to_use);
            game_status.item_to_use = 0;
            game_status.bit6 = FALSE;
        }
    }
    if (game_status.bit8 && !game_status.bit9) {
        if (faded_out) {
            GameLoop_ShowGrabbedFrame(brightness);
            faded_out = FALSE;
        } else {
            GameLoop_RedrawFrame();
        }
        MapViewer_Run();
    }
    if (map_load_state.step != 0) {
        if (load_held) {
            File_Lock();
        }
        MapLoad_Release();
    }
    if (faded_out) {
        if (!game_status.bit9) {
            GameLoop_ShowGrabbedFrame(brightness);
        } else {
            GameLoop_ClearBothBuffers();
        }
    }
}

/* Draws a game frame and shows it, then shows the next buffer too. */
void GameLoop_RedrawFrame(void) {
    Packet_SwapBuffers();
    Packet_SwapBuffers();
    DrawList_Clear();
    GameLoop_DrawFrame(0);
    Packet_WaitForSend();
    Display_SwapBuffers(0);
    Packet_SendChain();
    Packet_WaitForSend();
    Display_SwapBuffers(0);
}

/* Draws a game frame with the HUD off (MapDrawStateOverlays.hud_shown) and shows it: the picture a menu opens over. */
void GameLoop_DrawFrameWithoutHud(void) {
    int saved_unk68;

    Packet_SwapBuffers();
    Packet_SwapBuffers();
    DrawList_Clear();
    saved_unk68 = map_draw_state.hud_shown;
    map_draw_state.hud_shown = 0;
    GameLoop_DrawFrame(0);
    map_draw_state.hud_shown = saved_unk68;
    Packet_WaitForSend();
    Display_SwapBuffers(0);
    Packet_SendChain();
    Packet_WaitForSend();
}

/* Draws the grabbed frame (texture page 0x3240, the menus' backdrop) over the whole screen into both frame
   buffers, its colour game_options[10] * 128 / divisor. */
void GameLoop_ShowGrabbedFrame(int divisor) {
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
        Packet_SwapBuffers();
        DrawList_Clear();
        func_001095D0();
        context1_buffer_half = pass;
        context2_buffer_half = pass;
        GsDraw_ResetDrawTarget();
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(0x47, 0x31001);
        Packet_AddGsRegister(0x42, 0x44);
        Packet_AddGsRegister(8, 5);
        Packet_EndGsRegisters();
        GsDraw_QueueTexturedSprite(&sprite);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(8, 0);
        Packet_EndGsRegisters();
        func_001095D8();
        if (pass == 0) {
            context1_buffer_half = 1;
            context2_buffer_half = 1;
            Packet_SendChain();
            Packet_WaitForSend();
            Display_SwapBuffers(0);
        }
    }
}

/* Fills both frame buffers with black. */
void GameLoop_ClearBothBuffers(void) {
    int pass;

    for (pass = 0; pass < 2; pass++) {
        Packet_SwapBuffers();
        DrawList_Clear();
        func_001095D0();
        context1_buffer_half = pass;
        context2_buffer_half = pass;
        GsDraw_ResetDrawTarget();
        GsDraw_FillScreen(0, 0, 0, 0x80, 0x44);
        func_001095D8();
        if (pass == 0) {
            context1_buffer_half = 1;
            context2_buffer_half = 1;
            Packet_SendChain();
            Packet_WaitForSend();
            Display_SwapBuffers(0);
        }
    }
}

/* Pause: when the pause button is pressed, holds the game until it is pressed again. */
void GameLoop_UpdatePause(void) {
    PlayTime play_time;

    if (pad_state[1] & pause_button) {
        PlayTime_Update();
        game_status.paused = TRUE;
        play_time = current_play_time;
        Display_SwapBuffers(0);
        Packet_SendChain();
        Packet_SwapBuffers();
        DrawList_Clear();
        Wobble_StepOscillators();
        GameLoop_DrawFrame(0);
        Packet_WaitForSend();
        Display_SwapBuffers(0);
        Packet_SendChain();
        Packet_SwapBuffers();
        DrawList_Clear();
        Wobble_StepOscillators();
        GameLoop_DrawFrame(0);
        Packet_WaitForSend();
        Display_SwapBuffers(0);
        Input_PauseRumble();
        do {
            while (sceGsSyncV(0) != 1) {
            }
            Input_Update();
        } while (!(pad_state[1] & pause_button));
        Input_ResumeRumble();
        game_status.paused = FALSE;
        Packet_SendChain();
        Packet_SwapBuffers();
        DrawList_Clear();
        Wobble_StepOscillators();
        GameLoop_DrawFrame(0);
        Packet_WaitForSend();
        PlayTime_Resume(play_time);
    }
}

/* Drops every menu GameStatus asks for (bits 2 to 4, 8, 14 and the save request) and ends the map load hold. */
void GameLoop_CancelMenuRequests(void) {
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
