/* What the effect kinds share: the scale, spin and colour keys of an effect's definition, the hit check
   that ends an effect, angle and vector conversions, the placement and turn of a new effect's model,
   steering, and the entity searches effects aim with. */
#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

void Vector_Copy(void *, void *);
void sceVu0ScaleVectorXYZ(void *, void *, float);
void sceVu0AddVector(void *, void *, void *);
/* Wraps an angle into -pi..pi. */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}
int func_001B4F90(void *effect);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
extern f32 D_00431AF0[16];
float sqrtf(float);
float atan2f(float, float);
int rand(void);
double sin(double);
double cos(double);
#define TWO_PI 6.2831855f
#define PI 3.1415927f
void Vector_Normalize(void *dst, void *src);
void Vector_Scale(void *, void *, float);
void Vector_Add(void *, void *, void *);
extern Entity *D_004467E0;
int func_001B0040(Entity *entity);
float func_001B0178(void *, void *);
f32 func_001B0950(f32);
void Vector_Subtract(void *, void *, void *);

int EffectKeys_ApplySpin(EffectActor *effect);
int EffectKeys_ApplyColour(EffectActor *effect);
void EffectHit_CheckAndEnd(EffectActor *effect, void *state);
int EffectHit_CheckAndEndByFlag(EffectActor *effect, s8 *state);
void EffectHit_End(EffectActor *effect, s8 *state);
int EffectHit_Check(EffectActor *effect);
void EffectAngle_ToVector(float length, Vector4 *out, Vector4 *angles);
void EffectAngle_FromVector(void *angles, void *direction);
void EffectStart_PlaceModel(EffectActor *effect, EffectSlot *spawn_slot);
void EffectStart_TurnModel(EffectActor *effect, EffectSlot *spawn_slot);
static int EffectKeys_FindStage(void *effect, int *frames);
static void EffectKeys_Weights(f32 *result, f32 *eased, int value, int min, int max, int mode);
void EffectSteer_Toward(Vector4 *velocity, Vector4 *direction, float speed, float turn);
int EffectHit_FindAimedEntity(EffectActor *effect, Vector4 *position, Vector4 *reference);
Entity *EffectHit_FindNearestEntity(EffectActor *effect, Vector4 *position, Vector4 *unused_rotation);

/* Sets the model's scale from the definition's scale keys; returns the key reached, or -1 while it blends
   between two. */
int EffectKeys_ApplyScale(EffectActor *effect) {
    Vector4 scaled_a;
    Vector4 scaled_b;
    int keyframe_frames[4];
    f32 blend_a;
    f32 blend_b;
    int stage;

    keyframe_frames[0] = 0;
    keyframe_frames[1] = effect->def->keyframes_a[0];
    keyframe_frames[2] = effect->def->keyframes_a[1];
    keyframe_frames[3] = effect->def->keyframe_end;
    stage = EffectKeys_FindStage(effect, keyframe_frames);
    if (keyframe_frames[stage + 1] - keyframe_frames[stage] <= 0) {
        Vector_Copy(&effect->model->unkE0, &effect->def->keys_a[stage]);
        return stage;
    }
    EffectKeys_Weights(&blend_a, &blend_b, effect->frame, keyframe_frames[stage], keyframe_frames[stage + 1], effect->def->modes_a[stage]);
    sceVu0ScaleVectorXYZ(&scaled_a, &effect->def->keys_a[stage], blend_a);
    sceVu0ScaleVectorXYZ(&scaled_b, &effect->def->keys_a[stage + 1], blend_b);
    sceVu0AddVector(&effect->model->unkE0, &scaled_a, &scaled_b);
    return -1;
}

/* Turns the model by the definition's spin keys, each angle wrapped; returns the key reached, or -1 while it
   blends between two. */
