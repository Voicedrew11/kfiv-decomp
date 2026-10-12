#include "kf_entity.h"

void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
void func_00194F48(f32 *, f32 *);

/* whether `point` lies within `radius` of the segment start..end (FALSE beyond its ends) */
int func_0018B3C0(float radius, int *start_arg, int end_arg, int point_arg) {
    f32 *start = (f32 *)start_arg;
    f32 *end = (f32 *)end_arg;
    f32 *point = (f32 *)point_arg;
    f32 along[4];
    f32 to_point[4];
    f32 nearest[4];
    f32 fraction;

    if (start[0] == end[0] && start[1] == end[1] && start[2] == end[2]) {
        return FALSE;
    }
    KF_Vector_Subtract(along, end, start);
    KF_Vector_Subtract(to_point, point, start);
    fraction = (along[0] * to_point[0] + along[1] * to_point[1] + along[2] * to_point[2]) /
               (along[0] * along[0] + along[1] * along[1] + along[2] * along[2]);
    if (fraction < 0.0f) {
        return FALSE;
    }
    if (fraction > 1.0f) {
        return FALSE;
    }
    KF_Vector_Scale(nearest, along, fraction);
    func_00216C48(nearest, nearest, start);
    // match: func_00194F48 returns the distance; other files declare it void
    if (((f32 (*)(f32 *, f32 *))func_00194F48)(point, nearest) <= radius) {
        return TRUE;
    }
    return FALSE;
}
