#include "kf_game.h"

int func_001D25C8(ClockTime *);

extern ClockTime D_00446A10;
extern ClockTime D_00446A18;

/* Restart the play clock from a saved play time: keep it as the base and read the wall clock. */
void func_001D2048(ClockTime play_time) {
    D_00446A18.sec = play_time.sec;
    D_00446A18.min = play_time.min;
    D_00446A18.hour = play_time.hour;
    D_00446A18.mday = play_time.mday;
    D_00446A18.mon = play_time.mon;
    D_00446A18.year = play_time.year;
    func_001D25C8(&D_00446A10);
}
