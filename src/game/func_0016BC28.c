#include "kf_entity.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)
#define SCRATCH_MATRIX ((f32 *)0x70000010)

extern f32 D_00431AF0[16];
extern const f32 D_0044682C[];

int func_00192C98(void *, int, int, u32, int);
f32 func_0018A9F0(void *, void *);
float func_00194FB8(float);
float func_00195018(float, float, float);
void func_001958B8(char *, void (*)(void), int);
void sceVu0RotMatrixY(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);

void func_0016BC28(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;
    s32 run = FALSE;

    entity->unkC6 |= 0x1000;
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    height = func_0018A9F0(entity->unk14, entity->model->position);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, entity->params->unkC4, 1);
        unkE0->unk58 = 0.0f;
        unkE0->unk5C = 0.0f;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 0x11) {
            unkE0->unk54 = func_00194FB8(entity->model->unkD0[1] + 3.1415927f);
            unkE0->unk58 = -entity->params->unk438;
            unkE0->unk5C = 0.0f;
            unkE0->unk1C = 0.0f;
            entity->action_step++;
        }
        break;
    case 3:
        func_00192C98(entity, 4, 0, 0, 0);
        unkE0->unk5C = func_00195018(unkE0->unk5C, entity->params->unk54, entity->params->unk50);
        unkE0->unk58 = func_00195018(unkE0->unk58, -entity->params->unk43C, -entity->params->unk43A);
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        if (height == D_0044682C[0] || height >= entity->params->unk43E) {
            entity->action_step++;
        }
        break;
    case 4:
        unkE0->unk5C = func_00195018(unkE0->unk5C, entity->params->unk54, entity->params->unk50);
        unkE0->unk58 = func_00195018(unkE0->unk58, entity->params->unk43C, entity->params->unk43A);
        if (ENTITY_FLAGS(entity) & (0x8200L << 35)) {
            func_00192C98(entity, 9, 0, 0, 1);
            unkE0->unk58 = 0.0f;
            unkE0->unk5C = 0.0f;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
        }
        break;
    case 5:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
    SCRATCH_VECTOR[1] = unkE0->unk58;
    SCRATCH_VECTOR[2] = unkE0->unk5C;
    SCRATCH_VECTOR[0] = SCRATCH_VECTOR[3] = 0.0f;
    sceVu0RotMatrixY(SCRATCH_MATRIX, D_00431AF0, unkE0->unk54);
    func_00216BC0(SCRATCH_VECTOR, SCRATCH_MATRIX, SCRATCH_VECTOR);
    // match: never taken, but until the compiler works that out the branch separates the
    // scratchpad stores above from the loads below, so their addresses are not kept in registers
    if (run) {
        KF_Vector_Scale(SCRATCH_VECTOR, SCRATCH_VECTOR, 2.0f);
    }
    entity->unk84 += SCRATCH_VECTOR[0];
    entity->unk88 += SCRATCH_VECTOR[1];
    entity->unk8C += SCRATCH_VECTOR[2];
}
