#include "kf_script.h"

/* The script interpreter's command tables (0x4331B0, 0x433440, 0x433650) and the blocks of game variables. */

/* Every command a table points to, and the symbols of the variable blocks. */
void ScriptCmd_ActivateMapObject();
void ScriptCmd_AddItem();
void ScriptCmd_AddPlayerHp();
void ScriptCmd_AddPlayerMp();
void ScriptCmd_AdvanceRecollectionPage();
void ScriptCmd_Atan2();
void ScriptCmd_BlendEntityMotion();
void ScriptCmd_BreakMapObject();
void ScriptCmd_ChangeMap();
void ScriptCmd_ClearEntityFlag18();
void ScriptCmd_CommitMapLoad();
void ScriptCmd_Cos();
void ScriptCmd_DeactivateMapObject();
void ScriptCmd_Distance();
void ScriptCmd_Distance2D();
void ScriptCmd_DropItemFromEntity();
void ScriptCmd_EmptyPlayerGauges();
void ScriptCmd_ExitMainLoop();
void ScriptCmd_FlashScreenRed();
void ScriptCmd_GetEntityCounter();
void ScriptCmd_GetEntityFlags();
void ScriptCmd_GetEntityPosition();
void ScriptCmd_GetEntityRotation();
void ScriptCmd_GetEntitySpawnPosition();
void ScriptCmd_GetEntitySpawnRotation();
void ScriptCmd_GetItemCount();
void ScriptCmd_GetItemPosition();
void ScriptCmd_GetItemRotation();
void ScriptCmd_GetMapObjectParams();
void ScriptCmd_GetMapObjectPosition();
void ScriptCmd_GetMapObjectRotation();
void ScriptCmd_GetPlayerHp();
void ScriptCmd_GetPlayerMp();
void ScriptCmd_GetPlayerStepCounter();
void ScriptCmd_GetPointFromPlayer();
void ScriptCmd_GetPropPosition();
void ScriptCmd_GetPropRotation();
void ScriptCmd_GetRecollectionPage();
void ScriptCmd_GivePlayerControl();
void ScriptCmd_HoldFrameOnStart();
void ScriptCmd_IsEntityFacing();
void ScriptCmd_IsMapObjectMotionDone();
void ScriptCmd_IsMapObjectOpen();
void ScriptCmd_IsScriptRunning();
void ScriptCmd_KillPlayer();
void ScriptCmd_LaunchShot();
void ScriptCmd_LoadMap();
void ScriptCmd_MarkMessageHeard();
void ScriptCmd_MoveCameraTo();
void ScriptCmd_MoveItemBy();
void ScriptCmd_MoveItemTo();
void ScriptCmd_MoveMapObjectBy();
void ScriptCmd_MoveMapObjectTo();
void ScriptCmd_MovePlayerTo();
void ScriptCmd_MovePropBy();
void ScriptCmd_MovePropTo();
void ScriptCmd_MoveRecollectionPage();
void ScriptCmd_Nop109();
void ScriptCmd_Nop15();
void ScriptCmd_Nop16();
void ScriptCmd_Nop17();
void ScriptCmd_Nop18();
void ScriptCmd_Nop200();
void ScriptCmd_Nop201();
void ScriptCmd_Nop202();
void ScriptCmd_Nop203();
void ScriptCmd_Nop204();
void ScriptCmd_Nop205();
void ScriptCmd_Nop206();
void ScriptCmd_Nop207();
void ScriptCmd_Nop208();
void ScriptCmd_Nop209();
void ScriptCmd_Nop210();
void ScriptCmd_Nop211();
void ScriptCmd_Nop212();
void ScriptCmd_Nop213();
void ScriptCmd_Nop214();
void ScriptCmd_Nop215();
void ScriptCmd_Nop216();
void ScriptCmd_Nop217();
void ScriptCmd_Nop3();
void ScriptCmd_Nop6();
void ScriptCmd_Nop7();
void ScriptCmd_Nop8();
void ScriptCmd_OpenMenu();
void ScriptCmd_OpenSaveScreen();
void ScriptCmd_OpenSmith();
void ScriptCmd_OpenTrade();
void ScriptCmd_PauseMapObjectAnim();
void ScriptCmd_PlaceMapItem();
void ScriptCmd_PlayEntityMotion();
void ScriptCmd_PlayMapObjectAnim();
void ScriptCmd_PlayMapObjectAnimRunning();
void ScriptCmd_PlayMovingSound();
void ScriptCmd_PlayMovingSoundWithMode();
void ScriptCmd_PlaySound();
void ScriptCmd_PlaySoundAt();
void ScriptCmd_PlaySoundAtPoint();
void ScriptCmd_PlayVoiceSound();
void ScriptCmd_PlayVoiceSoundAt();
void ScriptCmd_PlayVoiceSoundAtWithMode();
void ScriptCmd_Random();
void ScriptCmd_RandomFloat();
void ScriptCmd_ReloadArmModels();
void ScriptCmd_RequestSoundLoad();
void ScriptCmd_ResumeMapObjectAnim();
void ScriptCmd_RumblePad();
void ScriptCmd_RunScript();
void ScriptCmd_SetBgmVolume();
void ScriptCmd_SetCameraLookAt();
void ScriptCmd_SetCameraMode();
void ScriptCmd_SetCameraUnk48();
void ScriptCmd_SetEntitiesFlag5();
void ScriptCmd_SetEntityAnimSpeed();
void ScriptCmd_SetEntityFlags();
void ScriptCmd_SetEntityPosition();
void ScriptCmd_SetEntityRotation();
void ScriptCmd_SetHudShown();
void ScriptCmd_SetMapAmbientLight();
void ScriptCmd_SetMapFog();
void ScriptCmd_SetMapLight();
void ScriptCmd_SetMapObjectColor();
void ScriptCmd_SetMapObjectParams();
void ScriptCmd_SetMapObjectPartShown();
void ScriptCmd_SetMapObjectScripted();
void ScriptCmd_SetPlayerCanRun();
void ScriptCmd_SetPropColor();
void ScriptCmd_SetPropSolid();
void ScriptCmd_SetRecollectionPage();
void ScriptCmd_SetScreenColor();
void ScriptCmd_SetSeVolume();
void ScriptCmd_SetVolumes();
void ScriptCmd_ShakeCamera();
void ScriptCmd_ShowMessageBox();
void ScriptCmd_ShowNotice();
void ScriptCmd_ShowSplash();
void ScriptCmd_ShowSplashTimed();
void ScriptCmd_SignalMapObject();
void ScriptCmd_Sin();
void ScriptCmd_Stall();
void ScriptCmd_StartEffect();
void ScriptCmd_StartEffectAtPlayer();
void ScriptCmd_StartEntityMotion();
void ScriptCmd_StartItemPickup();
void ScriptCmd_StopAllScripts();
void ScriptCmd_StopSound();
void ScriptCmd_StoreUninitializedResult10();
void ScriptCmd_StoreUninitializedResult9();
void ScriptCmd_TakePlayerControl();
void ScriptCmd_TrySetEntityFlag18();
void ScriptCmd_TurnCameraTo();
void ScriptCmd_TurnEntityBack();
void ScriptCmd_TurnEntityBy();
void ScriptCmd_TurnEntityToPlayer();
void ScriptCmd_TurnEntityToward();
void ScriptCmd_TurnItemBy();
void ScriptCmd_TurnItemTo();
void ScriptCmd_TurnMapObjectBy();
void ScriptCmd_TurnMapObjectTo();
void ScriptCmd_TurnPlayerTo();
void ScriptCmd_TurnPropBy();
void ScriptCmd_TurnPropTo();
void ScriptCmd_Wait();
void ScriptCmd_WaitEntityMotion();
void ScriptCmd_WaitForever19();
void ScriptCmd_WaitForever20();
void ScriptCmd_WaitForever4();
void ScriptCmd_WaitMapObjectMotion();
void ScriptCmd_WalkEntityTo();
void ScriptCmd_WalkEntityToFlat();

