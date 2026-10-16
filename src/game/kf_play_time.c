#include "kf_game.h"

#define BCD2DEC(x) (((x) & 0xF) + ((x) >> 4) * 10)

int PlayTime_ReadRealTimeClock(ClockTime *);
int PlayTime_ReadClock(void);
int PlayTime_ComputeCurrent(void);
int PlayTime_ClockSecondsBetween(ClockTime, ClockTime);
int PlayTime_ClockMinutesBetween(ClockTime, ClockTime);
int PlayTime_ToSeconds(ClockTime);
int PlayTime_ToMinutes(ClockTime);
int mktime(void *);
int PlayTime_ClockSecondsSinceStart(void);
int PlayTime_DayOfYear(ClockTime);
int sceCdReadClock(ClockTime *);

extern ClockTime play_clock_start;
extern ClockTime play_time_base;
extern ClockTime wall_clock_time;
extern ClockTime current_play_time;

/* Restart the play clock from a saved play time: keep it as the base and read the wall clock. */
void PlayTime_Resume(ClockTime play_time) {
    play_time_base.sec = play_time.sec;
    play_time_base.min = play_time.min;
    play_time_base.hour = play_time.hour;
    play_time_base.mday = play_time.mday;
    play_time_base.mon = play_time.mon;
    play_time_base.year = play_time.year;
    PlayTime_ReadRealTimeClock(&play_clock_start);
}

/* Start the play clock from zero. */
int PlayTime_Start(void) {
    play_time_base.sec = 0;
    play_time_base.min = 0;
    play_time_base.hour = 0;
    play_time_base.mday = 0;
    play_time_base.mon = 0;
    play_time_base.year = 0;
    PlayTime_ReadRealTimeClock(&play_clock_start);
    return 0;
}

/* Reads the wall clock and works out the play time now (current_play_time). */
void PlayTime_Update(void) {
    PlayTime_ReadClock();
    PlayTime_ComputeCurrent();
}

/* Seconds of wall-clock time between the clock start and the last clock read. */
int PlayTime_ClockSecondsSinceStart(void) {
    return PlayTime_ClockSecondsBetween(play_clock_start, wall_clock_time);
}

/* Minutes of wall-clock time between the clock start and the last clock read. */
int PlayTime_ClockMinutesSinceStart(void) {
    return PlayTime_ClockMinutesBetween(play_clock_start, wall_clock_time);
}

/* Seconds from the given play time to the current play time. */
int PlayTime_SecondsSince(ClockTime play_time) {
    return PlayTime_ToSeconds(current_play_time) - PlayTime_ToSeconds(play_time);
}

/* Minutes from the given play time to the current play time. */
int PlayTime_MinutesSince(ClockTime play_time) {
    return PlayTime_ToMinutes(current_play_time) - PlayTime_ToMinutes(play_time);
}

/* The day of the week of the last clock read, 0: Sunday. */
int PlayTime_GetWeekday(void) {
    CalendarTime calendar;

    calendar.tm_sec = wall_clock_time.sec;
    calendar.tm_min = wall_clock_time.min;
    calendar.tm_hour = wall_clock_time.hour;
    calendar.tm_mday = wall_clock_time.mday;
    calendar.tm_mon = wall_clock_time.mon - 1;
    calendar.tm_year = wall_clock_time.year + 0x64;
    mktime(&calendar);
    return calendar.tm_wday;
}

/* Seconds from one play time to another. */
int PlayTime_SecondsBetween(ClockTime start, ClockTime end) {
    int start_seconds = PlayTime_ToSeconds(start);

    return PlayTime_ToSeconds(end) - start_seconds;
}

/* Read the wall clock into wall_clock_time; gives sceCdReadClock's result. */
int PlayTime_ReadClock(void) {
    return PlayTime_ReadRealTimeClock(&wall_clock_time);
}

/* The play time now: the saved base plus the wall-clock seconds since the start, into current_play_time. */
int PlayTime_ComputeCurrent(void) {
    int elapsed;
    int seconds;
    int minutes;
    int hours;

    elapsed = PlayTime_ClockSecondsSinceStart();
    seconds = elapsed + PlayTime_ToSeconds(play_time_base);
    current_play_time.sec = seconds % 60;
    minutes = seconds / 60;
    current_play_time.min = minutes % 60;
    hours = minutes / 60;
    current_play_time.hour = hours % 99;
    current_play_time.mday = hours / 99;
    return 1;
}

/* A play time (seconds, minutes, hours, and hundreds of hours in mday) as seconds; the same code as
   PlayTime_ToSeconds. Nothing calls it. */
int PlayTime_AsSeconds(ClockTime play_time) {
    return play_time.sec + (play_time.min + (play_time.hour + play_time.mday * 99) * 60) * 60;
}

/* The day of the year of a clock time (mktime's tm_yday). */
int PlayTime_DayOfYear(ClockTime time) {
    int tm_fields[9];

    tm_fields[0] = time.sec;
    tm_fields[1] = time.min;
    tm_fields[2] = time.hour;
    tm_fields[3] = time.mday;
    tm_fields[4] = time.mon - 1;
    tm_fields[5] = time.year + 0x64;
    mktime(tm_fields);
    return tm_fields[7];
}

/* Seconds from clock start to clock end (same century). */
int PlayTime_ClockSecondsBetween(ClockTime start, ClockTime end) {
    int start_day;
    int end_day;
    char buf[0x30]; /* unused, but retail reserves the 0x30 bytes */

    start_day = PlayTime_DayOfYear(start);
    end_day = PlayTime_DayOfYear(end);
    if (start.year != end.year) {
        int year = start.year;

        if (year & 3) {
            end_day += (end.year - year) * 365;
        } else {
            end_day += (end.year - year) * 366;
        }
    }
    return (end.sec - start.sec)
        + ((end.min - start.min) + ((end.hour - start.hour) + (end_day - start_day) * 24) * 60) * 60;
}

/* Minutes from clock start to clock end (same century). */
int PlayTime_ClockMinutesBetween(ClockTime start, ClockTime end) {
    int start_day;
    int end_day;
    char buf[0x30]; /* unused, but retail reserves the 0x30 bytes */

    start_day = PlayTime_DayOfYear(start);
    end_day = PlayTime_DayOfYear(end);
    if (start.year != end.year) {
        int year = start.year;

        if (year & 3) {
            end_day += (end.year - year) * 365;
        } else {
            end_day += (end.year - year) * 366;
        }
    }
    return (end.min - start.min) + ((end.hour - start.hour) + (end_day - start_day) * 24) * 60;
}

/* A play time (seconds, minutes, hours, and hundreds of hours in mday) as seconds. */
int PlayTime_ToSeconds(ClockTime play_time) {
    return play_time.sec + (play_time.min + (play_time.hour + play_time.mday * 99) * 60) * 60;
}

/* A play time as minutes. */
int PlayTime_ToMinutes(ClockTime play_time) {
    return play_time.min + (play_time.hour + play_time.mday * 99) * 60;
}

/* Read the console's real-time clock into `time`, its BCD fields made binary; gives sceCdReadClock's result. */
int PlayTime_ReadRealTimeClock(ClockTime *time) {
    int result;

    result = sceCdReadClock(time);
    time->sec = BCD2DEC(time->sec);
    time->min = BCD2DEC(time->min);
    time->hour = BCD2DEC(time->hour);
    time->mday = BCD2DEC(time->mday);
    time->mon = BCD2DEC(time->mon);
    time->year = BCD2DEC(time->year);
    return result;
}
