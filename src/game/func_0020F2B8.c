#include "kf_collision.h"

extern const f32 D_00446FC8[];      /* -0.0f */

float sqrtf(float);
void KF_Vector_Subtract(void *, void *, void *);
void func_00216778(void *, void *);
void sceVu0Normalize(float *, float *);

/* Tests a ray (from `origin` along `direction` for `length`) against an upright cylinder: `base` on
   its axis, its radius and its height. Seen from above, finds the point of the ray nearest the axis;
   leaves that point, or where the ray enters the cylinder, and how far along the ray that is. */
void func_0020F2B8(f32 radius, f32 height, f32 length, f32 *base, f32 *origin, f32 *direction) {
    f32 start[4];
    f32 end[4];
    f32 along[4];
    f32 offset[4];
    f32 heading[4];
    f32 nearest[4];
    f32 radius_squared;
    f32 distance_squared;
    f32 facing;
    f32 part;           /* how far along the ray the nearest point is, 0 to 1 */
    f32 inside;
    f32 across;
    f32 ahead;
    f32 delta_x;
    f32 delta_z;

    radius_squared = radius * radius;
    COLLISION_RESULT->hit = 0;
    func_00216778(start, origin);
    end[0] = origin[0] + direction[0] * length;
    end[1] = origin[1] + direction[1] * length;
    end[2] = origin[2] + direction[2] * length;
    if (base[1] < end[1] && base[1] < start[1]) {
        return;
    }
    if (end[1] < base[1] - height && start[1] < base[1] - height) {
        return;
    }
    KF_Vector_Subtract(along, end, start);
    heading[0] = along[0];
    heading[1] = 0.0f;
    heading[2] = along[2];
    if (radius_squared + length * length < along[0] * along[0] + along[2] * along[2]) {
        return;
    }
    sceVu0Normalize(heading, heading);
    facing = heading[0] * along[0] + heading[1] * along[1] + heading[2] * along[2];
    if (-0.000001f < facing && facing < 0.000001f) {
        nearest[0] = start[0];
        nearest[1] = base[1];
        nearest[2] = start[2];
        KF_Vector_Subtract(along, base, nearest);
        distance_squared = along[0] * along[0] + along[2] * along[2];
        if (!(distance_squared <= radius_squared)) {
            return;
        }
        *&COLLISION_RESULT->hit = 1;
        *&COLLISION_RESULT->point[0] = nearest[0];
        *&COLLISION_RESULT->point[1] = nearest[1];
        *&COLLISION_RESULT->point[2] = nearest[2];
    } else {
        part = heading[0] * base[0] + heading[1] * base[1] + heading[2] * base[2];
        part += -heading[0] * start[0] - heading[1] * start[1] - heading[2] * start[2];
        facing = 1.0f / facing;
        part *= facing;
        nearest[0] = start[0] + along[0] * part;
        nearest[1] = start[1] + along[1] * part;
        nearest[2] = start[2] + along[2] * part;
        if (part < D_00446FC8[0]) {
            KF_Vector_Subtract(offset, base, start);
            distance_squared = offset[0] * offset[0] + offset[2] * offset[2];
            if (!(distance_squared <= radius_squared)) {
                return;
            }
            delta_x = nearest[0] - base[0];
            delta_z = nearest[2] - base[2];
            delta_z = delta_x * delta_x + delta_z * delta_z;
            delta_x = sqrtf(radius_squared - delta_z);
            delta_x -= sqrtf(distance_squared - delta_z);
            delta_z = sqrtf(along[0] * along[0] + along[2] * along[2]);
            *&COLLISION_RESULT->hit = 1;
            *&COLLISION_RESULT->distance = length * (delta_z - delta_x) / delta_z;
            *&COLLISION_RESULT->point[0] = nearest[0];
            *&COLLISION_RESULT->point[1] = nearest[1];
            *&COLLISION_RESULT->point[2] = nearest[2];
        } else if (1.0f < part) {
            KF_Vector_Subtract(offset, base, end);
            distance_squared = offset[0] * offset[0] + offset[2] * offset[2];
            if (!(distance_squared <= radius_squared)) {
                return;
            }
            delta_x = nearest[0] - base[0];
            delta_z = nearest[2] - base[2];
            delta_z = delta_x * delta_x + delta_z * delta_z;
            delta_x = sqrtf(radius_squared - delta_z);
            delta_x -= sqrtf(distance_squared - delta_z);
            delta_z = sqrtf(along[0] * along[0] + along[2] * along[2]);
            *&COLLISION_RESULT->hit = 1;
            *&COLLISION_RESULT->distance = length * (delta_z - delta_x) / delta_z;
            *&COLLISION_RESULT->point[0] = nearest[0];
            *&COLLISION_RESULT->point[1] = nearest[1];
            *&COLLISION_RESULT->point[2] = nearest[2];
        } else {
            KF_Vector_Subtract(offset, base, nearest);
            distance_squared = offset[0] * offset[0] + offset[2] * offset[2];
            if (!(distance_squared <= radius_squared)) {
                return;
            }
            if (!(nearest[1] <= base[1])) {
                return;
            }
            if (!(base[1] - height <= nearest[1])) {
                return;
            }
            across = base[0] - start[0];
            ahead = base[2] - start[2];
            across = across * across + ahead * ahead;
            if (across < radius_squared) {
                *&COLLISION_RESULT->distance = 0.0f;
                *&COLLISION_RESULT->point[0] = start[0];
                *&COLLISION_RESULT->point[1] = start[1];
                *&COLLISION_RESULT->point[2] = start[2];
            } else {
                inside = sqrtf(radius_squared - distance_squared);
                KF_Vector_Subtract(offset, nearest, start);
                *&COLLISION_RESULT->distance = sqrtf(offset[0] * offset[0] + offset[2] * offset[2]) - inside;
                *&COLLISION_RESULT->point[0] = nearest[0];
                *&COLLISION_RESULT->point[1] = nearest[1];
                *&COLLISION_RESULT->point[2] = nearest[2];
            }
            *&COLLISION_RESULT->hit = 1;
        }
    }
}
