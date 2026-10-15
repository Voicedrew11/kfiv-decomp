/* Small sums the entity code shares: clamps, distances, and angles wrapped to -pi..pi, stepped or eased
   towards a goal. */
#include "kf_entity_math.h"

float atan2f(float, float);
float sinf(float);
float cosf(float);
float sqrtf(float);

/* `value + amount`, kept within `low`..`high`. Nothing calls it. */
s32 EntityMath_AddClamped(s32 value, s32 amount, s32 low, s32 high) {
    s32 clamped;
    s32 sum;

    sum = value + amount;
    clamped = (sum < low) ? low : sum;
    return (high < clamped) ? high : clamped;
}

/* `value - amount`, kept within `low`..`high`. Nothing calls it. */
s32 EntityMath_SubtractClamped(s32 value, s32 amount, s32 low, s32 high) {
    s32 clamped;
    s32 difference;

    difference = value - amount;
    clamped = (difference < low) ? low : difference;
    return (high < clamped) ? high : clamped;
}

/* dst = a * (1 - t) + b * t across the xyz lanes, on the VU0. */
void EntityMath_LerpVector(void *dst, void *src_a, void *src_b, float ratio) {
    __asm__ volatile(
        "mfc1 $2, %3\n"
        "qmtc2.ni $2, $vf3\n"
        "vsubx.w $vf4, $vf0, $vf3x\n"
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x0(%2)\n"
        "vmulax.xyz ACC, $vf2, $vf3x\n"
        "vmaddw.xyz $vf1, $vf1, $vf4w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf1, 0x0(%0)\n"
        : : "r"(dst), "r"(src_a), "r"(src_b), "f"(ratio) : "$2");
}

/* The distance between two points. */
f32 EntityMath_Distance(f32 *point_a, f32 *point_b) {
    float result[4];

    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vaddy.x $vf16, $vf16, $vf16y\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(point_a), "r"(point_b), "r"(result) : "memory");
    return result[0];
}

/* The distance between two points, height left out. */
float EntityMath_DistanceXZ(float *point_a, float *point_b) {
    float result[4];

    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(point_a), "r"(point_b), "r"(result) : "memory");
    return result[0];
}

/* Wraps an angle in radians into -pi..pi. */
float EntityMath_WrapAngle(float radians) {
    s32 turns;
    float wrapped;

    turns = radians / 6.2831855f;
    wrapped = radians - (float)turns * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}

/* Moves `current` one step towards `target`, landing on it when less than a step away. */
f32 EntityMath_Approach(f32 current, f32 target, f32 step) {
    s32 steps;

    if (current == target) {
        return target;
    }
    if (step == 0.0f) {
        return current;
    }
    steps = (s32)((target - current) / step);
    if (steps > 0) {
        current += step;
    } else if (steps < 0) {
        current -= step;
    } else {
        current = target;
    }
    return current;
}

/* Moves *current one step towards `target` (landing on it when less than a step away) and returns how far it
   was. */
float EntityMath_StepToward(float *current, float target, float step) {
    float difference;
    s32 steps;

    if (*current == target) {
        return 0.0f;
    }
    difference = target - *current;
    if (step == 0.0f) {
        return difference;
    }
    steps = (s32)(difference / step);
    if (steps > 0) {
        *current += step;
    } else if (steps < 0) {
        *current -= step;
    } else {
        *current = target;
    }
    return difference;
}

/* Moves *value 1/`frames` of the way to `target` (all of it when `frames` is 1 or less) and returns how far
   it was. */
float EntityMath_EaseToward(float *value, float target, float frames) {
    float old;
    float gap;

    if (frames <= 1.0f) {
        *value = target;
        return 0.0f;
    }
    old = *value;
    gap = target - old;
    *value = old + gap / frames;
    return gap;
}

/* Turns the angle `angle` one step towards `target` the short way round and returns it wrapped. Nothing calls
   it. */
