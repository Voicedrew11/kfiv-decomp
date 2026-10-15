/* Stepping values and angles toward a target, angle wrapping, a triangle normal. */
#include "common.h"
#include "kf_angle.h"

void Vector_Subtract(float *dst, float *a, float *b);

static int Step_TowardInt(int *value, int target, int step);
static int Step_Toward(f32 target, f32 step, f32 *value);
static float Angle_WrapPositive(float radians);
static int Angle_CountSteps(float from, float target, float step);

/* Cross product of first and second into out (as Vector_Cross). Nothing calls it. */
void func_0019B620(float *out, float *first, float *second) {
    out[0] = (first[1] * second[2]) - (first[2] * second[1]);
    out[1] = (first[2] * second[0]) - (first[0] * second[2]);
    out[2] = (first[0] * second[1]) - (first[1] * second[0]);
}

/* Cross product of (second - first) and (third - second), written to out: the normal of a triangle, not
   normalised. */
void Vector_TriangleNormal(float *out, float *first, float *second, float *third) {
    float edge_ab[4];
    float edge_bc[4];

    Vector_Subtract(edge_ab, second, first);
    Vector_Subtract(edge_bc, third, second);
    out[0] = edge_ab[1] * edge_bc[2] - edge_ab[2] * edge_bc[1];
    out[1] = edge_ab[2] * edge_bc[0] - edge_ab[0] * edge_bc[2];
    out[2] = edge_ab[0] * edge_bc[1] - edge_ab[1] * edge_bc[0];
}

/* Moves *value toward target by |step|; returns 1 once it reaches target. */
static int Step_TowardInt(int *value, int target, int step) {
    int current = *value;

    step = ABS(step);
    if (current == target) {
        return TRUE;
    }
    if (current < target) {
        if (target - current <= step) {
            *value = target;
            return TRUE;
        }
        *value = current + step;
        return FALSE;
    } else {
        if (current - target <= step) {
            *value = target;
            return TRUE;
        }
        *value = current - step;
        return FALSE;
    }
}

/* Moves *value toward target by at most |step|; returns 1 once it has arrived. */
static int Step_Toward(f32 target, f32 step, f32 *value) {
    if (!(0.0f <= step)) {
        step = -step;
    }
    if (target - 1.1920929e-7f < *value && *value < target + 1.1920929e-7f) {
        *value = target;
        return 1;
    }
    if (*value < target) {
        if (target - *value <= step) {
            *value = target;
            return 1;
        }
        *value += step;
        return 0;
    }
    if (*value - target <= step) {
        *value = target;
        return 1;
    }
    *value -= step;
    return 0;
}

/* Wraps an angle to -pi..pi. */
float Angle_Wrap(float angle) {
    float wrapped;
    int turns;

    turns = angle / 6.2831855f;
    wrapped = angle - (float)turns * 6.2831855f;
    if (3.1415927f < wrapped) {
        wrapped -= 6.2831855f;
    } else if (wrapped < -3.1415927f) {
        wrapped += 6.2831855f;
    }
    return wrapped;
}

/* Wraps an angle to 0..2pi by repeated subtraction or addition. */
static float Angle_WrapPositive(float radians) {
    float angle;

    angle = radians;
    if (6.2831855f <= angle) {
        do {
            angle -= 6.2831855f;
        } while (6.2831855f <= angle);
    }
    if (angle < 0.0f) {
        do {
            angle += 6.2831855f;
        } while (angle < 0.0f);
    }
    return angle;
}

/* Moves *angle toward target by step the shorter way round; returns 1 once it has arrived. */
int Angle_Approach(float *angle, float target, float step) {
    float forward, backward;

    *angle = Angle_Wrap(*angle);
    target = Angle_Wrap(target);
    if (!(0.0f <= step)) {
        step = -step;
    }
    if (*angle == target) {
        return 1;
    }
    forward = Angle_WrapPositive(target - *angle);
    backward = Angle_WrapPositive(*angle - target);
    if (forward < backward) {
        if (forward <= step) {
            *angle = target;
            return 1;
        }
        *angle += step;
    } else {
        if (backward <= step) {
            *angle = target;
            return 1;
        }
        *angle -= step;
    }
    return 0;
}

/* Eases an angle toward target over frames steps along the shortest way round; returns 1 once it has
   arrived. */
int Angle_EaseToward(float target, float *angle, int frames) {
    float diff;

    *angle = Angle_Wrap(*angle);
    target = Angle_Wrap(target);
    if (*angle == target) {
        return 1;
    }
    if (frames < 2) {
        *angle = target;
        return 1;
    }
    diff = target - *angle;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    *angle = *angle + diff / frames;
    *angle = Angle_Wrap(*angle);
    return 0;
}

/* The angular distance between two angles in units of step, at least 1 unless they are equal. */
static int Angle_CountSteps(float from, float target, float step) {
    float start, end, forward, backward, count;

    start = Angle_Wrap(from);
    end = Angle_Wrap(target);
    if (start == end) {
        return 0;
    }
    forward = Angle_WrapPositive(end - start);
    backward = Angle_WrapPositive(start - end);
    count = Angle_Wrap(forward - backward);
    count = (0.0f <= count ? count : -count) / step;
    if (count <= 1.0f) {
        return 1;
    }
    return (int)count;
}