int EffectKeys_ApplySpin(EffectActor *effect) {
    Vector4 scaled_a;
    Vector4 scaled_b;
    int keyframe_frames[4];
    f32 blend_a;
    f32 blend_b;
    int stage;

    keyframe_frames[0] = 0;
    keyframe_frames[1] = effect->def->keyframes_b[0];
    keyframe_frames[2] = effect->def->keyframes_b[1];
    keyframe_frames[3] = effect->def->keyframe_end;
    stage = EffectKeys_FindStage(effect, keyframe_frames);
    if (keyframe_frames[stage + 1] - keyframe_frames[stage] <= 0) {
        sceVu0AddVector(&effect->model->rotation, &effect->model->rotation, &effect->def->keys_b[stage]);
        effect->model->rotation.x = wrap_angle(effect->model->rotation.x);
        effect->model->rotation.y = wrap_angle(effect->model->rotation.y);
        effect->model->rotation.z = wrap_angle(effect->model->rotation.z);
        return stage;
    }
    EffectKeys_Weights(&blend_a, &blend_b, effect->frame, keyframe_frames[stage], keyframe_frames[stage + 1], effect->def->modes_b[stage]);
    sceVu0ScaleVectorXYZ(&scaled_a, &effect->def->keys_b[stage], blend_a);
    sceVu0ScaleVectorXYZ(&scaled_b, &effect->def->keys_b[stage + 1], blend_b);
    sceVu0AddVector(&scaled_a, &scaled_a, &scaled_b);
    sceVu0AddVector(&effect->model->rotation, &scaled_a, &effect->model->rotation);
    effect->model->rotation.x = wrap_angle(effect->model->rotation.x);
    effect->model->rotation.y = wrap_angle(effect->model->rotation.y);
    effect->model->rotation.z = wrap_angle(effect->model->rotation.z);
    return -1;
}

/* Sets the effect's colour from the definition's colour keys; returns the key reached, or -1 while it
   blends between two. */
int EffectKeys_ApplyColour(EffectActor *effect) {
    Vector4 unused_a; // match: retail reserves the two vectors its sibling functions scale into
    Vector4 unused_b;
    int keyframe_frames[4];
    f32 blend_a;
    f32 blend_b;
    int stage;

    keyframe_frames[0] = 0;
    keyframe_frames[1] = effect->def->colour_keyframes[0];
    keyframe_frames[2] = effect->def->colour_keyframes[1];
    keyframe_frames[3] = effect->def->keyframe_end;
    stage = EffectKeys_FindStage(effect, keyframe_frames);
    if (keyframe_frames[stage + 1] - keyframe_frames[stage] <= 0) {
        EFFECT_COLOUR(effect)->red = effect->def->colour_keys[stage][0];
        EFFECT_COLOUR(effect)->green = effect->def->colour_keys[stage][1];
        EFFECT_COLOUR(effect)->blue = effect->def->colour_keys[stage][2];
        EFFECT_COLOUR(effect)->alpha = effect->def->colour_keys[stage][3];
        return stage;
    }
    EffectKeys_Weights(&blend_a, &blend_b, effect->frame, keyframe_frames[stage], keyframe_frames[stage + 1], effect->def->colour_modes[stage]);
    EFFECT_COLOUR(effect)->red = effect->def->colour_keys[stage][0] * blend_a + effect->def->colour_keys[stage + 1][0] * blend_b;
    EFFECT_COLOUR(effect)->green = effect->def->colour_keys[stage][1] * blend_a + effect->def->colour_keys[stage + 1][1] * blend_b;
    EFFECT_COLOUR(effect)->blue = effect->def->colour_keys[stage][2] * blend_a + effect->def->colour_keys[stage + 1][2] * blend_b;
    EFFECT_COLOUR(effect)->alpha = effect->def->colour_keys[stage][3] * blend_a + effect->def->colour_keys[stage + 1][3] * blend_b;
    return -1;
}

/* Once the hit check finds something, spawns def->end_effect and marks *state; the next call ends the
   effect. */
void EffectHit_CheckAndEnd(EffectActor *effect, void *state_arg) {
    s8 *state = state_arg;

    if (*state == 1) {
        Effect_Free(effect);
        *state = 0;
    } else if (effect->effect_param != 0xFFFF && effect->def->unk10B != 0xFF && func_001B4F90(effect) != 0) {
        if (effect->def->end_effect != 0xFFFF) {
            Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            *state = 1;
        }
    } else {
        *state = 0;
    }
}

/* EffectHit_CheckAndEnd that spawns def->unkF2 instead for hit flag 2 (and does not end); returns 1 for
   flag 2, 2 for another hit, else 0. */
