/* The game clock (GameClock.ticks, a tick a second of game time): stepping it, and moving it on to a time of
   day. Nothing calls either function. */
#include "kf_game.h"

extern ClockControl D_00432820;
extern u32 D_00446ED4;              /* the frame counter */
extern int D_00446EF0;

GameClock game_clock = { 0 };       /* 0x431CE8 the game clock */
u32 D_00431CF8[0x13E] = { 0 };      /* 0x431CF8 zero fill up to the map slot memory */

/* The game clock's tick count. */
static inline u32 Clock_GetTicks(void) {
    GameClock *clock = &game_clock;

    return clock->ticks;
}

/* Advances the game clock by a tick unless D_00432820 stops it (with its flag 0x200, only on every fourth
   frame), and by 59 more while bit 0x400 of D_00446EF0 is set. */
static void GameClock_Tick(void) {
    GameClock *clock;

    if (D_00432820.clock_stopped == 0) {
        if (D_00432820.flags & 0x200) {
            if (D_00432820.tick_held == 0 && (D_00446ED4 & 3) == 0) {
                clock = &game_clock;
                clock->ticks += 1;
            }
        } else if (D_00432820.tick_held == 0) {
            clock = &game_clock;
            clock->ticks += 1;
        }
    }
    if (D_00446EF0 & 0x400) {
        clock = &game_clock;
        clock->ticks += 0x3B;
    }
}

/* Advances the game clock to the next time it shows the given hour, minute and second. */
static void GameClock_AdvanceTo(int hour, int minute, int second) {
    u32 ticks = Clock_GetTicks();
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
    clock = &game_clock;
    clock->ticks += now;
}
