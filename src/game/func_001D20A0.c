#include "kf_game.h"

int func_001D25C8(ClockTime *);

extern ClockTime D_00446A10;
extern ClockTime D_00446A18;

/* Start the play clock from zero. */
int func_001D20A0(void) {
    D_00446A18.sec = 0;
    D_00446A18.min = 0;
    D_00446A18.hour = 0;
    D_00446A18.mday = 0;
    D_00446A18.mon = 0;
    D_00446A18.year = 0;
    func_001D25C8(&D_00446A10);
    return 0;
}
