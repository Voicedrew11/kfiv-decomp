/* Small sums the entity code shares: clamps, distances, angles wrapped to -pi..pi and eased towards a goal. */
#ifndef KF_ENTITY_MATH_H
#define KF_ENTITY_MATH_H

#include "common.h"
#include "kf_model.h"

/* `value + amount`, kept within `low`..`high`. */
s32 EntityMath_AddClamped(s32 value, s32 amount, s32 low, s32 high);

/* `value - amount`, kept within `low`..`high`. */
s32 EntityMath_SubtractClamped(s32 value, s32 amount, s32 low, s32 high);

/* dst = a * (1 - t) + b * t across the xyz lanes, on the VU0. */
void EntityMath_LerpVector(void *dst, void *src_a, void *src_b, float ratio);

/* The distance between two points. */
f32 EntityMath_Distance(f32 *point_a, f32 *point_b);

/* The distance between two points, height left out. */
float EntityMath_DistanceXZ(float *point_a, float *point_b);

/* An angle wrapped into -pi..pi. */
float EntityMath_WrapAngle(float radians);

/* `current` one step towards `target`. */
f32 EntityMath_Approach(f32 current, f32 target, f32 step);

/* Moves *current one step towards `target`; returns how far it was. */
float EntityMath_StepToward(float *current, float target, float step);

/* Moves *value 1/`frames` of the way to `target`; returns how far it was. */
float EntityMath_EaseToward(float *value, float target, float frames);

/* `angle` one step towards `target` the short way round. */
float EntityMath_ApproachAngle(float angle, float target, float step);

/* Turns *angle one step towards `target` the short way round; returns the difference it had. */
float EntityMath_StepAngleToward(float *angle, float target, float step);

/* Turns *angle 1/`rate` of the way to `goal` the short way round; returns the difference it had. */
float EntityMath_EaseAngleToward(float *angle, float goal, float rate);

/* atan2 of the x and z distances from `point_a` to `point_b`. */
float EntityMath_AngleXZ(float *point_a, float *point_b);

/* The heading that faces `target` from `origin`. */
float EntityMath_HeadingTo(float *origin, float *target);

/* (sin(angle), 0, cos(angle), 1). */
void EntityMath_SinCosVector(float *vector, float angle);

/* (-sin(angle), 0, -cos(angle), 1): the way a model with heading `angle` faces. */
void EntityMath_FacingVector(Vector4 *vector, float angle);

/* The rotation (pitch, heading) that points a model along `direction`. */
void EntityMath_VectorToRotation(float *angles, float *direction);

/* The angles (pitch, atan2 of z and x) of `direction`. */
void EntityMath_VectorToAngles(float *angles, float *direction);

/* `angle_to - angle_from`, wrapped into -pi..pi. */
float EntityMath_AngleDifference(float angle_from, float angle_to);

#endif
