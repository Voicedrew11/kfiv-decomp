/* The game's timer, EE timer 0 (kf_timer.c). */
#ifndef KF_TIMER_H
#define KF_TIMER_H

#include "common.h"

/* Timer 0 overflow handler: counts the overflow and acknowledges it. */
void Timer_CountOverflow(void);

/* Restarts the count. */
void Timer_Restart(void);

/* Timer 0 ticks since the last restart. */
int Timer_GetTicks(void);

#endif
