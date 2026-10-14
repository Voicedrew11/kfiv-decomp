/* EE timer set-up (kf_timer.c). */
#ifndef KF_TIMER_H
#define KF_TIMER_H

#include "common.h"

/* Programs EE timer `timer` with a mode and a compare value; timers 0 and 1 also get `handler` as their
   interrupt handler. Returns -1 for a timer number above 3. */
int Timer_Setup(u32 timer, int mode, int compare, void *handler);

#endif