float EntityMath_ApproachAngle(float angle, float target, float step) {
    float delta;
    float difference;
    s32 steps;

    if (angle == target) {
        return target;
    }
    if (step == 0.0f) {
        return angle;
    }
    delta = target - angle;
    difference = delta - (float)(s32)(delta / 6.2831855f) * 6.2831855f;
    if (difference > 3.1415927f) {
        difference -= 6.2831855f;
    } else if (difference < -3.1415927f) {
        difference += 6.2831855f;
    }
    steps = (s32)(difference / step);
    if (steps > 0) {
        angle += step;
    } else if (steps < 0) {
        angle -= step;
    } else {
        angle = target;
    }
    angle -= (float)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

/* Turns *angle one step towards `target` the short way round; returns the wrapped difference it had. Nothing
   calls it. */
float EntityMath_StepAngleToward(float *angle, float target, float step) {
    float old = *angle;
    float difference;
    float wrapped;
    float next;
    s32 steps;

    if (old == target) {
        return 0.0f;
    }
    difference = target - old;
    if (step == 0.0f) {
        return difference;
    }
    wrapped = difference;
    wrapped -= (float)(s32)(wrapped / 6.2831855f) * 6.2831855f;
    if (wrapped > 3.1415927f) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    steps = (s32)(wrapped / step);
    if (steps > 0) {
        *angle = old + step;
    } else if (steps < 0) {
        *angle = old - step;
    } else {
        *angle = target;
    }
    next = *angle;
    next -= (float)(s32)(next / 6.2831855f) * 6.2831855f;
    if (next > 3.1415927f) {
        next -= 6.2831855f;
    } else if (next < -3.1415927f) {
        next += 6.2831855f;
    }
    *angle = next;
    return wrapped;
}

/* Turns *angle 1/`rate` of the way to `goal` the short way round (all of it when `rate` is 1 or less);
   returns the wrapped difference it had. The constants are literals: retail's .lit4 D_00445D0C..D_00445D20
   are these six in load order, and an extern const version does not match. */
float EntityMath_EaseAngleToward(float *angle, float goal, float rate) {
    float old = *angle;
    float difference = goal - old;

    difference -= (float)(s32)(difference / 6.2831855f) * 6.2831855f;
    if (difference > 3.1415927f) {
        difference -= 6.2831855f;
    } else if (difference < -3.1415927f) {
        difference += 6.2831855f;
    }
    if (rate <= 1.0f) {
        *angle = goal;
        return difference;
    }
    rate = old + difference / rate; // match: the new angle reuses the register of `rate`
    rate -= (float)(s32)(rate / 6.2831855f) * 6.2831855f;
    if (rate > 3.1415927f) {
        rate -= 6.2831855f;
    } else if (rate < -3.1415927f) {
        rate += 6.2831855f;
    }
    *angle = rate;
    return difference;
}

/* atan2 of the x and z distances from `point_a` to `point_b` (EntityMath_HeadingTo less pi). */
float EntityMath_AngleXZ(float *point_a, float *point_b) {
    return atan2f(point_b[0] - point_a[0], point_b[2] - point_a[2]);
}

/* The heading that faces `target` from `origin` (a model faces -z), wrapped to -pi..pi. */
float EntityMath_HeadingTo(float *origin, float *target) {
    float angle;
    float wrapped;

    angle = atan2f(target[0] - origin[0], target[2] - origin[2]);
    wrapped = angle + 3.1415927f;
    wrapped -= (float)(s32)(wrapped / 6.2831855f) * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}

/* Builds the vector (sin(angle), 0, cos(angle), 1). Nothing calls it. */
void EntityMath_SinCosVector(float *vector, float angle) {
    float sine;

    sine = sinf(angle);
    ((s32 *)vector)[1] = 0; // match: the 0 is stored as an integer
    vector[0] = sine;
    vector[2] = cosf(angle);
    vector[3] = 1.0f;
}

/* Builds the vector (-sin(angle), 0, -cos(angle), 1): the way a model with heading `angle` faces. */
void EntityMath_FacingVector(Vector4 *vector, float angle) {
    vector->x = -sinf(angle);
    vector->y = 0.0f;
    vector->z = -cosf(angle);
    vector->w = 1.0f;
}

/* The rotation (pitch, heading) that points a model along `direction`, as the shot code turns shots along
   their bones. Nothing calls it. */
void EntityMath_VectorToRotation(float *angles, float *direction) {
    float heading;

    angles[0] = -atan2f(direction[1], sqrtf(direction[0] * direction[0] + direction[2] * direction[2]));
    heading = -atan2f(direction[2], direction[0]) + 1.5707964f;
    heading -= (float)(s32)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    angles[1] = heading;
    angles[2] = angles[3] = 0.0f;
}

/* The angles (pitch, atan2 of z and x) of `direction`. Nothing calls it. */
void EntityMath_VectorToAngles(float *angles, float *direction) {
    float yaw;

    angles[0] = -atan2f(direction[1], sqrtf(direction[0] * direction[0] + direction[2] * direction[2]));
    yaw = -atan2f(direction[2], direction[0]);
    yaw -= (float)(s32)(yaw / 6.2831855f) * 6.2831855f;
    if (yaw > 3.1415927f) {
        yaw -= 6.2831855f;
    } else if (yaw < -3.1415927f) {
        yaw += 6.2831855f;
    }
    angles[1] = yaw;
    angles[2] = angles[3] = 0.0f;
}

/* `angle_to - angle_from`, wrapped into -pi..pi. */
float EntityMath_AngleDifference(float angle_from, float angle_to) {
    float difference = angle_to - angle_from;

    difference -= (s32)(difference / 6.2831855f) * 6.2831855f;
    if (difference > 3.1415927f) {
        difference -= 6.2831855f;
    } else if (difference < -3.1415927f) {
        difference += 6.2831855f;
    }
    return difference;
}
