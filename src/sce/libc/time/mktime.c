/*
 * KFIV: not in ICO.
 * sce/libc/time/mktime.c  (newlib libc/time/mktime.c, ~1999)
 *
 * div_t is declared here because the public headers of the newlib install
 * are not part of this tree.
 */
#include <time.h>

typedef struct {
    int quot;
    int rem;
} div_t;

extern div_t div(int numer, int denom);

#define _CONST const

#define SECSPERMIN 60
#define SECSPERHOUR 3600
#define SECSPERDAY 86400
#define EPOCH_YR 70

static _CONST int DAYS_IN_MONTH[12] =
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static _CONST int _DAYS_BEFORE_MONTH[12] =
{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define _DAYS_IN_MONTH(x) ((x == 1) ? days_in_feb : DAYS_IN_MONTH[x])

#define _ISLEAP(y) (((y) % 4) == 0 && (((y) % 100) != 0 || (((y)+1900) % 400) == 0))
#define _DAYS_IN_YEAR(year) (_ISLEAP(year) ? 366 : 365)

static void
validate_structure (struct tm *tim_p)
{
  div_t res;
  int days_in_feb = 28;

  /* calculate time & date to account for out of range values */
  if (tim_p->tm_sec < 0 || tim_p->tm_sec > 59)
    {
      res = div (tim_p->tm_sec, 60);
      tim_p->tm_min += res.quot;
      if ((tim_p->tm_sec = res.rem) < 0)
        {
          tim_p->tm_sec += 60;
          --tim_p->tm_min;
        }
    }

  if (tim_p->tm_min < 0 || tim_p->tm_min > 59)
    {
      res = div (tim_p->tm_min, 60);
      tim_p->tm_hour += res.quot;
      if ((tim_p->tm_min = res.rem) < 0)
        {
          tim_p->tm_min += 60;
          --tim_p->tm_hour;
        }
    }

  if (tim_p->tm_hour < 0 || tim_p->tm_hour > 23)
    {
      res = div (tim_p->tm_hour, 24);
      tim_p->tm_mday += res.quot;
      if ((tim_p->tm_hour = res.rem) < 0)
        {
          tim_p->tm_hour += 24;
          --tim_p->tm_mday;
        }
    }

  if (tim_p->tm_mon > 11)
    {
      res = div (tim_p->tm_mon, 12);
      tim_p->tm_year += res.quot;
      if ((tim_p->tm_mon = res.rem) < 0)
        {
          tim_p->tm_mon += 12;
          --tim_p->tm_year;
        }
    }

  if (_ISLEAP (tim_p->tm_year))
    days_in_feb = 29;

  /* Adjust mday, month and year */
  if (tim_p->tm_mday <= 0)
    {
      while (tim_p->tm_mday <= 0)
        {
          if (--tim_p->tm_mon == -1)
            {
              tim_p->tm_mon = 11;
              --tim_p->tm_year;
              days_in_feb = _ISLEAP (tim_p->tm_year) ? 29 : 28;
            }
          tim_p->tm_mday += _DAYS_IN_MONTH (tim_p->tm_mon);
        }
    }
  else
    {
      while (tim_p->tm_mday > _DAYS_IN_MONTH (tim_p->tm_mon))
        {
          tim_p->tm_mday -= _DAYS_IN_MONTH (tim_p->tm_mon);
          if (++tim_p->tm_mon == 12)
            {
              tim_p->tm_mon = 0;
              ++tim_p->tm_year;
              days_in_feb = _ISLEAP (tim_p->tm_year) ? 29 : 28;
            }
        }
    }
}

time_t
mktime (struct tm *tim_p)
{
  time_t tim = 0;
  long days = 0;
  int year;

  /* Make sure we have a valid time */
  validate_structure (tim_p);

  /* compute hours, minutes, seconds */
  tim += tim_p->tm_sec + (tim_p->tm_min * SECSPERMIN) +
    (tim_p->tm_hour * SECSPERHOUR);

  /* compute days in year */
  days += tim_p->tm_mday - 1;
  days += _DAYS_BEFORE_MONTH[tim_p->tm_mon];

  if (tim_p->tm_mon > 1 && _ISLEAP (tim_p->tm_year))
    days++;

  /* compute day of the year */
  tim_p->tm_yday = days;

  if (tim_p->tm_year > 10000 || tim_p->tm_year < -10000)
    return (time_t) -1;

  /* compute days in other years */
  if (tim_p->tm_year > EPOCH_YR)
    {
      for (year = EPOCH_YR; year < tim_p->tm_year; year++)
        days += _DAYS_IN_YEAR (year);
    }
  else if (tim_p->tm_year < EPOCH_YR)
    {
      for (year = EPOCH_YR - 1; year > tim_p->tm_year; year--)
        days -= _DAYS_IN_YEAR (year);
      days -= _DAYS_IN_YEAR (year);
    }

  /* compute day of the week, Thursday 1970-01-01 is day 4 */
  tim_p->tm_wday = (days + 4) % 7;
  if (tim_p->tm_wday < 0)
    tim_p->tm_wday += 7;

  /* compute total seconds */
  tim += days * SECSPERDAY;

  return tim;
}
