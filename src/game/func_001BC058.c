#include "kf_player.h"
#include "kf_screen.h"

extern PlayerState D_00413F80;
extern u8 weapon_defs[];

extern DisplayContext *D_00446978;

void func_00109098(DisplayContext *, u8, u16);
int Sound_StopVoice(int); // match: this file only matches with this declaration

/* Kick off a request once the flag at 0x261 is clear and the table entry at 0x63 is not 2. */
void func_001BC058(void) {
    DisplayContext *display;

    if (D_00413F80.unk261 == 0 && weapon_defs[D_00413F80.weapon * 0x90 + 0x63] != 2) {
        Sound_StopVoice(2);
        display = D_00446978;
        D_00413F80.unk261 = 1;
        func_00109098(display, 0, display->settings->unk82);
    }
}
