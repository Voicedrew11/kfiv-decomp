#include "kf_entity.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)
#define SCRATCH_MATRIX ((f32 *)0x70000010)

extern f32 D_00431AF0[16];
extern PlayerData D_00413F80;

void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void func_00216D20(void *, void *);
void Vector_Scale(float *, float *, float);
float func_00199E68(Entity *, float);
int func_00192C98(void *, int, int, u32, int);

/* movement of this kind: turn and move as the command bits in Entity.unkC8 say, and pick the animation to go with it
   (it has turn animations of its own) */
void func_00128438(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 amount;
    s32 command;
    s32 first_command;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    // match: read through the union, and again below once the turn speed is stored
    first_command = ENTITY_COMMAND(entity);
    if (first_command & 0xFCFF) {
        if (first_command & 0x100) {
            unkE0->unk1C -= entity->params->unk58;
            if (unkE0->unk1C < -entity->params->turn_limit) {
                unkE0->unk1C = -entity->params->turn_limit;
            }
        } else if (first_command & 0x200) {
            unkE0->unk1C += entity->params->unk58;
            if (entity->params->turn_limit < unkE0->unk1C) {
                unkE0->unk1C = entity->params->turn_limit;
            }
        } else {
            unkE0->unk1C = 0.0f;
        }
        command = entity->unkC8;
        turn = unkE0->unk1C;
        if (command & 0x400) {
            turn += turn;
        }
        entity->unk94 += turn;
    } else {
        command = first_command;
    }
    if (ENTITY_FLAG_BITS(entity)->bits11 == 0) {
        vector = SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
        if (command & 3) {
            amount = 0.0f;
            if (command & 1) {
                amount = -1.0f;
            } else if (command & 2) {
                amount = 1.0f;
            }
            vector[2] += amount;
        }
        if (command & 0xC) {
            amount = 0.0f;
            if (command & 4) {
                amount = 1.0f;
            } else if (command & 8) {
                amount = -1.0f;
            }
            vector[0] += amount;
        }
        if (command & 0x30) {
            amount = 0.0f;
            if (command & 0x10) {
                amount = -1.0f;
            } else if (command & 0x20) {
                amount = 1.0f;
            }
            vector[1] += amount;
        }
        if (command & 0x3F) {
            unkE0->unk44 = func_00199E68(entity, unkE0->unk44);
            amount = unkE0->unk44;
            if (entity->unkC8 & 0x40) {
                amount += amount;
            }
            sceVu0RotMatrixZ(SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
            sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, entity->model->unkD0[0]);
            sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, entity->model->unkD0[1]);
            func_00216BC0(SCRATCH_VECTOR, SCRATCH_MATRIX, SCRATCH_VECTOR);
            func_00216D20(SCRATCH_VECTOR, SCRATCH_VECTOR);
            Vector_Scale(SCRATCH_VECTOR, SCRATCH_VECTOR, amount);
            command = entity->unkC8;
            entity->unk84 += SCRATCH_VECTOR[0];
            entity->unk88 += SCRATCH_VECTOR[1];
            entity->unk8C += SCRATCH_VECTOR[2];
        } else {
            unkE0->unk44 = 0.0f;
        }
    } else {
        unkE0->unk44 = 0.0f;
    }
    if (command & 1) {
        if (command & 0x40) {
            func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (command & 2) {
        func_00192C98(entity, 5, 0, entity->params->unkC4, 0);
    } else if (command & 0x10) {
        func_00192C98(entity, 8, 0, entity->params->unkC4, 0);
    } else if (command & 0x20) {
        func_00192C98(entity, 9, 0, entity->params->unkC4, 0);
    } else if (command & 4) {
        func_00192C98(entity, 6, 0, entity->params->unkC4, 0);
    } else if (command & 8) {
        func_00192C98(entity, 7, 0, entity->params->unkC4, 0);
    } else if (command & 0x100) {
        func_00192C98(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (command & 0x200) {
        func_00192C98(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}