extern u8 map_script_flags[];
extern u8 story_flags[];
extern u8 entity_group_state[];
extern u8 story_values[];
extern u8 weapon_level_records[];
extern u8 trade_stock_1[];
extern u8 trade_stock_2[];
extern u8 trade_stock_3[];
extern u8 trade_stock_4[];
extern u8 game_status[];
extern u8 pad_state[];
extern u8 player_state[];
extern u8 view_ray_status[];
extern u8 camera_state[];
extern u32 D_004335D0[0x10];
extern u32 D_00433610[0x10];
extern u16 forward_button;
extern u16 back_button;
extern u16 turn_right_button;
extern u16 turn_left_button;
extern u16 step_right_button;
extern u16 step_left_button;
extern u16 look_down_button;
extern u16 look_up_button;
extern u16 attack_button;
extern u16 magic_button;
extern u16 action_button;
extern u16 shortcut_button;
extern u8 item_counts[];
extern u8 spell_levels[];

ScriptCommandHandler script_general_command_handlers[0x64] = {
    ScriptCmd_Stall, ScriptCmd_Wait, ScriptCmd_RunScript, ScriptCmd_Nop3,
    ScriptCmd_WaitForever4, ScriptCmd_StopAllScripts, ScriptCmd_Nop6, ScriptCmd_Nop7,
    ScriptCmd_Nop8, ScriptCmd_StoreUninitializedResult9, ScriptCmd_StoreUninitializedResult10, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Nop15,
    ScriptCmd_Nop16, ScriptCmd_Nop17, ScriptCmd_Nop18, ScriptCmd_WaitForever19,
    ScriptCmd_WaitForever20, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_SetCameraUnk48, ScriptCmd_AdvanceRecollectionPage, ScriptCmd_GetRecollectionPage,
    ScriptCmd_SetRecollectionPage, ScriptCmd_ShowSplash, ScriptCmd_SetVolumes, ScriptCmd_SetSeVolume,
    ScriptCmd_SetBgmVolume, ScriptCmd_KillPlayer, ScriptCmd_MoveRecollectionPage, ScriptCmd_MarkMessageHeard,
    ScriptCmd_RequestSoundLoad, ScriptCmd_ShowSplashTimed, ScriptCmd_GetEntityCounter, ScriptCmd_EmptyPlayerGauges,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
    ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall, ScriptCmd_Stall,
}; /* 0x4331B0 commands 0 to 0x63 */

