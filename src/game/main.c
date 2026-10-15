#include "kf_game.h"
#include "kf_hud.h"
#include "kf_item_effect.h"
#include "kf_player.h"
#include "kf_message.h"
#include "kf_title.h"
#include "kf_menu_screens.h"
#include "kf_timer.h"

extern GameStatus game_status;
extern PlayerState player_state;
extern int D_00446734;          /* nonzero: a saved game was loaded */
extern int D_00446738;
extern u32 D_00446ED4;          /* the frame counter */

void System_Init(void);
void Display_Init(void);
int sceGsSyncV(int);
void Vram_Clear(int, int);
int func_001CB778(void);
void func_001CB9A0(void);
void System_InitMemory(void);
void Input_Init(void);
void Input_SetDefaultControls(void);
void Game_SetDefaultOptions(void);
void Timer_CountOverflow(void);
int Timer_Setup(u32, int, int, void *);
void Timer_Restart(void);
void func_00102178(void);
void func_00112648(void);
void Game_ResetProgress(void);
void func_001B5808(void);
void func_00124EF8(void);
void Sound_Init(void);
s32 SaveCard_Init(void);
void func_00109410(void);
void func_00111D00(void);
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
    D_00446ED4 = 0;
    System_Init();
    Display_Init();
    sceGsSyncV(0);
    Vram_Clear(0, 0x4000);
    func_001CB778();
    func_001CB9A0();
    System_InitMemory();
    ((void (*)(int))Input_Init)(0); // match: retail passes a zero the function does not take
    Input_SetDefaultControls();
    Game_SetDefaultOptions();
    sceGsSyncV(0);
    Timer_Setup(0, 0x1C1, 0xEA60, Timer_CountOverflow);
    Timer_Restart();
    func_00102178();
    func_00112648();
    Game_ResetProgress();
    func_001B5808();
    func_00124EF8();
    ItemEffect_Load();
    Message_Init();
    Menu_Init();
    Sound_Init();
    SaveCard_Init();
    func_00109410();
    func_00111D00();
    Hud_Load();
    Game_ProjectHazeLayers();
    Menu_Run(4);
    sceGsSyncV(0);
    sceGsSyncV(0);
    Vram_Clear(0, 0x4000);
    srand(Timer_GetTicks());

    for (;;) {
        if (D_00446734 == 0) {
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
        GameLoop_Run(D_00446734 != 0 ? D_00446738 : 0x63);
        func_001CB9A0();
        Sound_Init();
        if (player_state.unk1A8 != 0) {
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
