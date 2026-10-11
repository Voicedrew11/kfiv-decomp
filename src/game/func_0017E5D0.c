#include "kf_entity.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)
#define SCRATCH_MATRIX ((f32 *)0x70000010)

extern f32 D_00431AF0[16];

float func_00195388(float *, float, float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);

/* moves a flying entity: turn, roll into the turn, pitch towards the level unk79 asks for, then speed */
void func_0017E5D0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 roll;
    f32 step;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->unk5C) {
            unkE0->unk1C = -entity->params->unk5C;
        }
        roll = (unkE0->unk54 + 50.0f) * 0.017453292f;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->unk5C < unkE0->unk1C) {
            unkE0->unk1C = entity->params->unk5C;
        }
        roll = (-50.0f - unkE0->unk54) * 0.017453292f;
    } else {
        if (entity->params->unk58 < unkE0->unk1C) {
            unkE0->unk1C -= entity->params->unk58;
        } else if (unkE0->unk1C < -entity->params->unk58) {
            unkE0->unk1C += entity->params->unk58;
        } else {
            unkE0->unk1C = 0.0f;
        }
        roll = 0.0f;
    }
    entity->unk94 += unkE0->unk1C;
    func_00195388(&entity->model->unkD0[2], roll, 60.0f);
    if (unkE0->unk79 > 0) {
        if (ABS((f32)unkE0->unk79) * -0.17453292f < entity->model->unkD0[0]) {
            unkE0->unk48[1] -= 0.006981317f;
            if (unkE0->unk48[1] < -0.017453292f) {
                unkE0->unk48[1] = -0.017453292f;
            }
        } else {
            unkE0->unk48[1] = 0.0f;
        }
    } else if (unkE0->unk79 < 0) {
        if (entity->model->unkD0[0] < ABS((f32)unkE0->unk79) * 0.17453292f) {
            unkE0->unk48[1] += 0.006981317f;
            if (0.017453292f < unkE0->unk48[1]) {
                unkE0->unk48[1] = 0.017453292f;
            }
        } else {
            unkE0->unk48[1] = 0.0f;
        }
    } else if (unkE0->unk79 == 0) {
        if (0.017453292f < entity->model->unkD0[0]) {
            unkE0->unk48[1] = -0.017453292f;
        } else if (entity->model->unkD0[0] < -0.017453292f) {
            unkE0->unk48[1] = 0.017453292f;
        } else {
            unkE0->unk48[1] = -entity->model->unkD0[0];
        }
    }
    entity->unk90 += unkE0->unk48[1];
    SCRATCH_VECTOR[0] = SCRATCH_VECTOR[1] = SCRATCH_VECTOR[2] = SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50;
    if (entity->unkC8 & 1) {
        unkE0->unk48[0] -= step;
        if (unkE0->unk48[0] < -entity->params->unk54) {
            unkE0->unk48[0] = -entity->params->unk54;
        }
    } else if (entity->unkC8 & 2) {
        unkE0->unk48[0] += step;
        if (entity->params->unk54 < unkE0->unk48[0]) {
            unkE0->unk48[0] = entity->params->unk54;
        }
    } else if (step < unkE0->unk48[0]) {
        unkE0->unk48[0] -= step;
    } else if (unkE0->unk48[0] < -step) {
        unkE0->unk48[0] += step;
    } else {
        unkE0->unk48[0] = 0.0f;
    }
    if (entity->unkC8 & 4) {
        unkE0->unk44 += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk44) {
            unkE0->unk44 = entity->params->unk54;
        }
    } else if (entity->unkC8 & 8) {
        unkE0->unk44 -= entity->params->unk50;
        if (unkE0->unk44 < -entity->params->unk54) {
            unkE0->unk44 = -entity->params->unk54;
        }
    } else if (entity->params->unk50 < unkE0->unk44) {
        unkE0->unk44 -= entity->params->unk50;
    } else if (unkE0->unk44 < -entity->params->unk50) {
        unkE0->unk44 += entity->params->unk50;
    } else {
        unkE0->unk44 = 0.0f;
    }
    SCRATCH_VECTOR[0] = unkE0->unk44;
    SCRATCH_VECTOR[1] = 0.0f;
    SCRATCH_VECTOR[2] = unkE0->unk48[0];
    sceVu0RotMatrixZ(SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
    sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, entity->model->unkD0[0]);
    sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, entity->model->unkD0[1]);
    func_00216BC0(SCRATCH_VECTOR, SCRATCH_MATRIX, SCRATCH_VECTOR);
    // match: a test whose result is never used (func_0016C888 doubles the speed on this bit); it leaves
    // no code, but the scratchpad reads below only get their own address loads when it is here
    if (entity->unkC8 & 0x40) {
        step += step;
    }
    entity->unk84 += SCRATCH_VECTOR[0];
    entity->unk88 += SCRATCH_VECTOR[1];
    entity->unk8C += SCRATCH_VECTOR[2];
}