u8 *script_game_var_blocks[0x40] = {
    game_status, camera_state, player_state, view_ray_status,
    item_counts, trade_stock_1, trade_stock_2, trade_stock_3,
    trade_stock_4, spell_levels, map_script_flags, story_flags,
    entity_group_state, (u8 *)D_004335D0, (u8 *)D_00433610, story_values,
    weapon_level_records, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, 0x00000000, 0x00000000,
    NULL, 0x00000000, pad_state, (u8 *)&forward_button,
    (u8 *)&back_button, (u8 *)&turn_right_button, (u8 *)&turn_left_button, (u8 *)&step_right_button,
    (u8 *)&step_left_button, (u8 *)&look_down_button, (u8 *)&look_up_button, (u8 *)&attack_button,
    NULL, (u8 *)&magic_button, (u8 *)&action_button, (u8 *)&shortcut_button,
}; /* 0x433340 the blocks of game variables */

ScriptCommandHandler script_world_command_handlers[0x64] = {
    ScriptCmd_PlaceMapItem, ScriptCmd_StartItemPickup, ScriptCmd_MoveItemTo, ScriptCmd_TurnItemTo,
    ScriptCmd_DropItemFromEntity, ScriptCmd_MoveItemBy, ScriptCmd_TurnItemBy, ScriptCmd_GetItemPosition,
    ScriptCmd_GetItemRotation, ScriptCmd_Nop109, ScriptCmd_ActivateMapObject, ScriptCmd_DeactivateMapObject,
    ScriptCmd_MoveMapObjectTo, ScriptCmd_TurnMapObjectTo, ScriptCmd_BreakMapObject, ScriptCmd_IsMapObjectOpen,
    ScriptCmd_SetMapObjectScripted, ScriptCmd_PlayMapObjectAnimRunning, ScriptCmd_WaitMapObjectMotion, ScriptCmd_IsMapObjectMotionDone,
    ScriptCmd_MovePlayerTo, ScriptCmd_TurnPlayerTo, ScriptCmd_AddItem, ScriptCmd_GetItemCount,
    ScriptCmd_AddPlayerHp, ScriptCmd_AddPlayerMp, ScriptCmd_SetPlayerCanRun, ScriptCmd_GetPlayerStepCounter,
    ScriptCmd_GetPlayerHp, ScriptCmd_GetPlayerMp, ScriptCmd_StartEffect, ScriptCmd_FlashScreenRed,
    ScriptCmd_StartEffectAtPlayer, ScriptCmd_LaunchShot, ScriptCmd_Nop109, ScriptCmd_Nop109,
    ScriptCmd_Nop109, ScriptCmd_Nop109, ScriptCmd_Nop109, ScriptCmd_Nop109,
    ScriptCmd_SetCameraMode, ScriptCmd_SetCameraLookAt, ScriptCmd_MoveCameraTo, ScriptCmd_TurnCameraTo,
    ScriptCmd_ShakeCamera, ScriptCmd_Nop109, ScriptCmd_Nop109, ScriptCmd_SetPropColor,
    ScriptCmd_LoadMap, ScriptCmd_CommitMapLoad, ScriptCmd_SetPropSolid, ScriptCmd_MovePropTo,
    ScriptCmd_TurnPropTo, ScriptCmd_MovePropBy, ScriptCmd_TurnPropBy, ScriptCmd_GetPropPosition,
    ScriptCmd_GetPropRotation, ScriptCmd_SetMapFog, ScriptCmd_SetMapLight, ScriptCmd_SetMapAmbientLight,
    ScriptCmd_PlaySoundAt, ScriptCmd_PlaySoundAtPoint, ScriptCmd_PlayVoiceSoundAt, ScriptCmd_StopSound,
    ScriptCmd_PlayMovingSound, ScriptCmd_Nop109, ScriptCmd_PlaySound, ScriptCmd_PlayVoiceSound,
    ScriptCmd_PlayVoiceSoundAtWithMode, ScriptCmd_PlayMovingSoundWithMode, ScriptCmd_OpenMenu, ScriptCmd_OpenTrade,
    ScriptCmd_OpenSmith, ScriptCmd_OpenSaveScreen, ScriptCmd_ChangeMap, ScriptCmd_ExitMainLoop,
    ScriptCmd_HoldFrameOnStart, ScriptCmd_Nop109, ScriptCmd_Nop109, ScriptCmd_Nop109,
    ScriptCmd_SetMapObjectColor, ScriptCmd_SetMapObjectPartShown, ScriptCmd_SignalMapObject, ScriptCmd_GetMapObjectParams,
    ScriptCmd_SetMapObjectParams, ScriptCmd_MoveMapObjectBy, ScriptCmd_TurnMapObjectBy, ScriptCmd_GetMapObjectPosition,
    ScriptCmd_GetMapObjectRotation, ScriptCmd_PlayMapObjectAnim, ScriptCmd_ResumeMapObjectAnim, ScriptCmd_PauseMapObjectAnim,
    ScriptCmd_Nop109, ScriptCmd_Nop109, ScriptCmd_Nop109, ScriptCmd_ReloadArmModels,
    ScriptCmd_SetHudShown, ScriptCmd_SetScreenColor, ScriptCmd_RumblePad, ScriptCmd_IsScriptRunning,
}; /* 0x433440 commands 0x100 to 0x163 */

