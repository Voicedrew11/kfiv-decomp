#include "kf_player.h"
#include "kf_screen.h"

extern PlayerState D_00413F80;
extern u8 D_0041B9B8[];

extern DisplayContext *D_00446978;

void func_00109098(DisplayContext *, u8, u16);
int func_0019CDA0(int);

/* Kick off a request once the flag at 0x261 is clear and the table entry at 0x63 is not 2. */
void func_001BC058(void) {
    DisplayContext *display;

    if (D_00413F80.unk261 == 0 && D_0041B9B8[D_00413F80.unk3C * 0x90 + 0x63] != 2) {
        func_0019CDA0(2);
        display = D_00446978;
        D_00413F80.unk261 = 1;
        func_00109098(display, 0, display->settings->unk82);
    }
}
