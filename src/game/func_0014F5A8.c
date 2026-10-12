#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
float func_00194F80(void *, void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_0014F6E8(Entity *, s32);

void func_0014F5A8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (func_00194F80(entity->model->position, entity->spawn_info) > 160.0f) {
        ENTITY_FLAG_BITS(entity)->bit25 = 0;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 2:
                func_0014F6E8(entity, unknown);
                break;
            case 0:
            case 1:
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    }
}
