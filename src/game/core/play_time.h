/* Play time and the real-time clock (core/play_time.c). */
#ifndef KF_PLAY_TIME_H
#define KF_PLAY_TIME_H

#include "common.h"
#include "core/game.h"

/* core/play_time.c */
void PlayTime_Resume(ClockTime play_time);
int PlayTime_Start(void);
void PlayTime_Update(void);
int PlayTime_SecondsBetween(ClockTime start, ClockTime end);

/* data still in asm */
extern ClockTime play_clock_start;
extern ClockTime play_time_base;
extern ClockTime wall_clock_time;

#endif
