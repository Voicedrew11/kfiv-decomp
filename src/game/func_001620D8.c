#include "kf_entity.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)
#define SCRATCH_MATRIX ((f32 *)0x70000010)

extern f32 D_00431AF0[16];
// match: the size is unknown (a complete small type would be read through $gp) and it is
// const: the store to unk54 may then be scheduled after the lui of this address
extern const f32 D_00446820[];

int func_00192C98(void *, int, int, u32, int);
f32 func_0018A9F0(void *, void *);
float func_00194FB8(float);
float func_00195018(float, float, float);
s16 func_00190BE0(Entity *);
void func_001958B8(char *, void (*)(void), int);
void sceVu0RotMatrixY(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);

void func_001620D8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;
    s32 run = FALSE;

    entity->unkC6 |= 0x1000;
    entity->flags &= ~0x3800;
    entity->flags &= ~0x200;
    height = func_0018A9F0(entity->unk14, entity->model->position);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, entity->params->unkC4, 1);
        unkE0->unk50 = func_00194FB8(entity->model->unkD0[1] + 3.1415927f);
        unkE0->unk54 = unkE0->unk48;
        unkE0->unk58 = 0.0f;
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
        unkE0->unk4C = 0.0f;
        unkE0->unk1C = 0.0f;
        entity->action_step++;
    case 1:
        func_00192C98(entity, 8, 0, entity->params->unkC4, 0);
        unkE0->unk58 = func_00195018(unkE0->unk58, entity->params->unk54, entity->params->unk50);
        unkE0->unk54 = func_00195018(unkE0->unk54, -20.0f, 2.0f);
        if (height == D_00446820[0] || height >= 400.0f) {
            entity->action_step++;
        }
        break;
    case 2:
        func_00192C98(entity, 4, 0, entity->params->unkC4, 0);
        unkE0->unk58 = func_00195018(unkE0->unk58, entity->params->unk54, entity->params->unk50);
        unkE0->unk54 = func_00195018(unkE0->unk54, 20.0f, 2.0f);
        if (unkE0->unk54 > 0.0f) {
            entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
            entity->unkC8 |= 0x400;
        }
        if (entity->flags & (0x8200L << 35)) {
            func_00192C98(entity, 9, 0, 0, 1);
            unkE0->unk58 = unkE0->unk54 = 0.0f;
            entity->action_step++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
    SCRATCH_VECTOR[1] = unkE0->unk54;
    SCRATCH_VECTOR[2] = unkE0->unk58 + unkE0->unk58;
    SCRATCH_VECTOR[0] = SCRATCH_VECTOR[3] = 0.0f;
    sceVu0RotMatrixY(SCRATCH_MATRIX, D_00431AF0, unkE0->unk50);
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
