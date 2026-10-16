/* The game's timer: EE timer 0 counting up to 60000 and an overflow count (timer_overflow_count). */
#include "core/timer.h"
#include <eeregs.h>

extern int timer_overflow_count;    /* timer 0 overflows since the last restart */

/* Timer 0 overflow handler: counts the overflow and acknowledges it. */
void Timer_CountOverflow(void) {
    timer_overflow_count++;
    *T0_MODE |= 0x400;
}

/* Restarts the timer 0 clock. */
void Timer_Restart(void) {
    timer_overflow_count = 0;
    *T0_COUNT = 0;
}

/* Timer 0 ticks since the last restart (the counter overflows every 60000). */
int Timer_GetTicks(void) {
    return timer_overflow_count * 60000 + *T0_COUNT;
}

/* Empty. */
static void func_0020D0A8(void) {
}
