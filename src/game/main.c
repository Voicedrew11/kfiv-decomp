#include "core/game.h"
#include "ui/hud.h"
#include "effect/item_effect.h"
#include "player/player.h"
#include "ui/message.h"
#include "ui/title.h"
#include "ui/menu_screens.h"
#include "core/timer.h"

extern GameStatus game_status;
extern PlayerState player_state;
extern int save_loaded;          /* nonzero: a saved game was loaded */
extern int saved_map_tag;
extern u32 frame_counter;          /* the frame counter */

void System_Init(void);
void Display_Init(void);
int sceGsSyncV(int);
void Vram_Clear(int, int);
int SoundDriver_Init(void);
void SoundDriver_ReleaseAll(void);
void System_InitMemory(void);
void Input_Init(void);
void Input_SetDefaultControls(void);
void Game_SetDefaultOptions(void);
void Timer_CountOverflow(void);
int Timer_Setup(u32, int, int, void *);
void Timer_Restart(void);
void File_LoadArchiveToc(void);
void Map_LoadParamFiles(void);
void Game_ResetProgress(void);
void Input_StartupHook(void);
void Entity_StartupHook(void);
void Sound_Init(void);
s32 SaveCard_Init(void);
void Packet_Init(void);
void Wobble_ResetOscillators(void);
void Game_ProjectHazeLayers(void);
int Timer_GetTicks(void);
void srand(u32);
int TitleMenu_Run(void);
void Movie_PlayOpening(void);
void GameLoop_ResetSystems(int);
void GameLoop_Run(int);
void Movie_PlayEnding(void);

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
