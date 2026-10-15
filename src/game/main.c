#include "kf_game.h"
#include "kf_hud.h"
#include "kf_item_effect.h"
#include "kf_player.h"
#include "kf_message.h"
#include "kf_title.h"
#include "kf_menu_screens.h"
#include "kf_timer.h"

extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;
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
void func_001B5390(void);
void func_001B53E8(void);
void func_00102930(void);
void Timer_CountOverflow(void);
int Timer_Setup(u32, int, int, void *);
void Timer_Restart(void);
void func_00102178(void);
void func_00112648(void);
void func_001029A8(void);
void func_001B5808(void);
void func_00124EF8(void);
void Sound_Init(void);
s32 SaveCard_Init(void);
void func_00109410(void);
void func_00111D00(void);
void func_00106318(void);
int Timer_GetTicks(void);
void srand(u32);
int func_001F91F8(void);
void Movie_PlayOpening(void);
void func_001000C8(int);
void func_001001C0(int);
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
    ((void (*)(int))func_001B5390)(0); // match: retail passes a zero the function does not take
    func_001B53E8();
    func_00102930();
    sceGsSyncV(0);
    Timer_Setup(0, 0x1C1, 0xEA60, Timer_CountOverflow);
    Timer_Restart();
    func_00102178();
    func_00112648();
    func_001029A8();
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
    func_00106318();
    Menu_Run(4);
    sceGsSyncV(0);
    sceGsSyncV(0);
    Vram_Clear(0, 0x4000);
    srand(Timer_GetTicks());

    for (;;) {
        if (D_00446734 == 0) {
            sceGsSyncV(0);
            Vram_Clear(0, 0x4000);
            if (func_001F91F8() == 0) {
                Menu_Run(5);
                Movie_PlayOpening();
            }
            sceGsSyncV(0);
            Vram_Clear(0, 0x4000);
        }
        func_001000C8(0x263);
        func_001001C0(D_00446734 != 0 ? D_00446738 : 0x63);
        func_001CB9A0();
        Sound_Init();
        if (D_00413F80.unk1A8 != 0) {
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
        } else if (D_0038CC50.bits10 & 1) {
            Movie_PlayEnding();
        }
    }
}
