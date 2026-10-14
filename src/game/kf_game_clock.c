/* The game clock (GameClock.ticks, a tick a second of game time): stepping it, and moving it on to a time of
   day. Nothing calls either function. */
#include "kf_game.h"

extern GameClock D_00431CE8;
extern ClockControl D_00432820;
extern u32 D_00446ED4;              /* the frame counter */
extern int D_00446EF0;

/* The game clock's tick count. */
static inline u32 Clock_GetTicks(void) {
    GameClock *clock = &D_00431CE8;

    return clock->ticks;
}

/* Advances the game clock by a tick unless D_00432820 stops it (with its flag 0x200, only on every fourth
   frame), and by 59 more while bit 0x400 of D_00446EF0 is set. */
static void GameClock_Tick(void) {
    GameClock *clock;

    if (D_00432820.unk213 == 0) {
        if (D_00432820.flags & 0x200) {
            if (D_00432820.unk214 == 0 && (D_00446ED4 & 3) == 0) {
                clock = &D_00431CE8;
                clock->ticks += 1;
            }
        } else if (D_00432820.unk214 == 0) {
            clock = &D_00431CE8;
            clock->ticks += 1;
        }
    }
    if (D_00446EF0 & 0x400) {
        clock = &D_00431CE8;
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
    clock = &D_00431CE8;
    clock->ticks += now;
}
