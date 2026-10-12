#include "kf_entity.h"

extern f32 D_00431AF0[16];

void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, int *, float *);
void KF_Vector_Scale(float *, float *, float);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
int func_0018AE98(float, void *, void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_00182BD0(char *);
void func_001818B8(Entity *);
void func_00129738(Entity *);
void func_00129780(Entity *);

/* Action: drops along its own up axis until it lands on a floor (awake) or on something else (asleep). */
void func_001299F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 landing[4];
    f32 direction[4];
    f32 matrix[16];
    s32 hit;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    direction[0] = 0.0f;
    direction[1] = 1.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    entity->unkC6 = 0x1000;
    sceVu0RotMatrixZ(matrix, D_00431AF0, entity->model->unkD0[2]);
    sceVu0RotMatrixX(matrix, matrix, entity->model->unkD0[0]);
    sceVu0RotMatrixY(matrix, matrix, entity->model->unkD0[1]);
    sceVu0ApplyMatrix(direction, (int *)matrix, direction);
    hit = func_0018AE98(200.0f, entity->model->position, direction, landing);
    if (hit == 1) {
        func_00216778(entity->model->position, landing);
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        unkE0->unk7A = 0;
        unkE0->unk08 = (void (*)(void))func_00182BD0;
        unkE0->unk14 = (void (*)(void))func_001818B8;
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8((char *)entity, unkE0->unk18, 1);
        }
    } else if (hit == 2 || hit == 3) {
        func_00216778(entity->model->position, landing);
        unkE0->unk48 = landing[0];
        unkE0->unk4C = landing[1];
        unkE0->unk50 = landing[2];
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        unkE0->unk7A = 1;
        unkE0->unk08 = (void (*)(void))func_00129738;
        unkE0->unk14 = (void (*)(void))func_00129780;
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8((char *)entity, unkE0->unk18, 1);
        }
    } else {
        KF_Vector_Scale(direction, direction, 20.0f);
        func_00216C48(entity->model->position, entity->model->position, direction);
    }
}