int EffectHit_CheckAndEndByFlag(EffectActor *effect, s8 *state) {
    int hit_flags;

    if (*state == 1) {
        Effect_Free(effect);
        *state = 0;
        return 0;
    }
    if (effect->effect_param != 0xFFFF && effect->def->unk10B != 0xFF) {
        hit_flags = func_001B4F90(effect);
        if (hit_flags & 2) {
            if (effect->def->unkF2 != 0) {
                Effect_Spawn(effect->def->unkF2, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            return 1;
        }
        if (hit_flags != 0) {
            if (effect->def->end_effect != 0xFFFF) {
                Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                *state = 1;
            }
            return 2;
        }
    }
    *state = 0;
    return 0;
}

/* Spawns def->end_effect and marks *state, or ends the effect when *state is already marked. */
void EffectHit_End(EffectActor *effect, s8 *state) {
    if (*state == 1) {
        Effect_Free(effect);
        return;
    }
    Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    *state = 1;
}

/* 1 when the effect has a hit check (effect_param and def->unk10B set) and it finds something, else 0. */
int EffectHit_Check(EffectActor *effect) {
    if (effect->effect_param != 0xFFFF) {
        if (effect->def->unk10B != 0xFF) {
            if (func_001B4F90(effect) != 0) {
                return 1;
            }
        }
    }
    return 0;
}

/* Sets out to a vector of the given length along z, turned by angles (x pitch, y heading). */
void EffectAngle_ToVector(float length, Vector4 *out, Vector4 *angles) {
    f32 matrix[16];

    out->x = out->y = out->w = 0.0f;
    out->z = length;
    sceVu0RotMatrixX(matrix, D_00431AF0, angles->x);
    sceVu0RotMatrixY(matrix, matrix, angles->y);
    sceVu0ApplyMatrix(&out->x, matrix, &out->x);
}

/* Writes the pitch and heading of a direction vector to angles[0] and angles[1]. */
void EffectAngle_FromVector(void *angles_arg, void *direction_arg) {
    float *angles = angles_arg;
    float *direction = direction_arg;
    float negated_y = -direction[1];
    float horizontal = sqrtf(direction[0] * direction[0] + direction[2] * direction[2]);

    angles[0] = atan2f(negated_y, horizontal);
    angles[1] = atan2f(direction[0], direction[2]);
}

/* Places the model of a new effect: the definition's offset with a random spread, turned by the start
   rotation or the slot's, from the start position or the slot's. */
void EffectStart_PlaceModel(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 offset;
    Vector4 rotated;
    Vector4 origin;
    f32 pitch_sine;
    f32 pitch_cosine;
    f32 heading_sine;
    f32 heading_cosine;

    if (EFFECT_DEF_FLAG_WORD(effect->def) & 0x10000) {
        Vector_Copy(&effect->model->position, effect->def->offset_base);
        Vector_Copy(&origin, effect->start_position);
        pitch_sine = sin(-effect->start_rotation->x);
        pitch_cosine = cos(-effect->start_rotation->x);
        heading_sine = sin(-effect->start_rotation->y);
        heading_cosine = cos(-effect->start_rotation->y);
    } else {
        Vector_Copy(&effect->model->position, &spawn_slot->position);
        Vector_Copy(&origin, &spawn_slot->position);
        pitch_sine = sin(-spawn_slot->unk10.x);
        pitch_cosine = cos(-spawn_slot->unk10.x);
        heading_sine = sin(-spawn_slot->unk10.y);
        heading_cosine = cos(-spawn_slot->unk10.y);
    }
    offset.x = effect->def->offset_base[0] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[0];
    offset.y = effect->def->offset_base[1] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[1];
    offset.z = effect->def->offset_base[2] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[2];
    if (EFFECT_DEF_FLAG_WORD(effect->def) & 0x40000) {
        rotated.y = pitch_cosine * offset.y + pitch_sine * offset.z;
        rotated.z = -pitch_sine * offset.y + pitch_cosine * offset.z;
        offset.z = rotated.z;
        offset.y = rotated.y;
    }
    rotated.x = heading_cosine * offset.x - heading_sine * offset.z;
    rotated.z = heading_sine * offset.x + heading_cosine * offset.z;
    offset.z = rotated.z;
    offset.x = rotated.x;
    sceVu0AddVector(&effect->model->position, &offset, &origin);
    Vector_Copy(&effect->unk70, &effect->model->position);
}

/* Sets the model's rotation for a new effect: the start rotation or the slot's, plus the definition's
   base rotation with a random spread. */
void EffectStart_TurnModel(EffectActor *effect, EffectSlot *spawn_slot) {
    if (EFFECT_DEF_FLAGS(effect->def) & 0x10000) {
        Vector_Copy(&effect->model->rotation, effect->start_rotation);
    } else {
        Vector_Copy(&effect->model->rotation, &spawn_slot->unk10);
    }
    if (!(EFFECT_DEF_FLAGS(effect->def) & 0x40000)) {
        effect->model->rotation.x = 0.0f;
    }
    if (EFFECT_DEF_FLAGS(effect->def) & 0x800000) {
        effect->model->rotation.z = 0.0f;
    }
    effect->model->rotation.x += effect->def->rotation_base[0] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[0];
    effect->model->rotation.y += effect->def->rotation_base[1] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[1];
    effect->model->rotation.z += effect->def->rotation_base[2] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[2];
    effect->model->rotation.x = wrap_angle(effect->model->rotation.x);
    effect->model->rotation.y = wrap_angle(effect->model->rotation.y);
    effect->model->rotation.z = wrap_angle(effect->model->rotation.z);
}

/* The stage of the three key spans that the effect's frame is in (or the first empty one), or -1. */
static int EffectKeys_FindStage(void *effect, int *frames) {
    EffectActor *actor = effect;
    s32 segment;
    s32 *pair = frames;

    for (segment = 0; segment < 3; segment++, pair++) {
        if ((actor->frame >= pair[0] && actor->frame < pair[1]) || pair[1] - pair[0] <= 0) {
            return segment;
        }
    }
    return -1;
}

/* The weights of a key span's two ends at value: eased progress from min to max (mode 1: squared, 2: ease
   out) in *eased, 1 less that in *result. */
static void EffectKeys_Weights(f32 *result, f32 *eased, int value, int min, int max, int mode) {
    f32 fraction = ((f32)value - (f32)min) / ((f32)max - (f32)min);
    *eased = fraction;
    if (mode == 1) {
        *eased = fraction * fraction;
    } else if (mode == 2) {
        f32 remaining = 1.0f - fraction;
        *eased = 1.0f - remaining * remaining;
    }
    *result = 1.0f - *eased;
}

/* Turns velocity toward direction by turn, then sets its length to speed. */
void EffectSteer_Toward(Vector4 *velocity, Vector4 *direction, float speed, float turn) {
    Vector4 pull;

    Vector_Normalize(&pull, direction);
    Vector_Scale(&pull, &pull, turn);
    Vector_Add(velocity, velocity, &pull);
    Vector_Normalize(velocity, velocity);
    Vector_Scale(velocity, velocity, speed);
}

/* The entity within def->interval of position that lies most nearly in the direction of the reference
   angles, inside def->unkE4 of pitch and def->unkE8 of heading; 0 when none. */
int EffectHit_FindAimedEntity(EffectActor *effect, Vector4 *position, Vector4 *reference) {
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
        Vector_Subtract(&offset, &ENTITY_MODEL(entity)->position, position);
        offset.y -= entity->height * 0.5f;
        EffectAngle_FromVector(&angles, &offset);
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

/* The nearest entity within def->interval of position that can be targeted, or NULL. */
Entity *EffectHit_FindNearestEntity(EffectActor *effect, Vector4 *position, Vector4 *unused_rotation) {
    Entity *entity;
    Entity *nearest = NULL;
    f32 nearest_distance = 65535.0f;
    f32 distance;
    f32 stack_reserve[8]; // match: unused, keeps the retail stack frame size

    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (func_001B0040(entity) != 0) {
            continue;
        }
        distance = func_001B0178(&ENTITY_MODEL(entity)->position, position);
        if ((f32)effect->def->interval < distance) {
            continue;
        }
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = entity;
        }
    }
    return nearest;
}
