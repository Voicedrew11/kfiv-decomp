#include "kf_entity.h"

extern u32 D_00446ED4;
extern f32 D_004467FC[];

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
f32 func_0018B518(f32 *);
int func_00182DE0(void *, int);
void func_0012F448(Entity *, s32);
void func_0012F638(Entity *, s32);

void func_0012F2F8(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (func_0018B518(entity->model->position) != D_004467FC[0]) {
        ENTITY_FLAG_BITS(entity)->bits60 = 5;
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        func_0012F638(entity, unknown);
        return;
    }
    func_0012F448(entity, unknown);
}
