#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196C28(char *);
void func_0018E9C0(void *);
f32 func_0018B518(f32 *);
void func_0012BE50(Entity *, s32);
void func_0012C088(Entity *, s32);
void func_0012C1D0(Entity *, s32);

void func_0012BD18(Entity *entity, s32 unknown) {
    f32 floor_distance;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196C28((char *)entity);
    func_0018E9C0(entity);
    floor_distance = func_0018B518(entity->model->position);
    if (floor_distance < entity->unk30) {
        entity->model->position[1] -= entity->unk30 - floor_distance;
    }
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_0012BE50(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        func_0012C1D0(entity, unknown);
        return;
    }
    func_0012C088(entity, unknown);
}
