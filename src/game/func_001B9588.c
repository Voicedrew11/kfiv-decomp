#include "kf_player.h"

extern PlayerState D_00413F80;

/* Counts six countdown timers down to zero, clearing each matching flag on the way. */
void func_001B9588(void) {
    PlayerState *player = &D_00413F80;

    if (player->flag_timers[0] != 0) {
        player->timed_flags[0] = 0;
        player->flag_timers[0]--;
    }
    if (player->flag_timers[1] != 0) {
        player->timed_flags[1] = 0;
        player->flag_timers[1]--;
    }
    if (player->flag_timers[2] != 0) {
        player->timed_flags[2] = 0;
        player->flag_timers[2]--;
    }
    if (player->flag_timers[3] != 0) {
        player->timed_flags[3] = 0;
        player->flag_timers[3]--;
    }
    if (player->flag_timers[4] != 0) {
        player->timed_flags[4] = 0;
        player->flag_timers[4]--;
    }
    if (player->flag_timers[5] != 0) {
        player->timed_flags[5] = 0;
        player->flag_timers[5]--;
    }
}
