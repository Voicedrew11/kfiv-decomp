#include "kf_game.h"

extern GameClock D_00431CE8;

/* The game clock's tick count (a tick is one second of game time). */
static inline u32 KF_Clock_GetTicks(void) {
    GameClock *clock = &D_00431CE8;

    return clock->ticks;
}

/* Advances the game clock to the next time it shows the given hour, minute and second. */
void func_0020CD78(int hour, int minute, int second) {
    u32 ticks = KF_Clock_GetTicks();
    GameClock *clock;
    int target;
    int now;

    now = ticks % 60 + ticks / 60 % 60 * 60 + ((ticks / 3600 + 12) % 24 + 12) * 3600;
    target = minute * 60 + second + (hour + 12) * 3600;
    if (now < target) {
        now = target - now;
    } else {
        now = 86400 - (now - target);
    }
    clock = &D_00431CE8;
    clock->ticks += now;
}
