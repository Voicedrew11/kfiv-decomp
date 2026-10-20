/* KFIV: not in ICO. The newlib <time.h> declarations this tree uses: struct tm and mktime. */
#ifndef SCE_LIBC_TIME_H
#define SCE_LIBC_TIME_H

typedef long time_t;

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

time_t mktime(struct tm *tim_p); /* definition in sce/ */

#endif /* SCE_LIBC_TIME_H */
