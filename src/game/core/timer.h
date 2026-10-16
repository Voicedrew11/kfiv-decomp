/* The EE timers: the set-up of a timer (core/timer_setup.c) and the game's timer, EE timer 0 (core/timer.c). */
#ifndef KF_TIMER_H
#define KF_TIMER_H

#include "common.h"

/* Programs EE timer `timer` with a mode and a compare value; timers 0 and 1 also get `handler` as their
   interrupt handler. Returns -1 for a timer number above 3. */
int Timer_Setup(u32 timer, int mode, int compare, void *handler);

/* Timer 0 overflow handler: counts the overflow and acknowledges it. */
void Timer_CountOverflow(void);

/* Restarts the count. */
void Timer_Restart(void);

/* Timer 0 ticks since the last restart. */
int Timer_GetTicks(void);

#endif
