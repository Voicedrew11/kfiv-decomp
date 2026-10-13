#include "kf_entity.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)
#define SCRATCH_MATRIX ((f32 *)0x70000010)

extern f32 D_00431AF0[16];
extern PlayerData D_00413F80;

void sceVu0RotMatrixY(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void func_00216D20(void *, void *);
void Vector_Scale(float *, float *, float);
float func_00199E68(Entity *, float);
float func_001954A8(float *, float *);
int func_00192C98(void *, int, int, u32, int);

void func_0014BCF0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 heading;
    f32 amount;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    SCRATCH_VECTOR[0] = SCRATCH_VECTOR[1] = SCRATCH_VECTOR[2] = SCRATCH_VECTOR[3] = 0.0f;
    vector = SCRATCH_VECTOR;
    if (entity->unkC8 & 3) {
        amount = 0.0f;
        if (entity->unkC8 & 1) {
            amount = -1.0f;
        } else if (entity->unkC8 & 2) {
            amount = 1.0f;
        }
        vector[2] += amount;
    }
    if (entity->unkC8 & 0xC) {
        amount = 0.0f;
        if (entity->unkC8 & 4) {
            amount = 1.0f;
        } else if (entity->unkC8 & 8) {
            amount = -1.0f;
        }
        vector[0] += amount;
    }
    if (entity->unkC8 & 0xF) {
        unkE0->unk44 = func_00199E68(entity, unkE0->unk44);
        amount = unkE0->unk44;
        if (entity->unkC8 & 0x40) {
            amount += amount;
        }
        if (entity->target != NULL) {
            heading = func_001954A8(entity->model->position, entity->target);
            sceVu0RotMatrixY(SCRATCH_MATRIX, D_00431AF0, heading);
            func_00216BC0(SCRATCH_VECTOR, SCRATCH_MATRIX, SCRATCH_VECTOR);
            func_00216D20(SCRATCH_VECTOR, SCRATCH_VECTOR);
            Vector_Scale(SCRATCH_VECTOR, SCRATCH_VECTOR, amount);
            entity->unk84 += SCRATCH_VECTOR[0];
            entity->unk88 += SCRATCH_VECTOR[1];
            entity->unk8C += SCRATCH_VECTOR[2];
        }
    } else {
        unkE0->unk44 = 0.0f;
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
}
