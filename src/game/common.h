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
typedef u32 Uint128 __attribute__((mode(TI))); /* a 128-bit integer, moved with one lq/sq */

#ifndef NULL
#define NULL ((void *)0)
#endif

#define TRUE 1
#define FALSE 0

#define ABS(x) ((x) >= 0 ? (x) : -(x))

#define PI      3.1415927f
#define TWO_PI  6.2831855f
#define HALF_PI 1.5707964f
/* Degrees to radians; folds to the retail constant for whole-degree angles */
#define DEG_TO_RAD(deg) ((deg) * (PI / 180.0f))
#define FLT_MAX 3.4028235e38f
/* 1 / 2^31: scales rand() (0..0x7FFFFFFF) to 0..1 */
#define RAND_SCALE (1.0f / 2147483648.0f)

#endif
