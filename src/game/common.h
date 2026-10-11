/* Basic types shared by the game sources. */
#ifndef COMMON_H
#define COMMON_H

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long s64;               /* long is 64-bit on the EE */
typedef unsigned long u64;
typedef float f32;
typedef double f64;

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif
