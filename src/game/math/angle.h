/* Stepping values and angles toward a target, angle wrapping, a triangle normal. */
#ifndef KF_ANGLE_H
#define KF_ANGLE_H

#include "common.h"

/* Cross product of (second - first) and (third - second), written to out: the normal of a triangle, not
   normalised */
void Vector_TriangleNormal(float *out, float *first, float *second, float *third);
/* Wraps an angle to -pi..pi */
float Angle_Wrap(float angle);
/* Moves *angle toward target by step the shorter way round; returns 1 once it has arrived */
int Angle_Approach(float *angle, float target, float step);
/* Eases an angle toward target over frames steps along the shortest way round; returns 1 once it has arrived */
int Angle_EaseToward(float target, float *angle, int frames);

/* math/sine_table.c */
extern f32 sine_table[];

/* data still in asm */
extern f32 negative_zero_angle_step_limit[];  /* -0.0f */

#endif
