#include "kf_model.h"

extern Vector4 D_00413F80;

double sqrt(double);

/* Finds the point on the segment start..end nearest the player (x/z only) and returns its distance.
   The floats are the player and the segment seen from each end. */
f32 func_0019E478(Vector4 *start, Vector4 *end, Vector4 *nearest, f32 start_dx, f32 start_dz, f32 end_dx, f32 end_dz, f32 back_dx, f32 back_dz, f32 seg_dx, f32 seg_dz) {
    f32 dot_start;
    f32 dot_end;
    f32 cos_start;
    f32 cos_end;
    f32 along;

    dot_start = start_dx * seg_dx + start_dz * seg_dz;
    cos_start = dot_start / (sqrt(start_dx * start_dx + start_dz * start_dz) * sqrt(seg_dx * seg_dx + seg_dz * seg_dz));
    dot_end = end_dx * back_dx + end_dz * back_dz;
    cos_end = dot_end / (sqrt(end_dx * end_dx + end_dz * end_dz) * sqrt(back_dx * back_dx + back_dz * back_dz));
    if (cos_start > 0.0f && cos_start < 1.0f && cos_end > 0.0f && cos_end < 1.0f) {
        along = dot_start / (seg_dx * seg_dx + seg_dz * seg_dz);
        nearest->x = start->x + seg_dx * along;
        nearest->y = 0.0f;
        nearest->z = start->z + seg_dz * along;
    } else if (cos_start <= 0.0f) {
        nearest->x = start->x;
        nearest->y = start->y;
        nearest->z = start->z;
    } else if (cos_end <= 0.0f) {
        nearest->x = end->x;
        nearest->y = end->y;
        nearest->z = end->z;
    }
    return sqrt((D_00413F80.x - nearest->x) * (D_00413F80.x - nearest->x) + (D_00413F80.z - nearest->z) * (D_00413F80.z - nearest->z));
}
