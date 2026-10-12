#include "common.h"
#include "kf_effect.h"

#define TWO_PI 6.2831855f
#define PI 3.1415927f

extern Entity *D_004467E0;

int func_001B0040(Entity *entity);
float func_001B0178(void *, void *);
f32 func_001B0950(f32);
void func_001AF5B0(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);

/* Wraps an angle into -pi..pi. */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (float)(int)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    return angle;
}

/* The entity in the effect's range that lies closest to the direction the reference angles point in, or 0:
   it must be within the definition's unkE4 of the pitch and unkE8 of the heading. */
int func_001AFD08(EffectActor *effect, Vector4 *position, Vector4 *reference) {
    Vector4 angles;
    Vector4 offset;
    Entity *entity;
    Entity *best = NULL;
    f32 best_score = 65535.0f;
    f32 distance;
    f32 score;

    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (func_001B0040(entity) != 0) {
            continue;
        }
        distance = func_001B0178(&ENTITY_MODEL(entity)->position, position);
        if ((f32)effect->def->interval < distance) {
            continue;
        }
        KF_Vector_Subtract(&offset, &ENTITY_MODEL(entity)->position, position);
        offset.y -= entity->height * 0.5f;
        func_001AF5B0(&angles, &offset);
        angles.x = wrap_angle(angles.x - reference->x);
        angles.y = wrap_angle(angles.y - reference->y);
        angles.x = func_001B0950(angles.x);
        angles.y = func_001B0950(angles.y);
        if (effect->def->unkE4 <= angles.x) {
            continue;
        }
        if (effect->def->unkE8 <= angles.y) {
            continue;
        }
        score = distance * (angles.x + 1.0f) * (angles.y + 1.0f);
        if (score < 0.0f) {
            score = -score;
        }
        if (score < best_score) {
            best_score = score;
            best = entity;
        }
    }
    return (int)best;
}
