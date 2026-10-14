#include "kf_game.h"
#include "kf_hud.h"
#include "kf_item_effect.h"
#include "kf_player.h"
#include "kf_message.h"
#include "kf_title.h"
#include "kf_menu_screens.h"

extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;
extern int D_00446734;          /* nonzero: a saved game was loaded */
extern int D_00446738;
extern u32 D_00446ED4;          /* the frame counter */

void func_0020AF38(void);
void func_0020AFA8(void);
int sceGsSyncV(int);
void func_0019BDA0(int, int);
int func_001CB778(void);
void func_001CB9A0(void);
void func_0020B6A0(void);
void func_001B5390(void);
void func_001B53E8(void);
void func_00102930(void);
void func_0020D050(void);
int func_00216D50(u32, int, int, void *);
void func_0020D078(void);
void func_00102178(void);
void func_00112648(void);
void func_001029A8(void);
void func_001B5808(void);
void func_00124EF8(void);
void func_0019C300(void);
s32 SaveCard_Init(void);
void func_00109410(void);
void func_00111D00(void);
void func_00106318(void);
int func_0020D088(void);
void srand(u32);
int func_001F91F8(void);
void func_0020AD78(void);
void func_001000C8(int);
void func_001001C0(int);
void func_0020AE68(void);

/* Sets everything up, then loops for ever: the intro, the title screen, one game. */
int main(void) {
    D_00446ED4 = 0;
    func_0020AF38();
    func_0020AFA8();
    sceGsSyncV(0);
    func_0019BDA0(0, 0x4000);
    func_001CB778();
    func_001CB9A0();
    func_0020B6A0();
    ((void (*)(int))func_001B5390)(0); // match: retail passes a zero the function does not take
    func_001B53E8();
    func_00102930();
    sceGsSyncV(0);
    func_00216D50(0, 0x1C1, 0xEA60, func_0020D050);
    func_0020D078();
    func_00102178();
    func_00112648();
    func_001029A8();
    func_001B5808();
    func_00124EF8();
    ItemEffect_Load();
    Message_Init();
    Menu_Init();
    func_0019C300();
    SaveCard_Init();
    func_00109410();
    func_00111D00();
    Hud_Load();
    func_00106318();
    Menu_Run(4);
    sceGsSyncV(0);
    sceGsSyncV(0);
    func_0019BDA0(0, 0x4000);
    srand(func_0020D088());

    for (;;) {
        if (D_00446734 == 0) {
            sceGsSyncV(0);
            func_0019BDA0(0, 0x4000);
            if (func_001F91F8() == 0) {
                Menu_Run(5);
                func_0020AD78();
            }
            sceGsSyncV(0);
            func_0019BDA0(0, 0x4000);
        }
        func_001000C8(0x263);
        func_001001C0(D_00446734 != 0 ? D_00446738 : 0x63);
        func_001CB9A0();
        func_0019C300();
        if (D_00413F80.unk1A8 != 0) {
            sceGsSyncV(0);
            func_0019BDA0(0, 0x4000);
            do {
                // match: retail passes a zero the function does not take
                if (((int (*)(int))Title_RunGameOver)(0) != 1) {
                    break;
                }
            } while (Menu_Run(1) == 0);
            sceGsSyncV(0);
            func_0019BDA0(0, 0x4000);
        } else if (D_0038CC50.bits10 & 1) {
            func_0020AE68();
        }
    }
}
