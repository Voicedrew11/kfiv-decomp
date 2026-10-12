#include "kf_entity.h"

float func_00195388(float *, float, float);
void func_001958B8(char *, void (*)(void), int);
void func_00182BD0(char *);
void func_001818B8(Entity *);

/* Action: levels the model out (pitch and roll back to 0), then goes back to its handler. */
void func_00129CA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 pitch_left;
    f32 roll_left;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkC6 = 0x2000;
    pitch_left = func_00195388(&entity->model->unkD0[0], 0.0f, 6.0f);
    roll_left = func_00195388(&entity->model->unkD0[2], 0.0f, 6.0f);
    if (entity->flags & (0x8200L << 35)) {
        pitch_left = ABS(pitch_left);
        roll_left = ABS(roll_left);
        if (ABS(pitch_left) < 0.01f && ABS(roll_left) < 0.01f) {
            entity->model->unkD0[0] = 0.0f;
            entity->model->unkD0[2] = 0.0f;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            unkE0->unk08 = (void (*)(void))func_00182BD0;
            unkE0->unk14 = (void (*)(void))func_001818B8;
            func_001958B8((char *)entity, unkE0->unk18, 1);
        }
    }
}
