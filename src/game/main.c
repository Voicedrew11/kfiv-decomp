#include <libgraph.h>
#include "core/game.h"
#include "ui/hud.h"
#include "effect/item_effect.h"
#include "player/player.h"
#include "ui/message.h"
#include "ui/title.h"
#include "ui/menu_screens.h"
#include "core/timer.h"
#include "core/display.h"
#include "core/file.h"
#include "core/system.h"
#include "debug/screen_dump.h"
#include "enemy/enemy.h"
#include "entity/entity_world.h"
#include "gfx/packet.h"
#include "gfx/screen.h"
#include "input/input.h"
#include "map/map.h"
#include "save/save_card.h"
#include "sound/sound.h"
#include "sound/sound_game.h"
#include <stdlib.h>

/* Sets everything up, then loops for ever: the intro, the title screen, one game. */
int main(void) {
    frame_counter = 0;
    System_Init();
    Display_Init();
    sceGsSyncV(0);
    Vram_Clear(0, 0x4000);
    SoundDriver_Init();
    SoundDriver_ReleaseAll();
    System_InitMemory();
    ((void (*)(int))Input_Init)(0); // match: retail passes a zero the function does not take
    Input_SetDefaultControls();
    Game_SetDefaultOptions();
    sceGsSyncV(0);
    Timer_Setup(0, 0x1C1, 0xEA60, Timer_CountOverflow);
    Timer_Restart();
    File_LoadArchiveToc();
    Map_LoadParamFiles();
    Game_ResetProgress();
    Input_StartupHook();
    Entity_StartupHook();
    ItemEffect_Load();
    Message_Init();
    Menu_Init();
    Sound_Init();
    SaveCard_Init();
    Packet_Init();
    Wobble_ResetOscillators();
    Hud_Load();
    Game_ProjectHazeLayers();
    Menu_Run(4);
    sceGsSyncV(0);
    sceGsSyncV(0);
    Vram_Clear(0, 0x4000);
    srand(Timer_GetTicks());

    for (;;) {
        if (save_loaded == 0) {
            sceGsSyncV(0);
            Vram_Clear(0, 0x4000);
            if (TitleMenu_Run() == 0) {
                Menu_Run(5);
                Movie_PlayOpening();
            }
            sceGsSyncV(0);
            Vram_Clear(0, 0x4000);
        }
        GameLoop_ResetSystems(0x263);
        GameLoop_Run(save_loaded != 0 ? saved_map_tag : 0x63);
        SoundDriver_ReleaseAll();
        Sound_Init();
        if (player_state.death_timer != 0) {
            sceGsSyncV(0);
            Vram_Clear(0, 0x4000);
            do {
                // match: retail passes a zero the function does not take
                if (((int (*)(int))Title_RunGameOver)(0) != 1) {
                    break;
                }
            } while (Menu_Run(1) == 0);
            sceGsSyncV(0);
            Vram_Clear(0, 0x4000);
        } else if (game_status.bits10 & 1) {
            Movie_PlayEnding();
        }
    }
}