u32 D_004335D0[0x10]                         = { 0 }; /* 0x4335D0 zero fill */

u32 D_00433610[0x10]                         = { 0 }; /* 0x433610 zero fill */

ScriptCommandHandler script_entity_command_handlers[0x64] = {
    ScriptCmd_Nop200, ScriptCmd_Nop201, ScriptCmd_Nop202, ScriptCmd_Nop203,
    ScriptCmd_Nop204, ScriptCmd_Nop205, ScriptCmd_Nop206, ScriptCmd_Nop207,
    ScriptCmd_Nop208, ScriptCmd_Nop209, ScriptCmd_Nop210, ScriptCmd_Nop211,
    ScriptCmd_Nop212, ScriptCmd_Nop213, ScriptCmd_Nop214, ScriptCmd_Nop215,
    ScriptCmd_Nop216, ScriptCmd_Nop217, ScriptCmd_Sin, ScriptCmd_Cos,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_RandomFloat, ScriptCmd_GetPointFromPlayer,
    ScriptCmd_Atan2, ScriptCmd_Random, ScriptCmd_GivePlayerControl, ScriptCmd_TakePlayerControl,
    ScriptCmd_ShowMessageBox, ScriptCmd_ShowNotice, ScriptCmd_StartEntityMotion, ScriptCmd_PlayEntityMotion,
    ScriptCmd_WaitEntityMotion, ScriptCmd_BlendEntityMotion, ScriptCmd_TurnEntityToPlayer, ScriptCmd_WalkEntityTo,
    ScriptCmd_WalkEntityToFlat, ScriptCmd_TurnEntityBy, ScriptCmd_TurnEntityToward, ScriptCmd_TurnEntityBack,
    ScriptCmd_SetEntitiesFlag5, ScriptCmd_IsEntityFacing, ScriptCmd_SetEntityAnimSpeed, ScriptCmd_GetEntityPosition,
    ScriptCmd_GetEntityRotation, ScriptCmd_GetEntitySpawnPosition, ScriptCmd_GetEntitySpawnRotation, ScriptCmd_Distance2D,
    ScriptCmd_Distance, ScriptCmd_SetEntityFlags, ScriptCmd_GetEntityFlags, ScriptCmd_SetEntityPosition,
    ScriptCmd_SetEntityRotation, ScriptCmd_TrySetEntityFlag18, ScriptCmd_ClearEntityFlag18, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200,
    ScriptCmd_Nop200, ScriptCmd_Nop200, ScriptCmd_Nop200, 0x00000000,
}; /* 0x433650 commands 0x200 to 0x263 */
