#include "kf_entity.h"

void Vector_Copy(void *, void *);

/* func_00195388, which the game inlines here: turns *angle towards `goal`, by the whole
   difference when `rate` is 1 or less, else by 1/rate of it */
static inline f32 turn_towards(f32 *angle, f32 goal, f32 rate) {
    f32 difference = goal - *angle;
    f32 turned;

    difference = difference - (f32)(s32)(difference / 6.2831855f) * 6.2831855f;
    if (difference > 3.1415927f) {
        difference -= 6.2831855f;
    } else if (difference < -3.1415927f) {
        difference += 6.2831855f;
    }
    if (rate <= 1.0f) {
        *angle = goal;
        return difference;
    }
    turned = *angle + difference / rate;
    turned = turned - (f32)(s32)(turned / 6.2831855f) * 6.2831855f;
    if (turned > 3.1415927f) {
        turned -= 6.2831855f;
    } else if (turned < -3.1415927f) {
        turned += 6.2831855f;
    }
    *angle = turned;
    return difference;
}

/* turns the three angles of a model part. mode 1: `angles` towards `goal` (kept within pitch
   limits[0]..limits[1] and yaw limits[2]..limits[3] if given) and into the part; mode 0: `angles`
   towards the part's own and into the part; else: `angles` are read from the part.
   FALSE: no part. */
s32 func_0018DDA8(ModelPart *part, f32 *angles, s32 mode, f32 *goal, f32 *limits, f32 rate) {
    if (part == NULL) {
        return FALSE;
    }
    if (mode == 1) {
        if (limits != NULL) {
            if (goal[0] < limits[0]) {
                goal[0] = limits[0];
            }
            if (limits[1] < goal[0]) {
                goal[0] = limits[1];
            }
            if (goal[1] < limits[2]) {
                goal[1] = limits[2];
            }
            if (limits[3] < goal[1]) {
                goal[1] = limits[3];
            }
        }
        turn_towards(&angles[0], goal[0], rate);
        turn_towards(&angles[1], goal[1], rate);
        turn_towards(&angles[2], goal[2], rate);
        Vector_Copy(&part->unk50, angles);
    } else if (mode == 0) {
        turn_towards(&angles[0], part->unk50, rate);
        turn_towards(&angles[1], part->unk54, rate);
        turn_towards(&angles[2], part->unk58, rate);
        Vector_Copy(&part->unk50, angles);
    } else {
        Vector_Copy(angles, &part->unk50);
    }
    return TRUE;
}
