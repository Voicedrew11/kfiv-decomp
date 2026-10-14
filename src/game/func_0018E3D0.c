#include "kf_entity.h"

void Vector_Copy(void *, void *);
s32 func_0018C0D0(ModelInstance *, void *, f32 *, f32 *, f32 *);

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

/* func_0018DDA8 for a part that is to look at a point. mode 1: `angles` turn towards the angles
   func_0018C0D0 gives for `target` (FALSE if it gives none) and go into the part; mode 0 and other
   modes as in func_0018DDA8. FALSE: no model or no part. */
int func_0018E3D0(void *model, int part_arg, int angles_arg, int mode, void *target, int limits_arg, float rate) {
    ModelPart *part = (ModelPart *)part_arg;
    f32 *angles = (f32 *)angles_arg;
    f32 goal[4];

    if (model == NULL || part == NULL) {
        return FALSE;
    }
    if (mode == 1) {
        if (func_0018C0D0(model, part, target, (f32 *)limits_arg, goal) == 0) {
            return FALSE;
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
