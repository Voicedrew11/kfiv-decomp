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
int PlayTime_ReadRealTimeClock(ClockTime *time);
int PlayTime_ReadClock(void);
int PlayTime_ComputeCurrent(void);
int PlayTime_ClockSecondsBetween(ClockTime start, ClockTime end);
int PlayTime_ClockMinutesBetween(ClockTime start, ClockTime end);
int PlayTime_ToSeconds(ClockTime play_time);
int PlayTime_ToMinutes(ClockTime play_time);

/* data still in asm */
extern ClockTime play_clock_start;
extern ClockTime play_time_base;
extern ClockTime wall_clock_time;

#endif
